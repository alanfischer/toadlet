#ifndef LOGO_H
#define LOGO_H

#include <toadlet/egg.h>
#include <toadlet/flick.h>
#include <toadlet/peeper.h>
#include <toadlet/tadpole.h>
#include <toadlet/pad.h>

class  Logo:public Application{
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
