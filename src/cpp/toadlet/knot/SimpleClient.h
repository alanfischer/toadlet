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

#ifndef TOADLET_KNOT_SIMPLECLIENT_H
#define TOADLET_KNOT_SIMPLECLIENT_H

#include <toadlet/egg/Event.h>
#include <toadlet/egg/EventFactory.h>
#include <toadlet/egg/Thread.h>
#include <toadlet/egg/io/DataStream.h>
#include <toadlet/egg/io/MemoryStream.h>
#include <toadlet/knot/ClientEventConnection.h>

namespace toadlet{
namespace knot{

class TOADLET_API SimpleClient:public EventConnection,public ConnectorListener,public egg::Runnable{
public:
	TOADLET_SHARED_POINTERS(SimpleClient);

	SimpleClient(egg::EventFactory *eventFactory,Connector::ptr connector=NULL);
	virtual ~SimpleClient();

	bool opened(){return mRun;}
	void close();

	void setConnector(Connector::ptr connector);
	Connector::ptr getConnector(){return mConnector;}
	Connection::ptr getConnection(){return mConnection;}

	void connected(Connection::ptr connection);
	void disconnected(Connection::ptr connection);

	bool send(egg::Event::ptr event);
	bool sendToClient(int toClientID,egg::Event::ptr event);
	egg::Event::ptr receive();

	int getClientID(){return mClientID;}

	int update(){return 0;}

	void run();

protected:
	void eventReceived(int fromClientID,egg::Event::ptr event);

	int sendEvent(int toClientID,egg::Event::ptr event);
	int receiveEvent(int *fromClientID,egg::Event::ptr *event);

	int mClientID;

	egg::EventFactory *mEventFactory;
	egg::io::MemoryStream::ptr mPacketIn;
	egg::io::DataStream::ptr mDataPacketIn;
	egg::io::MemoryStream::ptr mPacketOut;
	egg::io::DataStream::ptr mDataPacketOut;

	Connector::ptr mConnector;
	Connection::ptr mConnection;
	egg::Collection<egg::Event::ptr> mEvents;
	egg::Mutex mEventsMutex;
	egg::Collection<int> mEventClientIDs;

	egg::Thread::ptr mThread;
	bool mRun;
};

}
}

#endif
