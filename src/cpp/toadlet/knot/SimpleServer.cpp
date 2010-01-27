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
#include <toadlet/knot/event/RoutedEvent.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace knot{

SimpleServer::ServerClient::ServerClient(SimpleServer *server,EventFactory *factory,Connection::ptr connection):SimpleEventConnection(factory,connection){
	mServer=server;
}

void SimpleServer::ServerClient::eventReceived(Event::ptr event){
	mServer->eventReceived(this,event);
}

SimpleServer::SimpleServer(EventFactory *eventFactory,Connector::ptr connector):
	mEventFactory(NULL)
	//mConnector,
	//mClients,
	//mConnectionClients,
	//mClientsMutex
{
	if(eventFactory!=NULL){
		setEventFactory(eventFactory);
	}

	if(connector!=NULL){
		setConnector(connector);
	}
}

SimpleServer::~SimpleServer(){
	setConnector(NULL);
}

void SimpleServer::setEventFactory(EventFactory *eventFactory){
	mEventFactory=eventFactory;

	mClientsMutex.lock();
		for(int i=0;i<mClients.size();++i){
			mClients[i]->setEventFactory(eventFactory);
		}
	mClientsMutex.unlock();
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
	mClientsMutex.lock();
		ServerClient::ptr client(new ServerClient(this,mEventFactory,connection));
		mClients.add(client);
		mConnectionClients[connection]=client;
	mClientsMutex.unlock();
}

void SimpleServer::disconnected(Connection::ptr connection){
	mClientsMutex.lock();
		ServerClient::ptr client=mConnectionClients[connection];
		mConnectionClients.erase(connection);
		mClients.remove(client);
	mClientsMutex.unlock();
}

bool SimpleServer::broadcast(Event::ptr event){
	bool result=false;
	mClientsMutex.lock();
		for(int i=0;i<mClients.size();++i){
			result|=mClients[i]->send(event);
		}
	mClientsMutex.unlock();
	return result;
}

bool SimpleServer::sendToClient(int clientID,egg::Event::ptr event){
	bool result=false;
	mClientsMutex.lock();
		result=mClients[clientID]->send(event);
	mClientsMutex.unlock();
	return result;
}

void SimpleServer::eventReceived(ServerClient *client,egg::Event::ptr event){
	if(event->getType()==Event::Type_ROUTED){
		RoutedEvent *routedEvent=(RoutedEvent*)event.get();
		sendToClient(routedEvent->getDestinationID(),event);
	}
}

}
}
