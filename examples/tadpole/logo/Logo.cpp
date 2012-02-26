#include "Logo.h"

class GravityFollower:public BaseComponent,InputDeviceListener{
public:
	GravityFollower(InputDevice *device):BaseComponent(){
		mDevice=device;
		mDevice->setListener(this);
	}

	void destroy(){
		mDevice->setListener(NULL);
	}

	bool parentChanged(Node *node){
		mNode=node;
		return true;
	}

	void logicUpdate(int dt,int scope){
		mLastTranslate.set(mTranslate);
		mLastRotate.set(mRotate);

		mMotionMutex.lock();
			if(mMotionData.values.size()>0){
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

					((CameraNode*)mNode)->setLookAt(eye,Math::ZERO_VECTOR3,up);
					mTranslate.set(mNode->getTranslate());
					mRotate.set(mNode->getRotate());
				}
			}
		mMotionMutex.unlock();
	}
	
	void frameUpdate(int dt,int scope){
		Vector3 translate;
		Math::lerp(translate,mLastTranslate,mTranslate,mNode->getScene()->getLogicFraction());
		mNode->setTranslate(translate);
		Quaternion rotate;
		Math::slerp(rotate,mLastRotate,mRotate,mNode->getScene()->getLogicFraction());
		mNode->setRotate(rotate);
	}

	void inputDetected(const InputData &data){
		if(data.type==InputDevice::InputType_MOTION){
			mMotionMutex.lock();
			mMotionData.set(data);
			mMotionMutex.unlock();
		}
	}

	Node *mNode;
	InputDevice *mDevice;
	Mutex mMotionMutex;
	InputData mMotionData;
	Vector3 mTranslate,mLastTranslate;
	Quaternion mRotate,mLastRotate;
};

Logo::Logo(Application *app){
	this->app=app;
}

Logo::~Logo(){
}

void Logo::create(){
	engine=app->getEngine();
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

 	meshNode=engine->createNodeType(MeshNode::type(),scene);
	meshNode->setMesh("lt.xmsh");
//	meshNode->getController()->start();
//	meshNode->getController()->setCycling(Controller::Cycling_REFLECT);
	scene->getRoot()->attach(meshNode);

	cameraNode=engine->createNodeType(CameraNode::type(),scene);
	cameraNode->setLookAt(Vector3(0,-Math::fromInt(150),0),Math::ZERO_VECTOR3,Math::Z_UNIT_VECTOR3);
	cameraNode->setClearColor(Colors::BLUE);
	scene->getRoot()->attach(cameraNode);

// Only looks good if running on device, in simulator its always a top down view
#if 1
	InputDevice *motionDevice=app->getInputDevice(InputDevice::InputType_MOTION);
	if(motionDevice!=NULL){
		cameraNode->attach(new GravityFollower(motionDevice));
		motionDevice->start();
	}
#endif
}

void Logo::destroy(){
	scene->destroy();
}

void Logo::resized(int width,int height){
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

void Logo::render(){
	RenderDevice *renderDevice=engine->getRenderDevice();

	renderDevice->beginScene();
		cameraNode->render(renderDevice);
	renderDevice->endScene();
	renderDevice->swap();
}

void Logo::update(int dt){
	scene->update(dt);
}

Applet *createApplet(Application *app){return new Logo(app);}
void destroyApplet(Applet *applet){delete applet;}