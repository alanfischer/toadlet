#ifndef VIEWER_H
#define VIEWER_H

#include <toadlet/toadlet.h>

class Viewer:public Object,public Applet{
public:
	TOADLET_IOBJECT(Viewer);

	Viewer(Application *app);
	virtual ~Viewer();

	void create(){}
	void destroy();
	void setNode(Node *node);

	void update(int dt);
	void render();

	void mouseMoved(int x,int y);
	void mousePressed(int x,int y,int button);
	void mouseReleased(int x,int y,int button);
	void mouseScrolled(int x,int y,int scroll){}

	void resized(int width,int height){}
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
	Camera::ptr mCamera;
	LightComponent::ptr mLight;
 	int mMouseX,mMouseY;
	bool mDrag;
	toadlet::scalar mDistance;
	bool mZoom;
};

#endif
