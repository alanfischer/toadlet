#ifndef LOGO_H
#define LOGO_H

#include <toadlet/egg/io/MemoryInputStream.h>
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

class Logo:public Application{
public:
	Logo();
	virtual ~Logo();

	void create();
	void resized(int width,int height);
	void render(Renderer *renderer);
	void update(int dt);

	CameraNode::ptr cameraNode;
	MeshNode::ptr meshNode;
};

#endif
