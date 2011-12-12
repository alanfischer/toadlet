#ifndef LOGO_H
#define LOGO_H

#include <toadlet/toadlet.h>

class Logo:public Applet{
public:
	Logo(Application *app);
	virtual ~Logo();

	void create();
	void destroy();
	void render(RenderDevice *renderDevice);
	void update(int dt);

	void resized(int width,int height);
	void focusGained(){}
	void focusLost(){}

	void keyPressed(int key){}
	void keyReleased(int key){}

	void mousePressed(int x,int y,int button){}
	void mouseMoved(int x,int y){}
	void mouseReleased(int x,int y,int button){}
	void mouseScrolled(int x,int y,int scroll){}
	
	Material::ptr makeFunkyMaterial();

	Application *app;
	Engine *engine;
	Scene::ptr scene;
	CameraNode::ptr cameraNode;
	MeshNode::ptr meshNode;
};

#endif
