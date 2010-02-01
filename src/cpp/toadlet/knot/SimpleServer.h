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
#include <toadlet/knot/ConnectorListener.h>
#include <toadlet/knot/SimpleEventConnection.h>

namespace toadlet{
namespace knot{

class TOADLET_API SimpleServer:public ConnectorListener{
public:
	TOADLET_SHARED_POINTERS(SimpleServer);

	SimpleServer(egg::EventFactory *eventFactory,Connector::ptr connector=NULL);
	virtual ~SimpleServer();

	virtual void setEventFactory(egg::EventFactory *eventFactory);
	virtual egg::EventFactory *getEventFactory(){return mEventFactory;}

	virtual void setConnector(Connector::ptr connector);
	virtual Connector::ptr getConnector(){return mConnector;}

	virtual void connected(Connection::ptr connection);
	virtual void disconnected(Connection::ptr connection);

	virtual bool broadcast(egg::Event::ptr event);
	virtual bool sendToClient(int toClientID,egg::Event::ptr event);
	virtual egg::Event::ptr receive();
	virtual EventConnection::ptr getClient(int i);

protected:
	class ServerClient:public SimpleEventConnection{
	public:
		TOADLET_SHARED_POINTERS(ServerClient);

		ServerClient(SimpleServer *server,egg::EventFactory *factory,Connection::ptr connection);

	protected:
		void eventReceived(egg::Event::ptr event);

		SimpleServer *mServer;
	};

	virtual bool eventReceived(ServerClient *client,egg::Event::ptr event);

	egg::EventFactory *mEventFactory;
	Connector::ptr mConnector;
	egg::Collection<ServerClient::ptr> mClients;
	egg::Map<Connection::ptr,ServerClient::ptr> mConnectionClients;
	egg::Mutex mClientsMutex;
};

}
}

#endif
