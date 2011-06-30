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

class TOADLET_API SimpleEventConnection:public EventConnection,public ConnectionListener,public Runnable{
public:
	TOADLET_SHARED_POINTERS(SimpleEventConnection);

	const static int CONTROL_EVENT_FLAG=0x4000; // Switch to 0x4000 from 0x8000 since setting the sign bit for a short ended up confusing the bit operations in java
	const static int CONTROL_EVENT_ROUTE=1;

	SimpleEventConnection(EventFactory *eventFactory=NULL,Connection::ptr connection=Connection::ptr());
	virtual ~SimpleEventConnection();

	virtual bool opened(){return mRun;}
	virtual void close();

	virtual void setEventFactory(EventFactory *eventFactory);
	virtual EventFactory *getEventFactory(){return mEventFactory;}

	virtual void setConnection(Connection::ptr connection);
	virtual Connection::ptr getConnection(){return mConnection;}

	virtual void addEventConnectionListener(EventConnectionListener *listener,bool notifyAboutCurrent);
	virtual void removeEventConnectionListener(EventConnectionListener *listener,bool notifyAboutCurrent);

	virtual bool send(Event::ptr event);
	virtual Event::ptr receive();

	virtual int update(){return 0;}

	virtual void connected(Connection::ptr connection);
	virtual void disconnected(Connection::ptr connection);

	virtual void run();

protected:
	virtual bool eventReceived(Event::ptr event);

	virtual void notifyListenersConnected(EventConnection *connection);
	virtual void notifyListenersDisconnected(EventConnection *connection);

	virtual int sendEvent(Event::ptr event);
	virtual int receiveEvent(Event::ptr *event);

	EventFactory *mEventFactory;
	MemoryStream::ptr mPacketIn;
	DataStream::ptr mDataPacketIn;
	MemoryStream::ptr mPacketOut;
	DataStream::ptr mDataPacketOut;

	Connection::ptr mConnection;
	Collection<Event::ptr> mEvents;
	Mutex mEventsMutex;
	Collection<EventConnectionListener*> mListeners;
	Mutex mListenersMutex;

	Thread::ptr mThread;
	bool mRun;
};

}
}

#endif
