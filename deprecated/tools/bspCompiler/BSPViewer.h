#ifndef BSPVIEWER_H
#define BSPVIEWER_H

#define TOADLET_USE_GLRENDERER
#include "../../examples/BaseExample.h"
#include "BSPObject.h"

#include <toadlet/hop/Manager.h>
#include <toadlet/tadpole/sg/MeshEntity.h>
#include <toadlet/tadpole/hop/HopSceneManager.h>
#include <toadlet/tadpole/hop/HopEntity.h>
#include "../../experimental/bsp/InputData.h"
#include "../../experimental/bsp/OutputData.h"
#include "../../experimental/bsp/Tracer.h"

class BSPViewer:public BaseExample,public toadlet::hop::Manager{
public:
	BSPViewer();

	void create();

	SceneManager::Ptr makeSceneManager();

	void setCompilerData(const bsp::InputData &input,const bsp::Node *nodes,const bsp::OutputData &output);
	void setEpsilon(float epsilon);

	void run();

	void traceLine(const toadlet::egg::math::Segment &segment,toadlet::hop::Collision &result);
	void traceSolid(const toadlet::egg::math::Segment &segment,const toadlet::hop::Solid *solid,toadlet::hop::Collision &result);
	void preUpdate(toadlet::hop::Solid *solid,float dt){}
	void intraUpdate(toadlet::hop::Solid *solid,float dt){}
	void postUpdate(toadlet::hop::Solid *solid,float dt){}

	void mouseDown(Mouse button);
	void keyDown(int key);
	void keyUp(int key);
	void updateWorld(float dt);
	void mouseMove(int x,int y);
	void updateCamera();

protected:
	float mEpsilon;
	BSPObject::Ptr mBSPCity;
	toadlet::tadpole::hop::HopSceneManager::Ptr mScene;
	toadlet::tadpole::hop::HopEntity::Ptr mObserver;
	toadlet::egg::math::Vector3 mMove;
	toadlet::tadpole::hop::HopEntity::Ptr mWorld;
	bsp::Tracer mTracer;
};

#endif
