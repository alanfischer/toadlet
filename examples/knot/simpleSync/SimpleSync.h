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
	int debugUpdateMin,debugUpdateMax;
	Mutex mutex;

	Random random;
	CameraNode::ptr cameraNode;
	MeshNode::ptr meshNode;
	HopEntity::ptr block;
	HopEntity::ptr player[2];
	int movement;
};

#endif
