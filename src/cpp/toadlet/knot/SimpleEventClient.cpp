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
#include <toadlet/knot/SimpleEventClient.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace knot{

SimpleEventClient::SimpleEventClient(EventFactory *eventFactory,Connector::ptr connector):
	mClientID(0),

	mEventFactory(NULL)
	//mPacketIn,
	//mDataPacketIn,
	//mPacketOut,
	//mDataPacketOut,

	//mConnection,
	//mEvents,
	//mEventsMutex,
	//mEventClientIDs
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

SimpleEventClient::~SimpleEventClient(){
	setConnector(NULL);
}

void SimpleEventClient::setConnector(Connector::ptr connector){
	if(mConnector!=NULL){
		mConnector->removeConnectorListener(this,true);
	}

	mConnector=connector;

	if(mConnector!=NULL){
		mConnector->addConnectorListener(this,true);
	}
}

void SimpleEventClient::connected(Connection::ptr connection){
	mConnection=connection;
}

void SimpleEventClient::disconnected(Connection::ptr connection){
	if(mConnection==connection){
		mConnection=NULL;
	}
}

void SimpleEventClient::dataReady(Connection *connection){
	int amount=connection->receive(mPacketIn->getOriginalDataPointer(),mPacketIn->length());
//	int eventFrame=mDataPacketIn->readBigInt32();

	int fromClientID=mDataPacketIn->readBigInt32();
	int type=mDataPacketIn->readUInt8();
	Event::ptr event;
	if(mEventFactory!=NULL){
		event=mEventFactory->createEventType(type);
	}
	if(event==NULL){
		Logger::warning(Categories::TOADLET_KNOT,
			String("Received unknown event type:")+type);
	}
	else{
		event->read(mDataPacketIn);
	}

	mPacketIn->reset();

	eventReceived(event,fromClientID);
}

bool SimpleEventClient::sendEvent(Event::ptr event){
	return sendEventToClient(-1,event);
}

bool SimpleEventClient::sendEventToClient(int clientID,Event::ptr event){
//	mDataPacketOut->writeBigInt32(eventFrame);
	mDataPacketOut->writeBigInt32(clientID);
	mDataPacketOut->writeUInt8(event->getType());
	event->write(mDataPacketOut);

	int amount=mConnection->send(mPacketOut->getOriginalDataPointer(),mPacketOut->length());

	mPacketOut->reset();

//	mLocalEvents.add(event);
//	mLocalEventFrames.add(eventFrame);

	return amount>0;
}

bool SimpleEventClient::receiveEvent(Event::ptr &event,int &fromClientID){	
	mEventsMutex.lock();
		int size=mEvents.size();
		if(size>0){
			event=mEvents.at(size-1);
			mEvents.removeAt(size-1);
			fromClientID=mEventClientIDs.at(size-1);
			mEventClientIDs.removeAt(size-1);
		}
	mEventsMutex.unlock();

	return size>0;
}

void SimpleEventClient::eventReceived(Event::ptr event,int fromClientID){
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

//void SimpleEventClient::run(){
//}

}
}
