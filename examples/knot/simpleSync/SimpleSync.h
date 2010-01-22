#ifndef SIMPLESYNC_H
#define SIMPLESYNC_H

#include <toadlet/egg/io/MemoryStream.h>
#include <toadlet/tadpole/node/CameraNode.h>
#include <toadlet/tadpole/node/MeshNode.h>
#include <toadlet/tadpole/node/SceneNode.h>
#include <toadlet/pad/Application.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::node;
using namespace toadlet::tadpole::mesh;
using namespace toadlet::pad;

class SimpleSync:public Application{
public:
	SimpleSync();
	virtual ~SimpleSync();

	void create();
	void resized(int width,int height);
	void render(Renderer *renderer);
	void update(int dt);

	CameraNode::ptr cameraNode;
	MeshNode::ptr meshNode;
};

#endif
