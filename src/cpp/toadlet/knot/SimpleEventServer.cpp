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

#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/knot/SimpleEventServer.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace knot{

SimpleEventServer::SimpleEventServer(EventFactory *eventFactory,Connector::ptr connector):
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

SimpleEventServer::~SimpleEventServer(){
	setConnector(NULL);
}

void SimpleEventServer::setConnector(Connector::ptr connector){
	if(mConnector!=NULL){
		mConnector->removeConnectorListener(this,true);
	}

	mConnector=connector;

	if(mConnector!=NULL){
		mConnector->addConnectorListener(this,true);
	}
}

void SimpleEventServer::connected(Connection::ptr connection){
	mConnectionsMutex.lock();
		mConnections.add(connection);
	mConnectionsMutex.unlock();
}

void SimpleEventServer::disconnected(Connection::ptr connection){
	mConnectionsMutex.lock();
		mConnections.remove(connection);
	mConnectionsMutex.unlock();
}

void SimpleEventServer::dataReady(Connection *connection){
	// TODO: Work in separate objects with their own dataReady, so we don't have to search for the proper client ID
	int fromClientID=-1;
	mConnectionsMutex.lock();
		int i;
		for(i=0;i<mConnections.size();++i){
			if(mConnections[i]==connection){
				fromClientID=i;
				break;
			}
		}
	mConnectionsMutex.unlock();

	int amount=connection->receive(mPacketIn->getOriginalDataPointer(),mPacketIn->length());
//	int eventFrame=mDataPacketIn->readBigInt32();

	int clientID=mDataPacketIn->readBigInt32();
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

	eventReceived(clientID,event,fromClientID);
}

bool SimpleEventServer::broadcastEvent(Event::ptr event){
	int fromClientID=-1;

//	mDataPacketOut->writeBigInt32(eventFrame);
	mDataPacketOut->writeBigInt32(fromClientID);
	mDataPacketOut->writeUInt8(event->getType());
	event->write(mDataPacketOut);

	mConnectionsMutex.lock();
		int i;
		for(i=0;i<mConnections.size();++i){
			mConnections[i]->send(mPacketOut->getOriginalDataPointer(),mPacketOut->length());
		}
	mConnectionsMutex.unlock();

	mPacketOut->reset();

	return true;
}

bool SimpleEventServer::sendEvent(int clientID,Event::ptr event,int fromClientID){
//	mDataPacketOut->writeBigInt32(eventFrame);
	mDataPacketOut->writeBigInt32(fromClientID);
	mDataPacketOut->writeUInt8(event->getType());
	event->write(mDataPacketOut);

	int amount=0;
	mConnectionsMutex.lock();
		if(clientID<0 || clientID>=mConnections.size()){
			mConnectionsMutex.unlock();
			mPacketOut->reset();
			Error::invalidParameters("invalid clientID");
			return false;
		}

		amount=mConnections[clientID]->send(mPacketOut->getOriginalDataPointer(),mPacketOut->length());
	mConnectionsMutex.unlock();

	mPacketOut->reset();

	return amount>0;
}

bool SimpleEventServer::receiveEvent(egg::Event::ptr &event,int &fromClientID){
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

void SimpleEventServer::eventReceived(int clientID,egg::Event::ptr event,int fromClientID){
	if(clientID!=-1){
		sendEvent(clientID,event,fromClientID);
	}
	else{
		mEventsMutex.lock();
			mEvents.add(event);
			mEventClientIDs.add(fromClientID);
		mEventsMutex.unlock();
	}
}

}
}
