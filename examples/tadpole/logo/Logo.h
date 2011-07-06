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
	void render(RenderDevice *renderDevice);
	void update(int dt);

	Material::ptr makeFunkyMaterial();

	Scene::ptr scene;
	CameraNode::ptr cameraNode;
	MeshNode::ptr meshNode;
};

#endif
