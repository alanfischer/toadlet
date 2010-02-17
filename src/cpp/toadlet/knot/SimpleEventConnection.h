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

#ifndef TOADLET_KNOT_SIMPLEEVENTCONNECTION_H
#define TOADLET_KNOT_SIMPLEEVENTCONNECTION_H

#include <toadlet/egg/Event.h>
#include <toadlet/egg/EventFactory.h>
#include <toadlet/egg/Mutex.h>
#include <toadlet/egg/Thread.h>
#include <toadlet/egg/io/DataStream.h>
#include <toadlet/egg/io/MemoryStream.h>
#include <toadlet/knot/EventConnection.h>
#include <toadlet/knot/EventConnectionListener.h>
#include <toadlet/knot/ConnectionListener.h>

namespace toadlet{
namespace knot{

class TOADLET_API SimpleEventConnection:public EventConnection,public ConnectionListener,public egg::Runnable{
public:
	TOADLET_SHARED_POINTERS(SimpleEventConnection);

	const static int CONTROL_EVENT_FLAG=0x4000; // Switch to 0x4000 from 0x8000 since setting the sign bit for a short ended up confusing the bit operations in java
	const static int CONTROL_EVENT_ROUTE=1;

	SimpleEventConnection(egg::EventFactory *eventFactory=NULL,Connection::ptr connection=Connection::ptr());
	virtual ~SimpleEventConnection();

	virtual bool opened(){return mRun;}
	virtual void close();

	virtual void setEventFactory(egg::EventFactory *eventFactory);
	virtual egg::EventFactory *getEventFactory(){return mEventFactory;}

	virtual void setConnection(Connection::ptr connection);
	virtual Connection::ptr getConnection(){return mConnection;}

	virtual void addEventConnectionListener(EventConnectionListener *listener,bool notifyAboutCurrent);
	virtual void removeEventConnectionListener(EventConnectionListener *listener,bool notifyAboutCurrent);

	virtual bool send(egg::Event::ptr event);
	virtual egg::Event::ptr receive();

	virtual int update(){return 0;}

	virtual void connected(Connection::ptr connection);
	virtual void disconnected(Connection::ptr connection);

	virtual void run();

protected:
	virtual bool eventReceived(egg::Event::ptr event);

	virtual void notifyListenersConnected(EventConnection *connection);
	virtual void notifyListenersDisconnected(EventConnection *connection);

	virtual int sendEvent(egg::Event::ptr event);
	virtual int receiveEvent(egg::Event::ptr *event);

	egg::EventFactory *mEventFactory;
	egg::io::MemoryStream::ptr mPacketIn;
	egg::io::DataStream::ptr mDataPacketIn;
	egg::io::MemoryStream::ptr mPacketOut;
	egg::io::DataStream::ptr mDataPacketOut;

	Connection::ptr mConnection;
	egg::Collection<egg::Event::ptr> mEvents;
	egg::Mutex mEventsMutex;
	egg::Collection<EventConnectionListener*> mListeners;
	egg::Mutex mListenersMutex;

	egg::Thread::ptr mThread;
	bool mRun;
};

}
}

#endif
