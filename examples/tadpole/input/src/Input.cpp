#include "Input.h"

class GravityFollower:public NodeListener,InputDeviceListener{
public:
	GravityFollower(InputDevice *device){
		mDevice=device;
		mDevice->setListener(this);
	}

	void nodeDestroyed(Node *node){
		mDevice->setListener(NULL);
	}

	void logicUpdated(Node *node,int dt){
		mLastTranslate.set(mTranslate);
		mLastRotate.set(mRotate);

		mMotionMutex.lock();

			const Vector4 &accel=mMotionData.values[InputData::Semantic_MOTION_ACCELERATION];
			Vector3 up;
			// When the phone is vertical, we're at 0,1,0
			// When the phone is horizontal, we're at 0,0,1
			// Store the z component to use for calculating our eye height
			up.set(-accel.x,-accel.y,-accel.z);
			scalar z=up.z;
			up.z=0;
			if(Math::normalizeCarefully(up,0)){
				Vector3 eye(0,z-Math::ONE,z);
				Math::normalize(eye);
				Math::mul(eye,Math::fromInt(150));

				((CameraNode*)node)->setLookAt(eye,Math::ZERO_VECTOR3,up);
				mTranslate.set(node->getTranslate());
				mRotate.set(node->getRotate());
			}

		mMotionMutex.unlock();
	}
	
	void frameUpdated(Node *node,int dt){
		Vector3 translate;
		Math::lerp(translate,mLastTranslate,mTranslate,node->getScene()->getLogicFraction());
		node->setTranslate(translate);
		Quaternion rotate;
		Math::slerp(rotate,mLastRotate,mRotate,node->getScene()->getLogicFraction());
		node->setRotate(rotate);
	}

	void inputDetected(const InputData &data){
		if(data.type==InputDevice::InputType_MOTION){
			mMotionMutex.lock();
			mMotionData.set(data);
			mMotionMutex.unlock();
		}
	}

	InputDevice *mDevice;
	Mutex mMotionMutex;
	InputData mMotionData;
	Vector3 mTranslate,mLastTranslate;
	Quaternion mRotate,mLastRotate;
};

Input::Input(Application *app){
	this->app=app;
}

Input::~Input(){
}

void Input::create(){

	engine=app->getEngine();
	engine->setDirectory("../res");

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

	cameraNode=engine->createNodeType(CameraNode::type(),scene);
	cameraNode->setLookAt(Vector3(0,0,Math::fromInt(100)),Math::ZERO_VECTOR3,Math::Y_UNIT_VECTOR3);
	cameraNode->setClearColor(Colors::BLUE);
	scene->getRoot()->attach(cameraNode);

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

void Input::resized(int width,int height){
	if(cameraNode!=NULL && width>0 && height>0){
		if(width>=height){
			cameraNode->setProjectionFovY(Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(width),Math::fromInt(height)),Math::fromInt(1),Math::fromInt(200));
		}
		else{
			cameraNode->setProjectionFovX(Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(height),Math::fromInt(width)),Math::fromInt(1),Math::fromInt(200));
		}
		cameraNode->setViewport(Viewport(0,0,width,height));
	}
}

void Input::render(RenderDevice *renderDevice){
	renderDevice->beginScene();
		cameraNode->render(renderDevice);
	renderDevice->endScene();
	renderDevice->swap();
}

void Input::update(int dt){
	scene->update(dt);
}

Node::ptr Input::makeLabel(const String &name){
	SpriteNode::ptr node=engine->createNodeType(SpriteNode::type(),scene);
	node->setMaterial(engine->getMaterialManager()->findMaterial(name));
	node->setScale(10);
	return node;
}

Node::ptr Input::makeNeedle(){
	ParentNode::ptr node=engine->createNodeType(ParentNode::type(),scene);

	MeshNode::ptr eye=engine->createNodeType(MeshNode::type(),scene);
	eye->setMesh(engine->getMeshManager()->createSphereMesh(Sphere(1)));
	node->attach(eye);

 	MeshNode::ptr point=engine->createNodeType(MeshNode::type(),scene);
	point->setMesh(engine->getMeshManager()->createAABoxMesh(AABox(-.25,0,-.25,.25,10,.25)));
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

	if(logStream!=NULL){
		String s=String()+data.type+":"+data.values[0][0]+","+data.values[0][1]+","+data.values[0][2]+","+data.values[0][3]+"\n";
		logStream->write((tbyte*)s.c_str(),s.length());
	}
}

void Input::startLogging(Stream::ptr stream){
	logStream=stream;

	if(logStream!=NULL){
		logStream->write((tbyte*)"START\n",6);
	}
}

void Input::stopLogging(){
	if(logStream!=NULL){
		logStream->close();
		logStream=NULL;
	}
}

Applet *createApplet(Application *app){return new Input(app);}
void destroyApplet(Applet *applet){delete applet;}