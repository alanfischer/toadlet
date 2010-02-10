#ifndef SIMPLESYNC_H
#define SIMPLESYNC_H

#include <toadlet/egg/System.h>
#include <toadlet/egg/io/MemoryStream.h>
#include <toadlet/knot/TCPConnector.h>
#include <toadlet/knot/SimpleServer.h>
#include <toadlet/knot/SimpleClient.h>
#include <toadlet/tadpole/node/CameraNode.h>
#include <toadlet/tadpole/node/MeshNode.h>
#include <toadlet/tadpole/node/SceneNode.h>
#include <toadlet/tadpole/plugins/hop/HopEntity.h>
#include <toadlet/pad/Application.h>
/*
An Event class for the Client message structure, which will control player movement.

Which means we can have a class of events for the ClientUpdateEvent sort of thing,
	which are time marked or maybe DT marked and can be retrieved from the client & servers?

Then we also have a generic class of just UpdateEvents which are different, and time marked, and are unique, as in only one will be dealt with each frame.  If we get repeats, then the old ones are thrown out.

We also need to add in the Sync/Ping something event to let us figure out the time differenceing for the lag/buffer code.
*/


using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::net;
using namespace toadlet::hop;
using namespace toadlet::knot;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::node;
using namespace toadlet::tadpole::mesh;
using namespace toadlet::pad;

class ClientEvent;

class SimpleSync:public Application,public EventFactory,public ConnectionListener,public Runnable,public UpdateListener{
public:
	TOADLET_SHARED_POINTERS(SimpleSync);

	SimpleSync();
	virtual ~SimpleSync();

	void accept(int localPort);
	void connect(int remoteHost,int remortPort);

	void create();
	void destroy();
	void resized(int width,int height);
	void render(Renderer *renderer);
	void update(int dt);
	void preLogicUpdate(int dt);
	void logicUpdate(int dt);
	void postLogicUpdate(int dt);
	void preRenderUpdate(int dt){scene->preRenderUpdate(dt);}
	void renderUpdate(int dt){scene->renderUpdate(dt);}
	void postRenderUpdate(int dt){scene->postRenderUpdate(dt);}
	void keyPressed(int key);
	void keyReleased(int key);
	void mousePressed(int x,int y,int button);
	void mouseMoved(int x,int y);
	void mouseReleased(int x,int y,int button);

	Event::ptr createEventType(int type);

	void connected(Connection::ptr connection);
	void disconnected(Connection::ptr connection);

	void run();

	SimpleServer::ptr server;
	SimpleClient::ptr client;

	HopScene::ptr scene;
	int nextUpdateTime;
	int clientServerTimeDifference;
	int clientLeadTime;
	int debugUpdateMin,debugUpdateMax;
	int packetDelay;
	int packetDelayVariance;
	Mutex mutex;

	Random random;
	CameraNode::ptr cameraNode;
	MeshNode::ptr meshNode;
	HopEntity::ptr block;
	HopEntity::ptr player[2];

	int playerCollision;
	int lastClientUpdateCounter[2];
	int flags;
	EulerAngle look;
	Collection<SharedPointer<ClientEvent> > sentClientEvents;
};

#endif
