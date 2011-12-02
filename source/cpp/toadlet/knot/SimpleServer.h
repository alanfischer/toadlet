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

#ifndef TOADLET_KNOT_SIMPLESERVER_H
#define TOADLET_KNOT_SIMPLESERVER_H

#include <toadlet/egg/Map.h>
#include <toadlet/egg/EventFactory.h>
#include <toadlet/knot/Connector.h>
#include <toadlet/knot/ConnectionListener.h>
#include <toadlet/knot/EventConnectionListener.h>
#include <toadlet/knot/SimpleEventConnection.h>

namespace toadlet{
namespace knot{

class TOADLET_API SimpleServer:public ConnectionListener{
public:
	TOADLET_SHARED_POINTERS(SimpleServer);

	SimpleServer(EventFactory *eventFactory=NULL,Connector::ptr connector=Connector::ptr());
	virtual ~SimpleServer();

	virtual void close();
	virtual void setEventFactory(EventFactory *eventFactory);
	virtual EventFactory *getEventFactory(){return mEventFactory;}

	virtual void setConnector(Connector::ptr connector);
	virtual Connector::ptr getConnector(){return mConnector;}

	virtual void addEventConnectionListener(EventConnectionListener *listener,bool notifyAboutCurrent);
	virtual void removeEventConnectionListener(EventConnectionListener *listener,bool notifyAboutCurrent);

	virtual bool broadcast(Event::ptr event);
	virtual bool sendToClient(int toClientID,Event::ptr event);
	virtual Event::ptr receive();
	virtual EventConnection::ptr getClient(int i);
	virtual EventConnection::ptr getClient(Connection::ptr connection);
	virtual EventConnection::ptr getClient(EventConnection *connection); // Only needed to get the smart pointer

	virtual int update();

	virtual void connected(Connection::ptr connection);
	virtual void disconnected(Connection::ptr connection);

protected:
	class ServerClient:public SimpleEventConnection{
	public:
		TOADLET_SHARED_POINTERS(ServerClient);

		ServerClient(SimpleServer *server,EventFactory *factory,Connection::ptr connection);

	protected:
		bool eventReceived(Event::ptr event);

		SimpleServer *mServer;
	};

	virtual bool eventReceived(ServerClient *client,Event::ptr event);
	
	virtual void notifyListenersConnected(EventConnection *connection);
	virtual void notifyListenersDisconnected(EventConnection *connection);

	EventFactory *mEventFactory;
	Connector::ptr mConnector;
	Collection<ServerClient::ptr> mClients;
	Collection<ServerClient::ptr> mDeadClients;
	Map<Connection::ptr,ServerClient::ptr> mConnectionClients;
	Mutex mClientsMutex;
	Collection<EventConnectionListener*> mListeners;
	Mutex mListenersMutex;
};

}
}

#endif
