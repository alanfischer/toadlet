#include "Logo.h"

Logo::Logo(Application *app){
	this->app=app;
}

Logo::~Logo(){
}

void Logo::create(){
	engine=app->getEngine();
	engine->getArchiveManager()->addDirectory("../../../data");

	scene=new Scene(engine);

	Node::ptr node=new Node(scene);
	{
		LightComponent::ptr light=new LightComponent();
		LightState state;
		state.type=LightState::Type_DIRECTION;
		state.direction=Math::NEG_Z_UNIT_VECTOR3;
		light->setLightState(state);
		node->attach(light);
	}
	scene->getRoot()->attach(node);

	Node::ptr lt=new Node(scene);
	{
 		MeshComponent::ptr mesh=new MeshComponent(engine);
		mesh->setMesh("lt.xmsh");
		lt->attach(mesh);

		if(mesh!=NULL){
			ActionComponent::ptr action=new ActionComponent("animation");
			AnimationAction::ptr animation=new AnimationAction(mesh->getSkeleton()->getAnimation(0));
			animation->setCycling(AnimationAction::Cycling_REFLECT);
			action->attach(animation);
			lt->attach(action);
			mesh->getSkeleton()->setRenderSkeleton(true);
		}
		lt->startAction("animation");
	}
	scene->getRoot()->attach(lt);

	node=new Node(scene);
	{
		camera=new CameraComponent(new Camera());
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
		camera->getCamera()->render(renderDevice,scene);
	renderDevice->endScene();
	renderDevice->swap();
}

void Logo::update(int dt){
	scene->update(dt);
}

Applet *createApplet(Application *app){return new Logo(app);}
