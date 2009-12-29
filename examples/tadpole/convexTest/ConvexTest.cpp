#include "ConvexTest.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/System.h>


using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::hop;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::node;
using namespace toadlet::pad;

#if defined(TOADLET_PLATFORM_WINCE)
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow){
#else
int main(int argc,char **argv){
#endif
	ConvexTest app;
	app.setFullscreen(false);
	app.create();
	app.start(true);
	app.destroy();
	return 0;
}

ConvexTest::ConvexTest(){
}

void ConvexTest::create(){
	Application::create();

	mScene=new HopScene(mEngine->createNodeType(SceneNode::type()));
	mEngine->setScene(mScene);

	mCamera=mEngine->createNodeType(CameraNode::type());
	mCamera->setLookAt(Vector3(-Math::fromInt(25),0,0),Math::ZERO_VECTOR3,Math::Z_UNIT_VECTOR3);
	mScene->getRootNode()->attach(mCamera);

	setupTest();
}

ConvexTest::~ConvexTest(){
}

void ConvexTest::update(int dt){
	mNode->setRotate(0,0,Math::ONE,Math::fromMilli(mScene->getRenderTime())*4);

	mScene->update(dt);
}

void ConvexTest::render(Renderer *renderer){
	renderer->beginScene();
		mScene->render(renderer,mCamera,NULL);
	renderer->endScene();
	renderer->swap();
}

void ConvexTest::mousePressed(int x,int y,int button){
	stop();
}

void ConvexTest::resized(int width,int height){
	if(mCamera!=NULL && width!=0 && height!=0){
		if(width>=height){
			mCamera->setProjectionFovY(Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(width),Math::fromInt(height)),Math::fromMilli(100),Math::fromInt(100));
		}
		else{
			mCamera->setProjectionFovX(Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(height),Math::fromInt(width)),Math::fromMilli(100),Math::fromInt(100));
		}
	}
}

void ConvexTest::setupTest(){
	if(mNode!=NULL){
		mNode->destroy();
	}
	mNode=mEngine->createNodeType(ParentNode::type());
	mScene->getRootNode()->attach(mNode);
	mScene->showCollisionVolumes(true,true);

	Shape::ptr sphere(new Shape(Sphere(Math::HALF)));
	HopEntity::ptr sphereEntity=mEngine->createNodeType(HopEntity::type());
	sphereEntity->addShape(sphere);
	sphereEntity->setCoefficientOfRestitution(0.9);
	sphereEntity->setTranslate(Vector3(0.0,0.0,2.0));
	mScene->getRootNode()->attach(sphereEntity);

	ConvexSolid cs;
	cs.planes.add(Plane(Vector3(1.0,0.0,0.0),0.5));
	cs.planes.add(Plane(Vector3(-1.0,0.0,0.0),0.5));
	cs.planes.add(Plane(Vector3(0.0,1.0,0.0),0.5));
	cs.planes.add(Plane(Vector3(0.0,-1.0,0.0),0.5));
	cs.planes.add(Plane(Vector3(1.0,1.0,1.0),0.5));
	cs.planes.add(Plane(Vector3(0.0,0.0,-1.0),0.5));
	Shape::ptr cshape(new Shape(cs));
	HopEntity::ptr convexEntity=mEngine->createNodeType(HopEntity::type());
	convexEntity->addShape(cshape);
	convexEntity->setLocalGravity(Math::ZERO_VECTOR3);
	convexEntity->setCoefficientOfRestitution(0.9);
	convexEntity->setInfiniteMass();
	mScene->getRootNode()->attach(convexEntity);

	mScene->setGravity(Vector3(0.0,0.0,-Math::ONE));
}
