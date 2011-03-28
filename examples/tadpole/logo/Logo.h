#ifndef LOGO_H
#define LOGO_H

#include <toadlet/toadlet.h>

class Logo:public Application{
public:
	Logo();
	virtual ~Logo();

	void create();
	void destroy();
	void resized(int width,int height);
	void render(Renderer *renderer);
	void update(int dt);

	Scene::ptr scene;
	CameraNode::ptr cameraNode;
	MeshNode::ptr meshNode;
};

#endif
