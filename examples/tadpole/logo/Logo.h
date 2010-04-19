#ifndef LOGO_H
#define LOGO_H

#include <toadlet/egg.h>
#include <toadlet/flick.h>
#include <toadlet/tadpole.h>
#include <toadlet/pad.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::flick;
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

	Scene::ptr scene;
	CameraNode::ptr cameraNode;
	MeshNode::ptr meshNode;
};

#endif
