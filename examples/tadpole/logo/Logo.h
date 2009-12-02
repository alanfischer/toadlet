#ifndef LOGO_H
#define LOGO_H

#include <toadlet/pad/Application.h>
#include <toadlet/tadpole/node/CameraNode.h>
#include <toadlet/tadpole/node/MeshNode.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::node;
using namespace toadlet::pad;

class Logo:public Application{
public:
	Logo(const String &respath);
	virtual ~Logo();

	void create();
	void resized(int width,int height);
	void render(Renderer *renderer);
	void update(int dt);

	String resourcePath;
	CameraNode::ptr cameraNode;
	MeshNode::ptr meshNode;
};

#endif
