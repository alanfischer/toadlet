#ifndef VIEWER_H
#define VIEWER_H

#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/node/CameraNode.h>
#include <toadlet/tadpole/node/LightNode.h>
#include <toadlet/tadpole/node/MeshNode.h>
#include <toadlet/pad/Application.h>

class Viewer:public toadlet::pad::Application{
public:
	Viewer();

	void create();
	void start(toadlet::tadpole::node::MeshNode::ptr meshNode);

	void update(int dt);
	void render(toadlet::peeper::Renderer *renderer);

	void mouseMoved(int x,int y);
	void mousePressed(int x,int y,int button);
	void mouseReleased(int x,int y,int button);
	void resized(int width,int height);
	void focusLost();

	inline toadlet::tadpole::Scene::ptr getScene(){return mScene;}

protected:
	void updateCamera();

	toadlet::tadpole::Scene::ptr mScene;
	toadlet::tadpole::node::ParentNode::ptr mParent;
	toadlet::tadpole::node::CameraNode::ptr mCamera;
	toadlet::tadpole::node::LightNode::ptr mLight;
 	int mMouseX,mMouseY;
	bool mDrag;
	toadlet::scalar mDistance;
	bool mZoom;
};

#endif
