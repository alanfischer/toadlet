#include "Logo.h"
#include "lt_xmsh.h"
#include "lt_mmsh.h"

class GravityFollower:public NodeListener,MotionDetectorListener{
public:
	GravityFollower(MotionDetector *detector,float offset){
		mDetector=detector;
		mDetector->setListener(this);
		mOffset=offset;
	}

	void nodeDestroyed(Node *node){
		mDetector->setListener(NULL);
	}

	void logicUpdate(Node *node,int dt){
		mLastTranslate.set(mTranslate);
		mLastRotate.set(mRotate);

		mMotionMutex.lock();

			Vector3 up;
			// When the phone is vertical, we're at 0,-1,0
			// When the phone is horizontal, we're at 1,0,0
			// Grab just the x,y component of this, and move it to x,z, since we are looking along the y.
			// Store the y component to use for calculating our eye height
			up.set(mMotionData.acceleration);
			scalar z=up.z;
			up.z=up.y;
			up.y=0;
			if(Math::normalizeCarefully(up,0)){
				Vector3 eye(0,-z-Math::ONE,-z);
				Math::normalize(eye);
				Math::mul(eye,Math::fromInt(150));

				((CameraNode*)node)->setLookAt(eye,Math::ZERO_VECTOR3,up);
				mTranslate.set(node->getTranslate());
				mRotate.set(node->getRotate());
			}

		mMotionMutex.unlock();
	}
	
	void frameUpdate(Node *node,int dt){
		Vector3 translate;
		Math::lerp(translate,mLastTranslate,mTranslate,node->getScene()->getLogicFraction());
		node->setTranslate(translate);
		Quaternion rotate;
		Math::slerp(rotate,mLastRotate,mRotate,node->getScene()->getLogicFraction());
		node->setRotate(rotate);
	}

	void motionDetected(const MotionDetector::MotionData &motionData){
		mMotionMutex.lock();
		mMotionData.set(motionData);
		mMotionMutex.unlock();
	}

	MotionDetector *mDetector;
	float mOffset;
	Mutex mMotionMutex;
	MotionDetector::MotionData mMotionData;
	Vector3 mTranslate,mLastTranslate;
	Quaternion mRotate,mLastRotate;
};

// To keep this example as simple as possible, it does not require any other data files, instead getting its data from lt_xmsh
Logo::Logo():Application(){
}

Logo::~Logo(){
}

void Logo::create(){
	Application::create(Application::RendererPlugin_D3D10);

	scene=Scene::ptr(new Scene(mEngine));

	MemoryStream::ptr in(new MemoryStream(lt_mmsh::data,lt_mmsh::length,lt_mmsh::length,false));
	Mesh::ptr mesh=shared_static_cast<Mesh>(getEngine()->getMeshManager()->findHandler("mmsh")->load(in,NULL));
	//MemoryStream::ptr in(new MemoryStream(lt_xmsh::data,lt_xmsh::length,lt_xmsh::length,false));
	//Mesh::ptr mesh=shared_static_cast<Mesh>(getEngine()->getMeshManager()->findHandler("xmsh")->load(in,NULL));

 	meshNode=getEngine()->createNodeType(MeshNode::type(),scene);
	meshNode->setMesh(mesh);
	meshNode->getAnimationController()->start();
	meshNode->getAnimationController()->setCycling(MeshNode::MeshAnimationController::Cycling_REFLECT);
	scene->getRoot()->attach(meshNode);

	cameraNode=getEngine()->createNodeType(CameraNode::type(),scene);
	cameraNode->setLookAt(Vector3(0,-Math::fromInt(150),0),Math::ZERO_VECTOR3,Math::Z_UNIT_VECTOR3);
	cameraNode->setClearColor(Colors::BLUE);
	scene->getRoot()->attach(cameraNode);

	MotionDetector *motionDetector=getMotionDetector();
	if(motionDetector!=NULL){
		cameraNode->addNodeListener(NodeListener::ptr(new GravityFollower(motionDetector,Math::length(cameraNode->getTranslate()))));
		motionDetector->startup();
	}
}

void Logo::resized(int width,int height){
	if(cameraNode!=NULL && width>0 && height>0){
		if(width>=height){
			cameraNode->setProjectionFovY(Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(width),Math::fromInt(height)),Math::fromInt(10),Math::fromInt(200));
		}
		else{
			cameraNode->setProjectionFovX(Math::degToRad(Math::fromInt(45)),Math::div(Math::fromInt(height),Math::fromInt(width)),Math::fromInt(10),Math::fromInt(200));
		}
		cameraNode->setViewport(Viewport(0,0,width,height));
	}
}

void Logo::render(Renderer *renderer){
	renderer->beginScene();
		scene->render(renderer,cameraNode,NULL);
	renderer->endScene();
	renderer->swap();
}

void Logo::update(int dt){
	scene->update(dt);
}

#if !defined(TOADLET_PLATFORM_OSX)
#if defined(TOADLET_PLATFORM_WINCE)
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow){
	Logo app;
	app.setFullscreen(true);
#else
int main(int argc,char **argv){
	Logo app;
#endif
	app.create();
	app.start();
	app.destroy();
	return 0;
}
#endif
