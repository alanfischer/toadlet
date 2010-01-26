#if 1

#include <toadlet/egg/Logger.h>
#include <toadlet/egg/System.h>
#include <toadlet/knot/TCPConnector.h>
#include <toadlet/knot/DebugListener.h>
#include <toadlet/knot/SimpleClientEventConnection.h>
//#include <toadlet/knot/SimpleServerEventConnection.h>
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

	SimpleClientEventConnection::ptr client1(new SimpleClientEventConnection(eventFactory,Connector::ptr(new TCPConnector(5252))));
	SimpleClientEventConnection::ptr client2(new SimpleClientEventConnection(eventFactory,Connector::ptr(new TCPConnector(Socket::stringToIP("127.0.0.1"),5252))));

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
		Logger::alert("Received:"+shared_static_cast<MessageEvent>(receiveEvent)->getText());
	}

	QT_CHECK(receiveEvent!=NULL && shared_static_cast<MessageEvent>(sendEvent)->getText().equals(shared_static_cast<MessageEvent>(receiveEvent)->getText()));

	client2->send(sendEvent=Event::ptr(new MessageEvent("Greets to the greety!")));
	for(receiveEvent=NULL,endTime=System::mtime()+5000;System::mtime()<endTime && receiveEvent==NULL;receiveEvent=client1->receive());
	if(receiveEvent!=NULL){
		Logger::alert("Received:"+shared_static_cast<MessageEvent>(receiveEvent)->getText());
	}

	QT_CHECK(receiveEvent!=NULL && shared_static_cast<MessageEvent>(sendEvent)->getText().equals(shared_static_cast<MessageEvent>(receiveEvent)->getText()));
}
/*
// The EventListener interface seems to imply that it listens to things internally to an Event, whereas its really a "EventServer/EventClient listener"
//  But unless EventServer & EventClient both implement an EventPump or something like that, in which case it would be an EventPumpListener, we're kind of SOL
QT_TEST(ClientServerTest){
	DebugListener::ptr debug(new DebugListener());
	SimpleEventFactory::ptr eventFactory(new SimpleEventFactory());

	// Create Server
	EventServer::ptr server=EventServer::ptr(new SimpleEventServer(eventFactory,Connector::ptr(new TCPConnector(6969))));

	// Create Clients
	EventClient::ptr client1=EventClient::ptr(new SimpleEventClient(eventFactory,Connector::ptr(new TCPConnector(Socket::stringToIP("127.0.0.1"),6969))));
	EventClient::ptr client2=EventClient::ptr(new SimpleEventClient(eventFactory,Connector::ptr(new TCPConnector(Socket::stringToIP("127.0.0.1"),6969))));

	Event::ptr sendEvent,receiveEvent;
	int fromClient=-1;
	int endTime=0;

	// Send some events
	client1->sendEvent(sendEvent=Event::ptr(new MessageEvent("Hello!")));
	for(receiveEvent=NULL,endTime=System::mtime()+5000;System::mtime()<endTime && receiveEvent==NULL;server->receiveEvent(receiveEvent,fromClient));
	if(receiveEvent!=NULL){
		Logger::alert("Received:"+shared_static_cast<MessageEvent>(receiveEvent)->getText());
	}

	QT_CHECK(receiveEvent!=NULL && fromClient==0 && shared_static_cast<MessageEvent>(sendEvent)->getText().equals(shared_static_cast<MessageEvent>(receiveEvent)->getText()));

	server->broadcastEvent(sendEvent=Event::ptr(new MessageEvent("Howdy!")));

	for(receiveEvent=NULL,endTime=System::mtime()+5000;System::mtime()<endTime && receiveEvent==NULL;client1->receiveEvent(receiveEvent,fromClient));
	if(receiveEvent!=NULL){
		Logger::alert("Received:"+shared_static_cast<MessageEvent>(receiveEvent)->getText());
	}

	QT_CHECK(receiveEvent!=NULL && shared_static_cast<MessageEvent>(sendEvent)->getText().equals(shared_static_cast<MessageEvent>(receiveEvent)->getText()));

	for(receiveEvent=NULL,endTime=System::mtime()+5000;System::mtime()<endTime && receiveEvent==NULL;client2->receiveEvent(receiveEvent,fromClient));
	if(receiveEvent!=NULL){
		Logger::alert("Received:"+shared_static_cast<MessageEvent>(receiveEvent)->getText());
	}

	QT_CHECK(receiveEvent!=NULL && shared_static_cast<MessageEvent>(sendEvent)->getText().equals(shared_static_cast<MessageEvent>(receiveEvent)->getText()));

	client1->sendEventToClient(1,sendEvent=Event::ptr(new MessageEvent("Sup!")));
	for(receiveEvent=NULL,endTime=System::mtime()+5000;System::mtime()<endTime && receiveEvent==NULL;client2->receiveEvent(receiveEvent,fromClient));
	if(receiveEvent!=NULL){
		Logger::alert("Received:"+shared_static_cast<MessageEvent>(receiveEvent)->getText());
	}

	QT_CHECK(receiveEvent!=NULL && shared_static_cast<MessageEvent>(sendEvent)->getText().equals(shared_static_cast<MessageEvent>(receiveEvent)->getText()));

//	int ping=client->pingAndWait(); // TODO: Figure out how pinging will work, if there are unprocessed Events hanging around like the Howdy above?
									//  We could include a PingID, or a more general EventID, and then the client itself would be able to look at
									//  incoming events and tell 'Hey this is a Pong Event, and it has the ID I'm waiting for
}
*/
#endif