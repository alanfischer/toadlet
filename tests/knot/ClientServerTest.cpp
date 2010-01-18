#include <toadlet/egg/System.h>
#include <toadlet/knot/TCPConnector.h>
#include <toadlet/knot/DebugListener.h>
#include "../quicktest.h"

using namespace toadlet::egg;
using namespace toadlet::egg::net;
using namespace toadlet::knot;

// The EventListener interface seems to imply that it listens to things internally to an Event, whereas its really a "EventServer/EventClient listener"
//  But unless EventServer & EventClient both implement an EventPump or something like that, in which case it would be an EventPumpListener, we're kind of SOL
QT_TEST(ClientServerTest){
	DebugListener::ptr debug(new DebugListener());

//	EventServer::ptr server;
//	EventClient::ptr client;
	// Create Server
	if(true){
//		server=new SimpleEventServer();
//		server->addEventListener(debug);

		// Listen for connections
		TCPConnector::ptr serverConnector(new TCPConnector());
		serverConnector->addConnectorListener(debug);
//		serverConnector->addConnectorListener(server);
		serverConnector->accept(6969);
	}
	else{ // TODO: Compact syntax?
//		server=new SimpleEventServer(new TCPConnector(6969));
	}
	
	// Create Client
	if(true){
//		client=new SimpleEventClient();
//		client->addEventListener(debug);

		// Connect
		TCPConnector::ptr clientConnector(new TCPConnector());
		clientConnector->addConnectorListener(debug);
//		clientConnector->addConnectorListener(client);
		clientConnector->connect(Socket::stringToIP("127.0.0.1"),6969);
	}
	else{ // TODO: Compact syntax?
//		client=new SimpleEventClient(new TCPConnector(localhost:6969));
	}

	// Send some events
//	client->sendEvent(Event::ptr(new MessageEvent("Hello!")));
//	server->sendEvent(Event::ptr(new MessageEvent("Howdy!")));
//	int ping=client->pingAndWait(); // TODO: Figure out how pinging will work, if there are unprocessed Events hanging around like the Howdy above?
									//  We could include a PingID, or a more general EventID, and then the client itself would be able to look at
									//  incoming events and tell 'Hey this is a Pong Event, and it has the ID I'm waiting for
}
