#include "Logo.h"
#include <toadlet/tadpole/plugins/ShadowMappedRenderManager.h>

Logo::Logo(Application *app){
	this->app=app;
}

Logo::~Logo(){
}

void Logo::create(){
	engine=app->getEngine();
	engine->getArchiveManager()->addDirectory("../../data");

	scene=new Scene(engine);

	ShadowMappedRenderManager::ptr renderManager=new ShadowMappedRenderManager(scene);
	scene->setRenderManager(renderManager);

	Node::ptr node=new Node(scene);
	{
		LightComponent::ptr light=new LightComponent();
		LightState state;
		state.type=LightState::Type_DIRECTION;
		state.direction=Math::NEG_Z_UNIT_VECTOR3;
		light->setLightState(state);
		node->attach(light);

		renderManager->setLight(light);
	}
	scene->getRoot()->attach(node);

	Node::ptr lt=new Node(scene);
	{
 		MeshComponent::ptr mesh=new MeshComponent(engine);
		mesh->setMesh("lt.tmsh");
		lt->attach(mesh);

		if(mesh!=NULL){
			AnimationAction::ptr animation=new AnimationAction(mesh->getSkeleton()->getAnimation(0));
			animation->setCycling(AnimationAction::Cycling_REFLECT);
			lt->attach(new ActionComponent("animation",animation));
			mesh->getSkeleton()->setRenderSkeleton(true);
		}
		lt->startAction("animation");
	}
	scene->getRoot()->attach(lt);

	node=new Node(scene);
	{
		camera=new CameraComponent(new Camera(engine));
		camera->setClearColor(Colors::BLUE);
		node->attach(camera);
		camera->setLookAt(Vector3(0,-Math::fromInt(150),0),Math::ZERO_VECTOR3,Math::Z_UNIT_VECTOR3);
	}
	scene->getRoot()->attach(node);
}

void Logo::destroy(){
	scene->destroy();
}

void Logo::render(){
	RenderDevice *renderDevice=engine->getRenderDevice();

	renderDevice->beginScene();
		camera->render(renderDevice,scene);
	renderDevice->endScene();
	renderDevice->swap();
}

void Logo::update(int dt){
	scene->update(dt);
}

Applet *createApplet(Application *app){return new Logo(app);}
