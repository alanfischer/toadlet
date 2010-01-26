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
#include <toadlet/knot/SimpleClient.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace knot{

SimpleClient::SimpleClient(EventFactory *eventFactory,Connector::ptr connector):
	mClientID(0),

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

	mEventFactory=eventFactory;

	if(connector!=NULL){
		setConnector(connector);
	}
}

SimpleClient::~SimpleClient(){
	close();
}

void SimpleClient::close(){
	if(mConnector!=NULL){
		mConnector->close();
	}

	setConnector(NULL);

	mRun=false;
	if(mThread!=NULL){
		mThread->join();
		mThread=NULL;
	}
}

void SimpleClient::setConnector(Connector::ptr connector){
	if(mConnector!=NULL){
		mConnector->removeConnectorListener(this,true);
	}

	mConnector=connector;

	if(mConnector!=NULL){
		mConnector->addConnectorListener(this,true);
	}
}

void SimpleClient::connected(Connection::ptr connection){
	mConnection=connection;

	mThread=Thread::ptr(new Thread(this));
	mRun=true;
	mThread->start();
}

void SimpleClient::disconnected(Connection::ptr connection){
	mRun=false;
	if(mThread!=NULL){
		mThread->join();
		mThread=NULL;
	}

	if(mConnection==connection){
		mConnection=NULL;
	}
}

bool SimpleClient::send(Event::ptr event){
	return sendEvent(-1,event)>0;
}

bool SimpleClient::sendToClient(int toClientID,Event::ptr event){
	return sendEvent(toClientID,event)>0;
}

Event::ptr SimpleClient::receive(){	
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

void SimpleClient::run(){
	while(mRun){
		Event::ptr event;
		int fromClientID;
		int amount=receiveEvent(&fromClientID,&event);
		if(amount>0){
			eventReceived(fromClientID,event);
		}
		else if(amount<0){
			if(mConnector!=NULL){
				mConnector->close();
			}
		}
		System::msleep(0);
	}
}

void SimpleClient::eventReceived(int fromClientID,Event::ptr event){
//	if(event->getType()==EventType_PING){
//		if(mEventFactory!=NULL){
//			event=mEventFactory->createEventType(type);
//		}
//		sendEventToClient(clientID,pongEvent);
//	}

	mEventsMutex.lock();
		mEvents.add(event);
		mEventClientIDs.add(fromClientID);
	mEventsMutex.unlock();
}

int SimpleClient::sendEvent(int toClientID,Event::ptr event){
//	mDataPacketOut->writeBigInt32(eventFrame);
	mDataPacketOut->writeBigInt32(toClientID);
	mDataPacketOut->writeUInt8(event->getType());
	event->write(mDataPacketOut);

	int amount=mConnection->send(mPacketOut->getOriginalDataPointer(),mPacketOut->length());

	mPacketOut->reset();

	return amount;
}

int SimpleClient::receiveEvent(int *fromClientID,Event::ptr *event){
	int amount=mConnection->receive(mPacketIn->getOriginalDataPointer(),mPacketIn->length());
	if(amount>0){
//		int eventFrame=mDataPacketIn->readBigInt32();
		*fromClientID=mDataPacketIn->readBigInt32();
		int type=mDataPacketIn->readUInt8();
		if(mEventFactory!=NULL){
			*event=mEventFactory->createEventType(type);
		}
		if(*event!=NULL){
			(*event)->read(mDataPacketIn);
		}

		mPacketIn->reset();
	}

	return amount;
}

}
}
