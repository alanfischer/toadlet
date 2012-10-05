#include "Input.h"

Input::Input(Application *app){
	this->app=app;
}

Input::~Input(){
}

void Input::create(){
	engine=app->getEngine();
	engine->getArchiveManager()->addDirectory("../res");

	scene=new Scene(engine);

	Node::ptr lightNode=new Node(scene);
	{
		LightComponent::ptr light=new LightComponent();
		LightState state;
		state.type=LightState::Type_DIRECTION;
		state.direction=Math::NEG_Z_UNIT_VECTOR3;
		light->setLightState(state);
		lightNode->attach(light);
	}
	scene->getRoot()->attach(lightNode);

	motionLabel=makeLabel("M.png");
	motionLabel->setTranslate(-25,25,0);
	scene->getRoot()->attach(motionLabel);

	for(int i=0;i<3;++i){
		motionNeedle[i]=makeNeedle();
		motionNeedle[i]->setTranslate(5+i*20,25,0);
		scene->getRoot()->attach(motionNeedle[i]);
	}

	proximityLabel=makeLabel("P.png");
	proximityLabel->setTranslate(-25,0,0);
	scene->getRoot()->attach(proximityLabel);

	proximityNeedle=makeNeedle();
	proximityNeedle->setTranslate(25,0,0);
	scene->getRoot()->attach(proximityNeedle);

	lightLabel=makeLabel("L.png");
	lightLabel->setTranslate(-25,-25,0);
	scene->getRoot()->attach(lightLabel);

	lightNeedle=makeNeedle();
	lightNeedle->setTranslate(25,-25,0);
	scene->getRoot()->attach(lightNeedle);

	camera=new Camera();
	camera->setLookAt(Vector3(0,0,Math::fromInt(150)),Math::ZERO_VECTOR3,Math::Y_UNIT_VECTOR3);
	camera->setClearColor(Colors::BLUE);

	InputDevice *motionDevice=app->getInputDevice(InputDevice::InputType_MOTION);
	if(motionDevice!=NULL){
		motionDevice->setListener(this);
		motionDevice->start();
	}
	InputDevice *proximityDevice=app->getInputDevice(InputDevice::InputType_PROXIMITY);
	if(proximityDevice!=NULL){
		proximityDevice->setListener(this);
		proximityDevice->start();
	}
}

void Input::destroy(){
	InputDevice *motionDevice=app->getInputDevice(InputDevice::InputType_MOTION);
	if(motionDevice!=NULL){
		motionDevice->stop();
	}

	InputDevice *proximityDevice=app->getInputDevice(InputDevice::InputType_PROXIMITY);
	if(proximityDevice!=NULL){
		proximityDevice->stop();
	}

	scene->destroy();
}

void Input::render(){
	RenderDevice *device=engine->getRenderDevice();
	
	device->beginScene();
		camera->render(device,scene);
	device->endScene();
	device->swap();
}

void Input::update(int dt){
	scene->update(dt);
}

Node::ptr Input::makeLabel(const String &name){
	Node::ptr node=new Node(scene);
	
	SpriteComponent::ptr sprite=new SpriteComponent(engine);
	sprite->setMaterial(engine->getMaterialManager()->findMaterial(name));
	node->attach(sprite);

	node->setScale(10);
	return node;
}

Node::ptr Input::makeNeedle(){
	Node::ptr node=new Node(scene);

	MeshComponent::ptr eye=new MeshComponent(engine);
	eye->setMesh(engine->createSphereMesh(Sphere(1),engine->getMaterialManager()->findMaterial("P.png")));
	node->attach(eye);

 	MeshComponent::ptr point=new MeshComponent(engine);
	point->setMesh(engine->createAABoxMesh(AABox(-.25,0,-.25,.25,10,.25),engine->getMaterialManager()->findMaterial("P.png")));
	node->attach(point);

	return node;
}

void Input::setNeedle(Node::ptr needle,float value){
	float max=5,min=-5;
	needle->setRotate(Math::Z_UNIT_VECTOR3,((value-min)/(max-min))*Math::HALF_PI);
}

void Input::inputDetected(const InputData &data){
	if(data.type==InputDevice::InputType_MOTION){
		for(int i=0;i<3;++i){
			setNeedle(motionNeedle[i],data.values[InputData::Semantic_MOTION_ACCELERATION][i]);
		}
	}
	if(data.type==InputDevice::InputType_PROXIMITY){
		setNeedle(proximityNeedle,data.values[InputData::Semantic_PROXIMITY][0]);
	}
	if(data.type==InputDevice::InputType_LIGHT){
		setNeedle(lightNeedle,data.values[InputData::Semantic_LIGHT][0]);
	}
}

Applet *createApplet(Application *app){return new Input(app);}
void destroyApplet(Applet *applet){delete applet;}