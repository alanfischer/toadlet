#ifndef SIMPLESYNC_H
#define SIMPLESYNC_H

#include <toadlet/egg/System.h>
#include <toadlet/egg/io/MemoryStream.h>
#include <toadlet/knot/TCPConnector.h>
#include <toadlet/knot/SimpleEventServer.h>
#include <toadlet/knot/SimpleEventClient.h>
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

class SimpleSync:public Application,public EventFactory,public Runnable{
public:
	TOADLET_SHARED_POINTERS(SimpleSync);

	SimpleSync();
	virtual ~SimpleSync();

	void accept(int localPort);
	void connect(int remoteHost,int remortPort);

	void create();
	void resized(int width,int height);
	void render(Renderer *renderer);
	void update(int dt);

	Event::ptr createEventType(int type);

	void run();

	EventServer::ptr eventServer;
	EventClient::ptr eventClient;

	CameraNode::ptr cameraNode;
	MeshNode::ptr meshNode;
HopEntity::ptr block;
};

#endif
