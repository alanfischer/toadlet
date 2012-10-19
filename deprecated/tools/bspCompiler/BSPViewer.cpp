#include "BSPViewer.h"

#include <toadlet/egg/io/FileInputStream.h>
#include <toadlet/egg/io/FileOutputStream.h>
#include <toadlet/tadpole/hop/HopEntity.h>
#include <toadlet/tadpole/hop/HopCollisionEvent.h>
#include <toadlet/tadpole/hop/Win32ChooseLibs.h>
#include "../../experimental/bsp/MeshDataConverter.h"
#include "../../experimental/bsp/Compiler.h"
#include "../../experimental/bsp/CollisionTreeHandler.h"

using namespace toadlet::egg::io;
using namespace toadlet::hop;
using namespace toadlet::tadpole::hop;
using namespace bsp;

BSPViewer::BSPViewer():BaseExample("BSPCompiler"){
	mEpsilon=0.0001f;
}

void BSPViewer::create(){
	BaseExample::create();

	mDistance=90;

	mWorld=HopEntity::make(mEngine);
	mWorld->getSolid()->setCoefficientOfGravity(0);
	mWorld->getSolid()->setInfiniteMass();
	mWorld->getSolid()->setCoefficientOfRestitution(0);
	mWorld->getSolid()->setCoefficientOfDynamicFriction(2);
	mScene->attach(mWorld);

	mObserver=HopEntity::make(mEngine);
	mObserver->setTranslate(Vector3(0,0,10));
	mObserver->getSolid()->setCoefficientOfGravity(0);
	mObserver->getSolid()->setCoefficientOfDynamicFriction(0);
	mObserver->getSolid()->setCoefficientOfRestitution(0);
	//mObserver->getSolid()->setInteraction(Solid::INTERACTION_NONE);
	mScene->attach(mObserver);

	mBSPCity=BSPObject::make(mEngine);
	mBSPCity->setTranslate(Vector3(0,0,0));
	mEngine->getSceneManager()->attach(mBSPCity);

	LightEntity::Ptr light=LightEntity::make(mEngine);
	light->setDiffuse(Vector4(1,1,1,0));
	light->setLinearAttenuation(0.5f);
	light->setDirection(Vector3(0,1,1));
	light->setType(Light::LT_DIRECTION);
	mEngine->getSceneManager()->attach(light);

	mEngine->getSceneManager()->getSceneStates()->ambientLight=Vector4(0.2,0.2,0.2,0);
}

SceneManager::Ptr BSPViewer::makeSceneManager(){
	mScene=HopSceneManager::make(mEngine);
	mScene->getSimulator()->setGravity(Vector3(0,0,-20));
	mScene->getSimulator()->setManager(this);
	mScene->getSimulator()->setEpsilon(mEpsilon);
	mScene->setFixedDT(1.0f/30.0f);
	return mScene;
}

void BSPViewer::run(){
	mLastTime=System::mtime();

	mRenderWindow->startEventLoop();
}

void BSPViewer::setCompilerData(const InputData &input,const Node *nodes,const OutputData &output){
	mBSPCity->setup(input.polygons,input.vertexes);
	mBSPCity->setOutputData(const_cast<OutputData*>(&output));

	mTracer.setNodes(nodes);
}

void BSPViewer::setEpsilon(float epsilon){
	mEpsilon=epsilon;
	if(mScene!=NULL){
		mScene->getSimulator()->setEpsilon(mEpsilon);
	}
}

void BSPViewer::mouseDown(Mouse button){
	if(button==MOUSE_MIDDLE){
		mBSPCity->showNextPlane();
	}

	BaseExample::mouseDown(button);
}

void BSPViewer::traceLine(const Segment &segment,Collision &result){
	Vector3 normal;
	float time;
	Vector3 endPoint;
	segment.getEndPoint(endPoint);
	time=mTracer.traceLine(segment.origin,endPoint,0,normal);
	result.time=time;
	if(time!=-1){
		result.point=makeLerp(segment.origin,endPoint,time);
		result.normal=normal;
		result.solid=mWorld->getSolid();
	}
}

void BSPViewer::traceSolid(const Segment &segment,const Solid *solid,Collision &result){
	Vector3 normal;
	float time;
	Vector3 endPoint;
	segment.getEndPoint(endPoint);
	time=mTracer.traceLine(segment.origin,endPoint,0,normal);
	result.time=time;
	if(time!=-1){
		result.point=makeLerp(segment.origin,endPoint,time);
		result.normal=normal;
		result.solid=mWorld->getSolid();
	}
}

void BSPViewer::keyDown(int key){
	if(key=='w'){
		mMove.y=1;
	}
	if(key=='s'){
		mMove.y=-1;
	}
	if(key=='a'){
		mMove.x=-1;
	}
	if(key=='d'){
		mMove.x=1;
	}
	if(key=='f'){
		mMove.z=1;
	}
	if(key=='v'){
		mMove.z=-1;
	}
	if(key=='p'){
		mBSPCity->showNextPlane();
	}
}

void BSPViewer::keyUp(int key){
	if(key=='w'){
		mMove.y=0;
	}
	if(key=='s'){
		mMove.y=0;
	}
	if(key=='a'){
		mMove.x=0;
	}
	if(key=='d'){
		mMove.x=0;
	}
	if(key=='f'){
		mMove.z=0;
	}
	if(key=='v'){
		mMove.z=0;
	}
}

void BSPViewer::updateWorld(float dt){
	Vector3 velocity=mObserver->getWorldRotate()*mMove*10;
	if(mObserver->getTouching()!=NULL){
		Plane plane(Vector3(),mObserver->getTouchingNormal());
		project(plane,velocity);
	}
	mObserver->setVelocity(velocity);

	HopCollisionEvent event;
	mWorld->getSceneManager()->traceLine(mObserver->getTranslate(),mObserver->getTranslate()+Vector3(0,0,-1000),mObserver,event);

	BaseExample::updateWorld(dt);

	updateCamera();
}

void BSPViewer::mouseMove(int x,int y){
	int diffx=mMouseX-x;
	int diffy=mMouseY-y;
	mMouseX=x;
	mMouseY=y;

	if(x==mWidth/2 && y==mHeight/2){
		return;
	}

	if(mDrag){
		float dx=((float)diffx)/100;
		float dy=((float)diffy)/100;

		mRotateX+=dx;
		mRotateY+=dy;

		if(mRotateY>HALF_PI-0.01){
			mRotateY=HALF_PI-0.01;
		}
		if(mRotateY<-HALF_PI+0.01){
			mRotateY=-HALF_PI+0.01;
		}

		mRenderWindow->setMousePosition(mWidth/2,mHeight/2);
	}
}

void BSPViewer::updateCamera(){
	if(mObserver!=NULL && mCamera!=NULL){
		Matrix3x3 offset=makeMatrix3x3FromZ(mRotateX) * makeMatrix3x3FromX(mRotateY);
		mObserver->setRotate(offset);
		Vector3 forward(0,1,0);
		forward=offset*forward;
		mCamera->setLookAt(mObserver->getWorldTranslate(),mObserver->getWorldTranslate()+forward,Vector3(0,0,1));
	}
}
