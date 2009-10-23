#include "Performance.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/System.h>
#include <toadlet/tadpole/MeshManager.h>
#include <toadlet/tadpole/entity/MeshEntity.h>

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::entity;
using namespace toadlet::pad;

#if defined(TOADLET_PLATFORM_WINCE)
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow){
#else
int main(int argc,char **argv){
#endif
	Performance app;
	app.create();
	app.setFullscreen(false);
	app.start(true);
	app.destroy();
	return 0;
}

Performance::Performance():
	mTest(0)
{}

void Performance::create(){
	Application::create();

	mScene=(Scene*)(new Scene())->create(mEngine);
	mEngine->setScene(mScene);

	mCamera=(CameraEntity*)(new CameraEntity())->create(mEngine);
	mCamera->setLookAt(Vector3(0,-Math::fromInt(5),0),Math::ZERO_VECTOR3,Math::Z_UNIT_VECTOR3);
	mScene->attach(mCamera);

	setupTest(mTest++,10);
}

Performance::~Performance(){
}

void Performance::update(int dt){
	mNode->setRotate(0,0,Math::ONE,Math::fromMilli(mScene->getVisualTime())*4);

	mScene->update(dt);
}

void Performance::render(Renderer *renderer){
	mEngine->contextUpdate(renderer);

	renderer->beginScene();

	mScene->render(renderer,mCamera);

	renderer->endScene();

	renderer->swap();

	setTitle(String("FPS:")+(int)(mCamera->getFramesPerSecond()/Math::ONE));
}

void Performance::mousePressed(int x,int y,int button){
	if(setupTest(mTest++,0)==false){
		stop();
	}
}

void Performance::resized(int width,int height){
	if(width!=0 && height!=0){
		if(width>=height){
			mCamera->setProjectionFovY(Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(width),Math::fromInt(height)),Math::fromMilli(100),Math::fromInt(100));
		}
		else{
			mCamera->setProjectionFovX(Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(height),Math::fromInt(width)),Math::fromMilli(100),Math::fromInt(100));
		}
	}
}

bool Performance::setupTest(int test,int intensity){
	if(mNode!=NULL){
		mNode->destroy();
	}
	mNode=(ParentEntity*)(new ParentEntity())->create(mEngine);
	mScene->attach(mNode);

	bool result=true;
	for(int i=0;i<intensity+1;++i){
		switch(test){
			case 0:
				mNode->attach(setupMinimumTest());
			break;
			case 1:
				mNode->attach(setupFillrateTest());
			break;
			case 2:
				mNode->attach(setupVertexrateTest());
			break;
			case 3:
				mNode->attach(setupDynamicTest());
			break;
			default:
				result=false;
			break;
		}
	}

	return result;
}

Entity::ptr Performance::setupMinimumTest(){
	MeshEntity::ptr cubeEntity=(MeshEntity*)(new MeshEntity())->create(mEngine);
	cubeEntity->load(mEngine->getMeshManager()->makeBox(AABox(-Math::HALF,-Math::HALF,-Math::HALF,Math::HALF,Math::HALF,Math::HALF)));
	return cubeEntity;
}

Entity::ptr Performance::setupFillrateTest(){
	int i;
	Mesh::ptr cubeMesh=mEngine->getMeshManager()->makeBox(AABox(-Math::HALF,-Math::HALF,-Math::HALF,Math::HALF,Math::HALF,Math::HALF));
	ParentEntity::ptr node=(ParentEntity*)(new ParentEntity())->create(mEngine);

	for(i=0;i<10;++i){
		MeshEntity::ptr cubeEntity=(MeshEntity*)(new MeshEntity())->create(mEngine);
		cubeEntity->load(cubeMesh);
		cubeEntity->setScale(Math::fromInt(5),Math::fromInt(5),Math::fromInt(5));
		node->attach(cubeEntity);
	}

	for(i=0;i<node->getNumChildren();++i){
		node->getChild(i)->setRotate(0,0,Math::ONE,Math::div(Math::fromInt(i)*Math::TWO_PI,Math::fromInt(node->getNumChildren())));
	}

	return node;
}

Entity::ptr Performance::setupVertexrateTest(){
	MeshEntity::ptr sphereEntity=(MeshEntity*)(new MeshEntity())->create(mEngine);
	sphereEntity->load(mEngine->getMeshManager()->createSphere(Sphere(Math::HALF)));
	return sphereEntity;
}

Entity::ptr Performance::setupDynamicTest(){
	Entity::ptr entity=(Entity*)(new Entity())->create(mEngine);
	return entity;
}
