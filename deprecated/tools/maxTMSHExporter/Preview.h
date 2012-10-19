#ifndef PREVIEW_H
#define PREVIEW_H

#include <toadlet/egg/Thread.h>
#include <toadlet/egg/math/Math.h>
#include <toadlet/peeper/Renderer.h>
#include <toadlet/peeper/RenderWindow.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/mesh/MeshData.h>
#include <toadlet/tadpole/sg/SceneManager.h>
#include <toadlet/tadpole/sg/MeshEntity.h>
#include <toadlet/tadpole/sg/CameraEntity.h>

class Preview:public toadlet::egg::Runnable,public toadlet::peeper::RenderWindowListener{
public:
	Preview(toadlet::tadpole::Engine *engine, toadlet::tadpole::mesh::MeshData::Ptr data);
	virtual ~Preview();

	void setLights(const toadlet::egg::Collection<toadlet::peeper::Light> &lights);
	void start();
	void run();
	void update();
	void render();

	void keyDown(int x){}
	void keyUp(int x){}
	void mouseMove(int x,int y);
	void mouseDown(Mouse b);
	void mouseUp(Mouse b);
	void mouseScroll(float f){}
	void resize(int x,int y){}
	void focusGained(){}
	void focusLost();
	void close(){}
	void mouseEnter(){}
	void mouseLeave(){}

protected:
	toadlet::peeper::RenderWindow *mRenderWindow;
	toadlet::peeper::Renderer *mRenderer;
	toadlet::tadpole::Engine *mEngine;
	toadlet::tadpole::sg::MeshEntity::Ptr mMesh;
	toadlet::tadpole::sg::NodeEntity::Ptr mScene;
	toadlet::tadpole::sg::CameraEntity::Ptr mCamera;
	unsigned int mLastTime;
	toadlet::egg::Thread *mThread;
 	unsigned int mMouseX,mMouseY;
	bool mDrag;
	float mDistance;
	bool mZoom;
};

#endif
