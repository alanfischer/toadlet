#ifndef VIEWER_H
#define VIEWER_H

#include <toadlet/pad/Application.h>
#include <toadlet/tadpole/entity/MeshEntity.h>

class Viewer:public toadlet::pad::Application{
public:
	Viewer();

	void start(toadlet::tadpole::entity::MeshEntity::ptr meshEntity);

	void update(int dt);
	void render(toadlet::peeper::Renderer *renderer);

	void mouseMoved(int x,int y);
	void mousePressed(int x,int y,int button);
	void mouseReleased(int x,int y,int button);
	void resized(int width,int height);
	void focusLost();

protected:
	void updateCamera();

	toadlet::tadpole::entity::ParentEntity::ptr mParent;
	toadlet::tadpole::entity::CameraEntity::ptr mCamera;
	toadlet::tadpole::entity::LightEntity::ptr mLight;
 	int mMouseX,mMouseY;
	bool mDrag;
	toadlet::scalar mDistance;
	bool mZoom;
};

#endif
