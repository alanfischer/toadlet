#ifndef EXAMPLE_REDUCERGUI_H
#define EXAMPLE_REDUCERGUI_H

#include <toadlet/egg/String.h>
#include <toadlet/egg/Thread.h>
#include <toadlet/egg/math/Math.h>
#include <toadlet/peeper/Renderer.h>
#include <toadlet/peeper/RenderWindow.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/sg/SceneManager.h>
#include <toadlet/tadpole/sg/MeshEntity.h>
#include <toadlet/tadpole/sg/CameraEntity.h>
#include <toadlet/goo/ButtonControl.h>
#include <toadlet/goo/Container.h>
#include <toadlet/goo/GUIRenderer.h>
#include <toadlet/goo/SliderControl.h>
#include "reducer/Reducer.h"

class ReducerGUI:public toadlet::egg::Runnable,public toadlet::peeper::RenderWindowListener{
public:
	ReducerGUI(toadlet::tadpole::Engine *engine,toadlet::tadpole::sg::MeshEntity::Ptr mesh,const toadlet::egg::String &mshFileName);
	virtual ~ReducerGUI();

	void start();
	void run();
	void update();
	void render();
	bool running();

	void keyDown(int x){}
	void keyUp(int x){}
	void mouseMove(int x,int y);
	void mouseDown(Mouse b);
	void mouseUp(Mouse b);
	void mouseScroll(float f){}
	void mouseEnter(){}
	void mouseLeave(){}
	void resize(int x,int y){mWidth=x;mHeight=y;}
	void focusGained(){}
	void focusLost();
	void close(){}
	void sliderHandler();
	void buttonHandler();

protected:
	toadlet::egg::String mMshFileName;
	toadlet::egg::Thread *mThread;
	toadlet::peeper::RenderWindow *mRenderWindow;
	toadlet::peeper::Renderer *mRenderer;
	toadlet::tadpole::Engine *mEngine;
	toadlet::tadpole::mesh::MeshData::Ptr mMeshData;
	toadlet::tadpole::sg::NodeEntity::Ptr mScene;
	toadlet::tadpole::sg::CameraEntity::Ptr mCamera;
	toadlet::goo::GUIRenderer *mGUIRenderer;
	toadlet::goo::Container *mGUI;
	toadlet::goo::SliderControl *mSlider;
	toadlet::goo::ButtonControl *mButton;
	reducer::Reducer *mReducer;
	
	float mNumTriangles;
	long mLastTime;
 	int mMouseX,mMouseY;
	int mWidth,mHeight;
	bool mDrag;
	float mDistance;
	bool mZoom;
};

#endif
