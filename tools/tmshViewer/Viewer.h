#ifndef VIEWER_H
#define VIEWER_H

#include <toadlet/tadpole.h>
#include <toadlet/pad.h>

class Viewer:public Applet{
public:
	Viewer(Application *app);
	virtual ~Viewer();

	void create(){}
	void destroy();
	void setNode(MeshNode::ptr meshNode);

	void update(int dt);
	void render();

	void mouseMoved(int x,int y);
	void mousePressed(int x,int y,int button);
	void mouseReleased(int x,int y,int button);
	void mouseScrolled(int x,int y,int scroll){}

	void resized(int width,int height);
	void focusGained(){}
	void focusLost();

	void keyPressed(int key){}
	void keyReleased(int key){}

	inline Scene::ptr getScene(){return mScene;}

protected:
	void updateCamera();

	Application *mApp;
	Engine *mEngine;
	Scene::ptr mScene;
	Node::ptr mParent;
	CameraNode::ptr mCamera;
	LightNode::ptr mLight;
 	int mMouseX,mMouseY;
	bool mDrag;
	toadlet::scalar mDistance;
	bool mZoom;
};

#endif
