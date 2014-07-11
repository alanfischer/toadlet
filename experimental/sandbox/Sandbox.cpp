#include "Sandbox.h"
#include <toadlet/tadpole/plugins/AssimpHandler.h>

Sandbox::Sandbox(Application *app){
	this->app=app;
}

Sandbox::~Sandbox(){
}

void Sandbox::create(){
	engine=app->getEngine();
	engine->getArchiveManager()->addDirectory("..");

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
// 		MeshComponent::ptr mesh=new MeshComponent(engine);
//		mesh->setMesh("spider.obj");
//		lt->attach(mesh);

		AssimpHandler::ptr handler=new AssimpHandler(engine);
		Node::ptr node=handler->load(new FileStream("../duck.dae",FileStream::Open_READ_BINARY),"");
		lt->setScale(100);
		lt->attach(node);

//		PhysicsComponent::ptr physics=scene->getPhysicsManager()->createPhysicsComponent();
//		physics->setMass(1);
//		physics->setBound(new Bound(Sphere(1)));
//		lt->attach(physics);
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

void Sandbox::destroy(){
	scene->destroy();
}

void Sandbox::render(){
	RenderDevice *renderDevice=engine->getRenderDevice();

	renderDevice->beginScene();
		camera->getCamera()->render(renderDevice,scene);
	renderDevice->endScene();
	renderDevice->swap();
}

void Sandbox::update(int dt){
	scene->update(dt);
}

Applet *createApplet(Application *app){return new Sandbox(app);}
