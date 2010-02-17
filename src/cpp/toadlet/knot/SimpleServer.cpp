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
using namespace toadlet::knot::event;

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

void SimpleServer::addEventConnectionListener(EventConnectionListener *listener,bool notifyAboutCurrent){
	mListenersMutex.lock();
		mListeners.add(listener);
	mListenersMutex.unlock();

	if(notifyAboutCurrent){
		mClientsMutex.lock();
			int i;
			for(i=0;i<mClients.size();++i){
				listener->connected(mClients[i]);
			}
		mClientsMutex.unlock();
	}
}

void SimpleServer::removeEventConnectionListener(EventConnectionListener *listener,bool notifyAboutCurrent){
	if(notifyAboutCurrent){
		mClientsMutex.lock();
			int i;
			for(i=0;i<mClients.size();++i){
				listener->disconnected(mClients[i]);
			}
		mClientsMutex.unlock();
	}

	mListenersMutex.lock();
		mListeners.remove(listener);
	mListenersMutex.unlock();
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

EventConnection::ptr SimpleServer::getClient(EventConnection *connection){
	ServerClient::ptr client;
	mClientsMutex.lock();
		int i;
		for(i=0;i<mClients.size();++i){
			if(mClients[i]==connection){
				client=mClients[i];
				break;
			}
		}
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

void SimpleServer::connected(Connection::ptr connection){
	mClientsMutex.lock();
		ServerClient::ptr client(new ServerClient(this,mEventFactory,connection));

		mClients.add(client);
		mConnectionClients[connection]=client;
		notifyListenersConnected(client);
	mClientsMutex.unlock();
}

void SimpleServer::disconnected(Connection::ptr connection){
	mClientsMutex.lock();
		ServerClient::ptr client=mConnectionClients[connection];

		notifyListenersDisconnected(client);
		client->disconnected(connection);
		mConnectionClients.erase(connection);
		mClients.remove(client);
		mDeadClients.add(client);
	mClientsMutex.unlock();
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

void SimpleServer::notifyListenersConnected(EventConnection *connection){
	int i;
	Collection<EventConnectionListener*> listeners;

	mListenersMutex.lock();
		listeners.addAll(mListeners);
	mListenersMutex.unlock();

	for(i=0;i<listeners.size();++i){
		listeners[i]->connected(connection);
	}
}

void SimpleServer::notifyListenersDisconnected(EventConnection *connection){
	int i;
	Collection<EventConnectionListener*> listeners;

	mListenersMutex.lock();
		listeners.addAll(mListeners);
	mListenersMutex.unlock();

	for(i=0;i<listeners.size();++i){
		listeners[i]->disconnected(connection);
	}
}

}
}
