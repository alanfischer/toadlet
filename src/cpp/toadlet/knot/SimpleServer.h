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

#include <toadlet/egg/EventFactory.h>
#include <toadlet/knot/ConnectorListener.h>

namespace toadlet{
namespace knot{

class TOADLET_API SimpleServer:public ConnectorListener{
public:
	TOADLET_SHARED_POINTERS(SimpleServer);

	SimpleServer(egg::EventFactory *eventFactory,Connector::ptr connector=NULL);
	virtual ~SimpleServer();

	void setConnector(Connector::ptr connector);
	Connector::ptr getConnector(){return mConnector;}

	void connected(Connection::ptr connection);
	void disconnected(Connection::ptr connection);

	bool broadcastEvent(egg::Event::ptr event);
	bool sendEvent(int clientID,egg::Event::ptr event,int fromClientID);
	bool receiveEvent(egg::Event::ptr &event,int &fromClientID);

protected:
	void eventReceived(int clientID,egg::Event::ptr event,int fromClientID);

	int mClientID;

	egg::EventFactory *mEventFactory;

	Connector::ptr mConnector;
	egg::Collection<Connection::ptr> mConnections;
	egg::Mutex mConnectionsMutex;
	egg::Collection<egg::Event::ptr> mEvents;
	egg::Mutex mEventsMutex;
	egg::Collection<int> mEventClientIDs;
};

}
}

#endif
