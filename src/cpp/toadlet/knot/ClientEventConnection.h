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

#ifndef TOADLET_KNOT_CLIENTEVENTCONNECTION_H
#define TOADLET_KNOT_CLIENTEVENTCONNECTION_H

#include <toadlet/egg/Event.h>
#include <toadlet/knot/ConnectorListener.h>
#include <toadlet/knot/EventConnection.h>

namespace toadlet{
namespace knot{

class TOADLET_API ClientEventConnection:public EventConnection,public ConnectorListener{
public:
	TOADLET_SHARED_POINTERS(ClientEventConnection);

	virtual ~ClientEventConnection(){}

	virtual void setConnector(Connector::ptr connector)=0;
	virtual Connector::ptr getConnector()=0;

	virtual void connected(Connection::ptr connection)=0;
	virtual void disconnected(Connection::ptr connection)=0;

	virtual bool send(egg::Event::ptr event)=0;
	virtual egg::Event::ptr receive()=0;
	virtual bool sendToClient(int clientID,egg::Event::ptr event)=0;
	
	virtual int getClientID()=0;
};

}
}

#endif
