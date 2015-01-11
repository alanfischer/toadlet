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

//	ShadowMappedRenderManager::ptr renderManager=new ShadowMappedRenderManager(scene);
//	scene->setRenderManager(renderManager);

	Node::ptr node=new Node(scene);
	{
		LightComponent::ptr light=new LightComponent();
		LightState state;
		state.type=LightState::Type_DIRECTION;
		state.direction=Math::NEG_Z_UNIT_VECTOR3;
		light->setLightState(state);
		node->attach(light);

		Matrix4x4 lookMatrix;
		Math::setMatrix4x4FromLookAt(lookMatrix,Vector3(0,0,150),Math::ZERO_VECTOR3,Math::Y_UNIT_VECTOR3,true);
		node->setMatrix4x4(lookMatrix);

//		renderManager->setLight(light);
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
		}
		lt->startAction("animation");
	}
	scene->getRoot()->attach(lt);

	Node::ptr ground=new Node(scene);
	{
 		MeshComponent::ptr mesh=new MeshComponent(engine);
		mesh->setMesh(engine->createAABoxMesh(AABox(-100,-100,-1,100,100,0),static_pointer_cast<Material>(engine->getMaterialManager()->find("C:\\Users\\siralanf\\Pictures\\My Scans\\scan0001.jpg"))));
		ground->attach(mesh);

		ground->setTranslate(0,0,-30);
	}
	scene->getRoot()->attach(ground);

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
