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

bool SimpleServer::ServerClient::eventReceived(Event::ptr event){
	return mServer->eventReceived(this,event);
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

void SimpleServer::close(){
	if(mConnector!=NULL){
		mConnector->close();
	}

	egg::Collection<ServerClient::ptr> serverClients;
	mClientsMutex.lock();
		serverClients.addAll(mClients);
	mClientsMutex.unlock();
	int i;
	for(i=0;i<serverClients.size();++i){
		serverClients[i]->close();
	}
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
		mConnector->removeConnectionListener(this,true);
	}

	mConnector=connector;

	if(mConnector!=NULL){
		mConnector->addConnectionListener(this,true);
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
		client->disconnected(connection);
		mConnectionClients.erase(connection);
		mClients.remove(client);
		mDeadClients.add(client);
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

bool SimpleServer::sendToClient(int clientID,Event::ptr event){
	bool result=false;
	mClientsMutex.lock();
		result=mClients[clientID]->send(event);
	mClientsMutex.unlock();
	return result;
}

Event::ptr SimpleServer::receive(){
	Event::ptr event;
	mClientsMutex.lock();
		for(int i=0;i<mClients.size() && event==NULL;++i){
			event=mClients[i]->receive();
		}
	mClientsMutex.unlock();
	return event;	
}

EventConnection::ptr SimpleServer::getClient(int i){
	ServerClient::ptr client;
	mClientsMutex.lock();
		if(i>=0 && i<mClients.size()){
			client=mClients[i];
		}
	mClientsMutex.unlock();
	return client;
}

EventConnection::ptr SimpleServer::getClient(Connection::ptr connection){
	ServerClient::ptr client;
	mClientsMutex.lock();
		client=mConnectionClients.find(connection)->second;
	mClientsMutex.unlock();
	return client;
}

int SimpleServer::update(){
	mClientsMutex.lock();
		int i;
		for(i=0;i<mDeadClients.size();++i){
			mDeadClients[i]->close();
		}
		mDeadClients.clear();
	mClientsMutex.unlock();
	return 0;
}

bool SimpleServer::eventReceived(ServerClient *client,Event::ptr event){
	if(event->getType()==Event::Type_ROUTED){
		RoutedEvent *routedEvent=(RoutedEvent*)event.get();
		sendToClient(routedEvent->getDestinationID(),event);
		return true;
	}
	else{
		return false;
	}
}

}
}
