#include "Logo.h"

Logo::Logo(Application *app){
	this->app=app;
}

Logo::~Logo(){
}

void Logo::create(){
Logger::alert("CREATE er");
	engine=app->getEngine();
Logger::alert("CREATE era");
	engine->setDirectory("../../../data");

	scene=Scene::ptr(new Scene(engine));

//	DecalShadowSceneRenderer::ptr sceneRenderer(new DecalShadowSceneRenderer(scene));
//	sceneRenderer->setPlane(Plane(Math::Z_UNIT_VECTOR3,-30));
//	scene->setSceneRenderer(sceneRenderer);

	LightNode::ptr light=engine->createNodeType(LightNode::type(),scene);
	LightState state;
	state.type=LightState::Type_DIRECTION;
	state.direction=Math::NEG_Z_UNIT_VECTOR3;
	light->setLightState(state);
	scene->getRoot()->attach(light);

	Node::ptr lt=engine->createNodeType(LightNode::type(),scene);
	{
 		MeshNode::ptr mesh=engine->createNodeType(MeshNode::type(),scene);
		mesh->setMesh("lt.tmsh");
		lt->attach(mesh);

		if(mesh!=NULL){
			AnimationActionComponent::ptr animation=new AnimationActionComponent("animation");
			animation->attach(new MeshAnimation(mesh,0));
			animation->setCycling(AnimationActionComponent::Cycling_REFLECT);
			animation->start();
			lt->attach(animation);
		}
	}
	scene->getRoot()->attach(lt);

	camera=engine->createNodeType(CameraNode::type(),scene);
	camera->setLookAt(Vector3(0,-Math::fromInt(150),0),Math::ZERO_VECTOR3,Math::Z_UNIT_VECTOR3);
	camera->setClearColor(Colors::BLUE);
	scene->getRoot()->attach(camera);
}

void Logo::destroy(){
	scene->destroy();
}

void Logo::render(){
	RenderDevice *renderDevice=engine->getRenderDevice();

	renderDevice->beginScene();
		camera->render(renderDevice);
	renderDevice->endScene();
	renderDevice->swap();
}

void Logo::update(int dt){
	scene->update(dt);
}

Applet *createApplet(Application *app){return new Logo(app);}
void destroyApplet(Applet *applet){delete applet;}