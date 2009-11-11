#include "Logo.h"

Logo::Logo(const String &resPath):Application(){
	resourcePath=resPath;
}

Logo::~Logo(){
}

void Logo::create(){
	Application::create();

	getEngine()->setScene((Scene*)((new Scene())->create(getEngine())));

	meshEntity=mEngine->createEntityType(MeshEntity::type());
	meshEntity->load(getEngine()->cacheMesh(resourcePath));
	meshEntity->getAnimationController()->start();
	getEngine()->getScene()->attach(meshEntity);

	cameraEntity=mEngine->createEntityType(CameraEntity::type());
	cameraEntity->setLookDir(Vector3(Math::fromInt(25),-Math::fromInt(100),0),Math::Y_UNIT_VECTOR3,Math::Z_UNIT_VECTOR3);
	cameraEntity->setClearColor(Colors::BLUE);
	getEngine()->getScene()->attach(cameraEntity);
}

void Logo::resized(int width,int height){
	if(cameraEntity!=NULL){
		if(width>0 && height>0){
			if(width>=height){
				cameraEntity->setProjectionFovY(Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(width),Math::fromInt(height)),Math::fromInt(10),Math::fromInt(200));
			}
			else{
				cameraEntity->setProjectionFovX(Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(height),Math::fromInt(width)),Math::fromInt(10),Math::fromInt(200));
			}
		}
		cameraEntity->setViewport(Viewport(0,0,width,height));
	}
}

void Logo::render(Renderer *renderer){
	getEngine()->contextUpdate(renderer);
	renderer->beginScene();
		getEngine()->getScene()->render(renderer,cameraEntity);
	renderer->endScene();
	renderer->swap();
}

void Logo::update(int dt){
	getEngine()->getScene()->update(dt);
}
