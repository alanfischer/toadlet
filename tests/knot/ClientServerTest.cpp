#include <toadlet/egg/System.h>
#include <toadlet/knot/TCPConnector.h>
#include <toadlet/knot/DebugListener.h>
#include <toadlet/knot/SimpleEventClient.h>
#include <toadlet/knot/SimpleEventServer.h>
#include "../quicktest.h"

using namespace toadlet::egg;
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

// The EventListener interface seems to imply that it listens to things internally to an Event, whereas its really a "EventServer/EventClient listener"
//  But unless EventServer & EventClient both implement an EventPump or something like that, in which case it would be an EventPumpListener, we're kind of SOL
QT_TEST(ClientServerTest){
	DebugListener::ptr debug(new DebugListener());
	SimpleEventFactory::ptr eventFactory(new SimpleEventFactory());

	EventServer::ptr server;
	EventClient::ptr client;
	// Create Server
	if(true){
		server=EventServer::ptr(new SimpleEventServer(eventFactory));
//		server->addEventListener(debug);

		// Listen for connections
		TCPConnector::ptr serverConnector(new TCPConnector());
		server->setConnector(serverConnector);
		serverConnector->addConnectorListener(debug);
		serverConnector->accept(6969);
	}
	else{ // TODO: Compact syntax?
//		server=new SimpleEventServer(new TCPConnector(6969));
	}
	
	// Create Client
	if(true){
		client=EventClient::ptr(new SimpleEventClient(eventFactory));
//		client->addEventListener(debug);

		// Connect
		TCPConnector::ptr clientConnector(new TCPConnector());
		client->setConnector(clientConnector);
		clientConnector->addConnectorListener(debug);
		clientConnector->connect(Socket::stringToIP("127.0.0.1"),6969);
	}
	else{ // TODO: Compact syntax?
//		client=new SimpleEventClient(new TCPConnector(localhost:6969));
	}

	// Send some events
	client->sendEvent(Event::ptr(new MessageEvent("Hello!")));
	server->broadcastEvent(Event::ptr(new MessageEvent("Howdy!")));
	while(true);
//	int ping=client->pingAndWait(); // TODO: Figure out how pinging will work, if there are unprocessed Events hanging around like the Howdy above?
									//  We could include a PingID, or a more general EventID, and then the client itself would be able to look at
									//  incoming events and tell 'Hey this is a Pong Event, and it has the ID I'm waiting for
}
