#if 1

#include <toadlet/egg/Logger.h>
#include <toadlet/egg/System.h>
#include <toadlet/knot/TCPConnector.h>
#include <toadlet/knot/DebugListener.h>
#include <toadlet/knot/SimpleClient.h>
#include <toadlet/knot/SimpleServer.h>
#include "../quicktest.h"

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::net;
using namespace toadlet::knot;

class MessageEvent:public Event{
public:
	enum{
		EventType_MESSAGE=101
	};

	MessageEvent():Event(EventType_MESSAGE){}

	MessageEvent(const String &text):Event(EventType_MESSAGE){
		mText=text;
	}

	void setText(const String &text){mText=text;}
	const String getText() const{return mText;}

	int read(DataStream *stream){return stream->readBigInt16String(mText);}
	int write(DataStream *stream){return stream->writeBigInt16String(mText);}

protected:
	String mText;
};

class SimpleEventFactory:public EventFactory{
public:
	TOADLET_SHARED_POINTERS(SimpleEventFactory);

	Event::ptr createEventType(int type){
		if(type==MessageEvent::EventType_MESSAGE){
			return Event::ptr(new MessageEvent());
		}
		else{
			return Event::ptr(new Event());
		}
	}
};

QT_TEST(ClientClientTest){
	SimpleEventFactory::ptr eventFactory(new SimpleEventFactory());

	SimpleClient::ptr client1(new SimpleClient(eventFactory,Connector::ptr(new TCPConnector(5252))));
	SimpleClient::ptr client2(new SimpleClient(eventFactory,Connector::ptr(new TCPConnector(Socket::stringToIP("127.0.0.1"),5252))));

	System::msleep(1000);
	if(client1->getConnection()==NULL || client2->getConnection()==NULL){
		Logger::alert("Error connecting");
		return;
	}

	Event::ptr sendEvent,receiveEvent;
	int endTime=0;

	// Send some events
	client1->send(sendEvent=Event::ptr(new MessageEvent("Hello!")));
	for(receiveEvent=NULL,endTime=System::mtime()+5000;System::mtime()<endTime && receiveEvent==NULL;receiveEvent=client2->receive());
	if(receiveEvent!=NULL){
		Logger::alert("Received:"+((MessageEvent*)receiveEvent->getRootEvent())->getText());
	}

	QT_CHECK(receiveEvent!=NULL && ((MessageEvent*)sendEvent->getRootEvent())->getText().equals(((MessageEvent*)receiveEvent->getRootEvent())->getText()));

	client2->send(sendEvent=Event::ptr(new MessageEvent("Greets to the greety!")));
	for(receiveEvent=NULL,endTime=System::mtime()+5000;System::mtime()<endTime && receiveEvent==NULL;receiveEvent=client1->receive());
	if(receiveEvent!=NULL){
		Logger::alert("Received:"+((MessageEvent*)receiveEvent->getRootEvent())->getText());
	}

	QT_CHECK(receiveEvent!=NULL && ((MessageEvent*)sendEvent->getRootEvent())->getText().equals(((MessageEvent*)receiveEvent->getRootEvent())->getText()));
}

// The EventListener interface seems to imply that it listens to things internally to an Event, whereas its really a "EventServer/EventClient listener"
//  But unless EventServer & EventClient both implement an EventPump or something like that, in which case it would be an EventPumpListener, we're kind of SOL
QT_TEST(ClientServerTest){
	DebugListener::ptr debug(new DebugListener());
	SimpleEventFactory::ptr eventFactory(new SimpleEventFactory());

	// Create Server
	SimpleServer::ptr server=SimpleServer::ptr(new SimpleServer(eventFactory,Connector::ptr(new TCPConnector(6969))));

	// Create Clients
	SimpleClient::ptr client1=SimpleClient::ptr(new SimpleClient(eventFactory,Connector::ptr(new TCPConnector(Socket::stringToIP("127.0.0.1"),6969))));
	SimpleClient::ptr client2=SimpleClient::ptr(new SimpleClient(eventFactory,Connector::ptr(new TCPConnector(Socket::stringToIP("127.0.0.1"),6969))));

	Event::ptr sendEvent,receiveEvent;
	int endTime=0;

	// Send some events
	client1->send(sendEvent=Event::ptr(new MessageEvent("Hello!")));
	for(receiveEvent=NULL,endTime=System::mtime()+5000;System::mtime()<endTime && receiveEvent==NULL;receiveEvent=server->receive());
	if(receiveEvent!=NULL){
		Logger::alert("Received:"+shared_static_cast<MessageEvent>(receiveEvent)->getText());
	}
	QT_CHECK(receiveEvent!=NULL && ((MessageEvent*)sendEvent->getRootEvent())->getText().equals(((MessageEvent*)receiveEvent->getRootEvent())->getText()));

	server->broadcast(sendEvent=Event::ptr(new MessageEvent("Howdy!")));
	for(receiveEvent=NULL,endTime=System::mtime()+5000;System::mtime()<endTime && receiveEvent==NULL;receiveEvent=client1->receive());
	if(receiveEvent!=NULL){
		Logger::alert("Received:"+((MessageEvent*)receiveEvent->getRootEvent())->getText());
	}
	QT_CHECK(receiveEvent!=NULL && ((MessageEvent*)sendEvent->getRootEvent())->getText().equals(((MessageEvent*)receiveEvent->getRootEvent())->getText()));
	for(receiveEvent=NULL,endTime=System::mtime()+5000;System::mtime()<endTime && receiveEvent==NULL;receiveEvent=client2->receive());
	if(receiveEvent!=NULL){
		Logger::alert("Received:"+((MessageEvent*)receiveEvent->getRootEvent())->getText());
	}
	QT_CHECK(receiveEvent!=NULL && ((MessageEvent*)sendEvent->getRootEvent())->getText().equals(((MessageEvent*)receiveEvent->getRootEvent())->getText()));

	client1->sendToClient(1,sendEvent=Event::ptr(new MessageEvent("Sup!")));
	for(receiveEvent=NULL,endTime=System::mtime()+5000;System::mtime()<endTime && receiveEvent==NULL;receiveEvent=client2->receive());
	if(receiveEvent!=NULL){
		Logger::alert("Received:"+((MessageEvent*)receiveEvent->getRootEvent())->getText());
	}
	QT_CHECK(receiveEvent!=NULL && ((MessageEvent*)sendEvent->getRootEvent())->getText().equals(((MessageEvent*)receiveEvent->getRootEvent())->getText()));

//	int ping=client->pingAndWait(); // TODO: Figure out how pinging will work, if there are unprocessed Events hanging around like the Howdy above?
									//  We could include a PingID, or a more general EventID, and then the client itself would be able to look at
									//  incoming events and tell 'Hey this is a Pong Event, and it has the ID I'm waiting for
}

#endif