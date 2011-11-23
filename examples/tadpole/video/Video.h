#ifndef VIDEO_H
#define VIDEO_H

#include <toadlet/toadlet.h>

class Video:public Applet{
public:
	Video(Application *app);
	virtual ~Video();

	void create();
	void destroy();
	void resized(int width,int height);
	void render(RenderDevice *device);
	void update(int dt);
	
	void focusGained(){}
	void focusLost(){}

	void keyPressed(int key);
	void keyReleased(int){}

	void mousePressed(int x,int y,int button){}
	void mouseMoved(int x,int y){}
	void mouseReleased(int x,int y,int button){}
	void mouseScrolled(int x,int y,int scroll){}

	void joyPressed(int button){}
	void joyMoved(scalar x,scalar y,scalar z,scalar r,scalar u,scalar v){}
	void joyReleased(int button){}

	Application *app;
	Engine *engine;
	Scene::ptr scene;
	CameraNode::ptr cameraNode;
	Texture::ptr texture;
	VideoController::ptr controller;
	MeshNode::ptr meshNode;
	AudioNode::ptr audioNode;
};

#endif
