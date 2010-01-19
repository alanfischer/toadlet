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

#ifndef TOADLET_KNOT_SIMPLEEVENTCLIENT_H
#define TOADLET_KNOT_SIMPLEEVENTCLIENT_H

#include <toadlet/egg/Event.h>
#include <toadlet/egg/io/DataStream.h>
#include <toadlet/egg/io/MemoryStream.h>
#include <toadlet/knot/EventClient.h>

namespace toadlet{
namespace knot{

class TOADLET_API SimpleEventClient:public EventClient{
public:
	TOADLET_SHARED_POINTERS(SimpleEventClient);

	SimpleEventClient();
	virtual ~SimpleEventClient();

	void connected(Connection::ptr connection);
	void disconnected(Connection::ptr connection);
	void dataReady(Connection::ptr connection);

	bool sendEvent(egg::Event::ptr event);
	bool sendEventToClient(egg::Event::ptr event,int clientID);
	bool receiveEvent(egg::Event::ptr &event,int &clientID);
	
	int getClientID(){return mClientID;}

protected:
	int mClientID;

	egg::io::MemoryStream::ptr mPacketIn;
	egg::io::DataStream::ptr mDataPacketIn;
	egg::io::MemoryStream::ptr mPacketOut;
	egg::io::DataStream::ptr mDataPacketOut;

	Connection::ptr mConnection;
};

}
}

#endif
