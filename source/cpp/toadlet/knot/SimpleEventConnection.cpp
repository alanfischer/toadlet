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

#include <toadlet/egg/Logger.h>
#include <toadlet/egg/System.h>
#include <toadlet/knot/SimpleEventConnection.h>
#include <toadlet/knot/event/RoutedEvent.h>

namespace toadlet{
namespace knot{

SimpleEventConnection::SimpleEventConnection(EventFactory *eventFactory,Connection::ptr connection):
	mEventFactory(NULL),
	//mPacketIn,
	//mDataPacketIn,
	//mPacketOut,
	//mDataPacketOut,

	//mConnection,
	//mEvents,
	//mEventsMutex,
	//mEventClientIDs,

	//mThread,
	mRun(false)
{
	int maxSize=4096*16;
	mPacketOut=MemoryStream::ptr(new MemoryStream(new tbyte[maxSize],maxSize,0,true));
	mDataPacketOut=DataStream::ptr(new DataStream(Stream::ptr(mPacketOut)));
	mPacketIn=MemoryStream::ptr(new MemoryStream(new tbyte[maxSize],maxSize,maxSize,true));
	mDataPacketIn=DataStream::ptr(new DataStream(Stream::ptr(mPacketIn)));

	if(eventFactory!=NULL){
		setEventFactory(eventFactory);
	}

	if(connection!=NULL){
		setConnection(connection);
	}
}

SimpleEventConnection::~SimpleEventConnection(){
	close();
}

void SimpleEventConnection::close(){
	mRun=false;

	if(mConnection!=NULL){
		mConnection->close();
	}

	if(mThread!=NULL){
		Thread::ptr thread=mThread;
		mThread=NULL;
		thread->join();
	}
}

void SimpleEventConnection::setEventFactory(EventFactory *eventFactory){
	mEventsMutex.lock();
		mEventFactory=eventFactory;
	mEventsMutex.unlock();
}

void SimpleEventConnection::setConnection(Connection::ptr connection){
	if(mConnection!=NULL){
		disconnected(mConnection);
	}

	if(connection!=NULL){
		connected(connection);
	}
}

void SimpleEventConnection::addEventConnectionListener(EventConnectionListener *listener,bool notifyAboutCurrent){
	mListenersMutex.lock();
		mListeners.add(listener);
	mListenersMutex.unlock();

	if(notifyAboutCurrent){
		listener->connected(this);
	}
}

void SimpleEventConnection::removeEventConnectionListener(EventConnectionListener *listener,bool notifyAboutCurrent){
	if(notifyAboutCurrent){
		listener->disconnected(this);
	}

	mListenersMutex.lock();
		mListeners.remove(listener);
	mListenersMutex.unlock();
}

void SimpleEventConnection::connected(Connection::ptr connection){
	// Since we don't join() in disconnected, we join here just in case its still running somehow
	if(mThread!=NULL){
		mThread->join();
	}

	mConnection=connection;

	notifyListenersConnected(this);

	mThread=Thread::ptr(new Thread(this));
	mRun=true;
	mThread->start();
}

void SimpleEventConnection::disconnected(Connection::ptr connection){
	mRun=false;
	// We don't want to mThread.join() here, since mThread could be the one that is notifying us that we are disconnected

	notifyListenersDisconnected(this);
}

bool SimpleEventConnection::send(Event::ptr event){
	return sendEvent(event)>0;
}

Event::ptr SimpleEventConnection::receive(){	
	Event::ptr event;

	mEventsMutex.lock();
		int size=mEvents.size();
		if(size>0){
			event=mEvents.at(0);
			mEvents.removeAt(0);
		}
	mEventsMutex.unlock();

	return event;
}

void SimpleEventConnection::run(){
	while(mRun){
		Event::ptr event;
		int amount=receiveEvent(&event);
		if(amount>0){
			if(eventReceived(event)==false){
				mEventsMutex.lock();
					mEvents.add(event);
					event=Event::ptr();
				mEventsMutex.unlock();
			}
		}
		System::msleep(0);
	}
}

bool SimpleEventConnection::eventReceived(Event::ptr event){
	return false;
}

void SimpleEventConnection::notifyListenersConnected(EventConnection *connection){
	int i;
	Collection<EventConnectionListener*> listeners;

	mListenersMutex.lock();
		listeners.addAll(mListeners);
	mListenersMutex.unlock();

	for(i=0;i<listeners.size();++i){
		listeners[i]->connected(connection);
	}
}

void SimpleEventConnection::notifyListenersDisconnected(EventConnection *connection){
	int i;
	Collection<EventConnectionListener*> listeners;

	mListenersMutex.lock();
		listeners.addAll(mListeners);
	mListenersMutex.unlock();

	for(i=0;i<listeners.size();++i){
		listeners[i]->disconnected(connection);
	}
}

int SimpleEventConnection::sendEvent(Event::ptr event){
	Event *rootEvent=event->getRootEvent();
	int eventType=rootEvent->getType();
	if(event->getType()==Event::Type_ROUTED){
		eventType|=CONTROL_EVENT_FLAG;
	}
	mDataPacketOut->writeBInt16(eventType);
	if(event->getType()==Event::Type_ROUTED){
		RoutedEvent *routeEvent=(RoutedEvent*)event.get();
		mDataPacketOut->writeUInt8(CONTROL_EVENT_ROUTE);
		mDataPacketOut->writeBInt32(routeEvent->getSourceID());
		mDataPacketOut->writeBInt32(routeEvent->getDestinationID());
	}
	rootEvent->write(mDataPacketOut);

	int amount=-1;
	if(mConnection!=NULL){
		amount=mConnection->send(mPacketOut->getOriginalDataPointer(),mPacketOut->length());
		if(amount<mPacketOut->length()){
			amount=-1;
		}
	}

	mPacketOut->reset();

	return amount;
}

int SimpleEventConnection::receiveEvent(Event::ptr *event){
	int amount=-1;
	if(mConnection!=NULL){
		amount=mConnection->receive(mPacketIn->getOriginalDataPointer(),mPacketIn->length());
	}
	if(amount>0){
		RoutedEvent::ptr routedEvent=NULL;
		int eventType=mDataPacketIn->readBInt16();
		if((eventType&CONTROL_EVENT_FLAG)==CONTROL_EVENT_FLAG){
			eventType=eventType&~CONTROL_EVENT_FLAG;
			int type=mDataPacketIn->readUInt8();
			if(type==CONTROL_EVENT_ROUTE){
				int sourceID=mDataPacketIn->readBInt32();
				int destinationID=mDataPacketIn->readBInt32();
				routedEvent=RoutedEvent::ptr(new RoutedEvent(NULL,sourceID,destinationID));
			}
		}
		mEventsMutex.lock();
			if(mEventFactory!=NULL){
				*event=mEventFactory->createEventType(eventType);
			}
		mEventsMutex.unlock();
		if(*event!=NULL){
			(*event)->read(mDataPacketIn);
		}
		if(routedEvent!=NULL){
			routedEvent->setEvent(*event);
			(*event)=routedEvent;
		}

		mPacketIn->reset();
	}

	return amount;
}

}
}