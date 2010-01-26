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
#include <toadlet/knot/SimpleServer.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace knot{

SimpleServer::SimpleServer(EventFactory *eventFactory,Connector::ptr connector):
	mEventFactory(NULL)

	//mConnection,
	//mEvents,
	//mEventsMutex,
	//mEventClientIDs
{
	mEventFactory=eventFactory;
	if(connector!=NULL){
		setConnector(connector);
	}
}

SimpleServer::~SimpleServer(){
	setConnector(NULL);
}

void SimpleServer::setConnector(Connector::ptr connector){
	if(mConnector!=NULL){
		mConnector->removeConnectorListener(this,true);
	}

	mConnector=connector;

	if(mConnector!=NULL){
		mConnector->addConnectorListener(this,true);
	}
}

void SimpleServer::connected(Connection::ptr connection){
	mConnectionsMutex.lock();
		mConnections.add(connection);
	mConnectionsMutex.unlock();
}

void SimpleServer::disconnected(Connection::ptr connection){
	mConnectionsMutex.lock();
		mConnections.remove(connection);
	mConnectionsMutex.unlock();
}
/*
bool SimpleServer::broadcastEvent(Event::ptr event){
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

bool SimpleServer::sendEvent(int clientID,Event::ptr event,int fromClientID){
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

bool SimpleServer::receiveEvent(egg::Event::ptr &event,int &fromClientID){
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

void SimpleServer::eventReceived(int clientID,egg::Event::ptr event,int fromClientID){
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
*/
}
}
