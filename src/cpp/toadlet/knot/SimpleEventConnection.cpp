/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright 2009, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer, Andrew Fischer
 *
 * This file is part of The Toadlet Engine.
 *
 * The Toadlet Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * The Toadlet Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with The Toadlet Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 ********** Copyright header - do not remove **********/

#include <toadlet/egg/Logger.h>
#include <toadlet/egg/System.h>
#include <toadlet/knot/SimpleEventConnection.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace knot{

SimpleEventConnection::SimpleEventConnection(EventFactory *eventFactory,Connection::ptr connection):
	mEventFactory(NULL),
	//mPacketIn,
	//mDataPacketIn,
	//mPacketOut,
	//mDataPacketOut,

	//mConnection,
	//mEvents,
	//mEventsMutex,
	//mEventClientIDs,

	//mThread,
	mRun(false)
{
	mPacketIn=MemoryStream::ptr(new MemoryStream(new byte[1024],1024,1024,true));
	mDataPacketIn=DataStream::ptr(new DataStream(Stream::ptr(mPacketIn)));
	mPacketOut=MemoryStream::ptr(new MemoryStream());
	mDataPacketOut=DataStream::ptr(new DataStream(Stream::ptr(mPacketOut)));

	if(eventFactory!=NULL){
		setEventFactory(eventFactory);
	}

	if(connection!=NULL){
		setConnection(connection);
	}
}

SimpleEventConnection::~SimpleEventConnection(){
	close();
}

void SimpleEventConnection::close(){
	mRun=false;
	if(mThread!=NULL){
		mThread->join();
		mThread=NULL;
	}
}

void SimpleEventConnection::setEventFactory(EventFactory *eventFactory){
	mEventsMutex.lock();
		mEventFactory=eventFactory;
	mEventsMutex.unlock();
}

void SimpleEventConnection::setConnection(Connection::ptr connection){
	if(mConnection!=NULL){
		disconnected(mConnection);
	}

	if(connection!=NULL){
		connected(connection);
	}
}

void SimpleEventConnection::connected(Connection::ptr connection){
	mConnection=connection;

	mThread=Thread::ptr(new Thread(this));
	mRun=true;
	mThread->start();
}

void SimpleEventConnection::disconnected(Connection::ptr connection){
	mRun=false;
	if(mThread!=NULL){
		mThread->join();
		mThread=NULL;
	}

	if(mConnection==connection){
		mConnection=NULL;
	}
}

bool SimpleEventConnection::send(Event::ptr event){
	return sendEvent(-1,event)>0;
}

Event::ptr SimpleEventConnection::receive(){	
	Event::ptr event;
	int fromClientID=0;

	mEventsMutex.lock();
		int size=mEvents.size();
		if(size>0){
			event=mEvents.at(size-1);
			mEvents.removeAt(size-1);
			fromClientID=mEventClientIDs.at(size-1);
			mEventClientIDs.removeAt(size-1);
		}
	mEventsMutex.unlock();

	return event;
}

void SimpleEventConnection::run(){
	while(mRun){
		Event::ptr event;
		int fromClientID;
		int amount=receiveEvent(&fromClientID,&event);
		if(amount>0){
			eventReceived(fromClientID,event);
		}
		else if(amount<0){
			receiveError();
		}
		System::msleep(0);
	}
}

void SimpleEventConnection::eventReceived(int fromClientID,Event::ptr event){
	mEventsMutex.lock();
		mEvents.add(event);
		mEventClientIDs.add(fromClientID);
	mEventsMutex.unlock();
}

void SimpleEventConnection::receiveError(){
}

int SimpleEventConnection::sendEvent(int toClientID,Event::ptr event){
//	mDataPacketOut->writeBigInt32(eventFrame);
	mDataPacketOut->writeBigInt32(toClientID);
	mDataPacketOut->writeUInt8(event->getType());
	event->write(mDataPacketOut);

	int amount=mConnection->send(mPacketOut->getOriginalDataPointer(),mPacketOut->length());

	mPacketOut->reset();

	return amount;
}

int SimpleEventConnection::receiveEvent(int *fromClientID,Event::ptr *event){
	int amount=mConnection->receive(mPacketIn->getOriginalDataPointer(),mPacketIn->length());
	if(amount>0){
//		int eventFrame=mDataPacketIn->readBigInt32();
		*fromClientID=mDataPacketIn->readBigInt32();
		int type=mDataPacketIn->readUInt8();
		mEventsMutex.lock();
			if(mEventFactory!=NULL){
				*event=mEventFactory->createEventType(type);
			}
		mEventsMutex.unlock();
		if(*event!=NULL){
			(*event)->read(mDataPacketIn);
		}

		mPacketIn->reset();
	}

	return amount;
}

}
}
