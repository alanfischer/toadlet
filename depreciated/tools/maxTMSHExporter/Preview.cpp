#include "Preview.h"

#include <toadlet/egg/math/Math.h>
#include <toadlet/egg/System.h>
#include <toadlet/peeper/BlendFunction.h>
#include <toadlet/tadpole/material/StandardMaterial.h>
#include <toadlet/tadpole/sg/LightEntity.h>
#include <toadlet/egg/math/MathTextSTDStream.h>
#include <iostream>
#include <fstream>

using namespace toadlet::egg::math;
using namespace toadlet::egg;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::sg;
using namespace toadlet::tadpole::mesh;
using namespace toadlet::tadpole::material;
using namespace toadlet::peeper;

extern "C"{
	RenderWindow *new_RenderWindow();
	Renderer *new_Renderer();
}

class Axis:public Entity,Renderable{
public:
	TOADLET_ENTITY(Axis,Entity);

	void create(){
		Super::create();

		mVertexBuffer.resize(VertexBuffer::VT_POSITION,4);
		mIndexBuffer.resize(2);
		mVertexBuffer.position(0)=Vector3(0.0,0.0,0.0);
		mVertexBuffer.position(1)=Vector3(1.0,0.0,0.0);
		mVertexBuffer.position(2)=Vector3(0.0,1.0,0.0);
		mVertexBuffer.position(3)=Vector3(0.0,0.0,1.0);
	}

	void setScale(float s){
		Super::setScale(s,s,s);
	}

	void render(RenderQueue *q,Frustum *f) const{
		q->addRenderable(this);
	}

	bool castsShadows() const{
		return false;
	}

	void render(Renderer *r) const{
		LightEffect le;
		le.trackColor=true	;
		r->setLightEffect(le);
		r->setLighting(false);
		r->disableTextureStage(0);

		mIndexBuffer.index(0)=0;
		mIndexBuffer.index(1)=1;
		r->setColor(Vector4(1,0,0,1));
		r->renderPrimitive(PRIMITIVE_LINES,&mVertexBuffer,&mIndexBuffer);

		mIndexBuffer.index(0)=0;
		mIndexBuffer.index(1)=2;
		r->setColor(Vector4(0,1,0,1));
		r->renderPrimitive(PRIMITIVE_LINES,&mVertexBuffer,&mIndexBuffer);

		mIndexBuffer.index(0)=0;
		mIndexBuffer.index(1)=3;
		r->setColor(Vector4(0,0,1,1));
		r->renderPrimitive(PRIMITIVE_LINES,&mVertexBuffer,&mIndexBuffer);
	}

	const Transform &getTransform() const{return mWorldTransform;}
	const Material *getMaterial() const{return NULL;}
	bool isTransparent() const{return true;}

protected:
	mutable VertexBuffer mVertexBuffer;
	mutable IndexBuffer mIndexBuffer;
};

Preview::Preview(Engine *engine, MeshData::Ptr data){
	Axis::Ptr axis;

	#ifndef TOADLET_DEBUG
		try{
	#endif
			mEngine=engine;
			mScene=NodeEntity::make(mEngine);
			mEngine->getSceneManager()->attach(mScene);

			mMesh=MeshEntity::make(mEngine);
			mMesh->load(data);
			if(mMesh->getSkeletonSystem()){
				//mMesh->setRenderSkeleton(true);
				mMesh->getSkeletonSystem()->addController("main")->startAnimation(0,true);
			}
			mScene->attach(mMesh);

			mCamera=CameraEntity::make(mEngine);
			mEngine->getSceneManager()->attach(mCamera);

			axis=Axis::make(mEngine);
			mScene->attach(axis);
	#ifndef TOADLET_DEBUG
		}
		catch(std::exception &e){
			TOADLET_LOG(NULL,Logger::LEVEL_ERROR) << e.what();
		}
	#endif

	Vector3 maxs = mMesh->getTightBox().getMaxs();
	Vector3 mins = mMesh->getTightBox().getMins();
	float d=0;
	for(int i=0; i<3; ++i){
		if(d<abs(maxs[i])){
			d=abs(maxs[i]);
		}
		if(d<abs(mins[i])){
			d=abs(mins[i]);
		}
	}
	mDistance=1.75f*d;

	mCamera->setOriginalFrustum(Frustum(90,1,0.1,mDistance*3.0));
	mCamera->setAutogenViewport();

	axis->setScale(0.5*d);

	mMouseX=0;
	mMouseY=0;
	mDrag=false;
	mZoom=false;
	mLastTime=0;
}

Preview::~Preview(){
}

void Preview::setLights(const Collection<toadlet::peeper::Light> &lights){
	mEngine->getSceneManager()->getSceneStates()->ambientLight=Vector4(0.25f,0.25f,0.25f,1.0f);

	int i;
	if(lights.size()==0){
		LightEntity::Ptr l=LightEntity::make(mEngine);
		l->setType(Light::LT_DIRECTION);
		l->setDirection(Vector3(0,-1,1));
		l->setDiffuse(Color::COLOR_WHITE);
		l->setSpecular(Color::COLOR_WHITE);
		mEngine->getSceneManager()->attach(l);
	}
	else{
		for(i=0;i<lights.size();i++){
			const Light &light=lights[i];

			LightEntity::Ptr l=LightEntity::make(mEngine);
			*(l->internal_getLight())=light;
			l->setTranslate(light.getPosition());
			mEngine->getSceneManager()->attach(l);
		}
	}
}

void Preview::start(){
	mThread=new Thread(this);
	mThread->start();
}

void Preview::run(){
	#ifndef TOADLET_DEBUG
		try{
	#endif
			mRenderWindow=new_RenderWindow();
			mRenderWindow->startup("Preview",640,480,false,Visual());
			mRenderWindow->setRenderWindowListener(this);

			mRenderer=new_Renderer();
			mRenderer->startup(mRenderWindow);
			mEngine->registerRenderer(mRenderer);

			mCamera->getCamera()->setRenderTarget(mRenderWindow);

			LightEntity::Ptr light;
			int i;
			for(i=0;i<mEngine->getSceneManager()->getNumChildren();++i){
				light=shared_dynamic_cast<LightEntity>(mEngine->getSceneManager()->getChild(i));
				if(light!=NULL){
					break;
				}
			}

			mRenderWindow->startEventLoop();

			mRenderer->shutdown();
			mRenderWindow->shutdown();

			if(mEngine){
				delete mEngine;
				mEngine=0;
			}
			if(mRenderer){
				delete mRenderer;
				mRenderer=0;
			}
			if(mRenderWindow){
				delete mRenderWindow;
				mRenderWindow=0;
			}
	#ifndef TOADLET_DEBUG
		}
		catch(std::exception &e){
			TOADLET_LOG(NULL,Logger::LEVEL_ERROR) << e.what();
		}
	#endif

	delete mThread;
	mThread=0;

	delete this;
}

void Preview::update(){
	unsigned int t=System::mtime();
	if(t-mLastTime>0){
		unsigned int dt=t-mLastTime;
		if(dt>100){
			dt=100;
		}

		mEngine->update(dt/1000.0f);
		mLastTime=t;

		if(mEngine->getFPSUpdated()){
			std::cout << "FPS:" << mEngine->getFPS() << std::endl;
		}
	}
}

void Preview::render(){
	mEngine->updateRendererResources(mRenderer);

	mRenderer->clearBuffer(BUFFER_COLOR|BUFFER_DEPTH,Vector4(0,0,0,0));

	mEngine->getSceneManager()->setActiveCamera(mCamera);
	mEngine->getSceneManager()->render(mRenderer);

	mRenderer->swapBuffers();
}

void Preview::mouseMove(unsigned int x,unsigned int y){
	int diffx=mMouseX-x;
	int diffy=mMouseY-y;
	
	if(mDrag){
		float dx=((float)diffx)*0.01f;
		float dy=((float)diffy)*0.01f;

		EulerAngleXYZ ex(-dy,0.0,-dx);
		Matrix3x3 rot=mScene->getRotate();
		rot=Math::makeMatrix3x3FromEulerAngleXYZ(ex)*rot;
		mScene->setRotate(rot);
	}
	if(mZoom){
		float dy=((float)diffy)*0.1f;

		mDistance-=dy;
		if(mDistance<0){
			mDistance=0;
		}
	}
	
	mMouseX=x;
	mMouseY=y;

	mCamera->getCamera()->setAutogenFrustum(90,4,mDistance*3.0f);

	Vector3 eye=Vector3(0,1,0);
	Math::normalize(eye);
	eye*=-mDistance;
	mCamera->setLookAt(eye,Vector3(0,0,0),Vector3(0,0,1));
}

void Preview::mouseDown(Mouse button){
	if(button==MOUSE_LEFT){
		mDrag=true;
	}
	else if(button==MOUSE_RIGHT){
		mZoom=true;
	}
}
	
void Preview::mouseUp(Mouse button){
	if(button==MOUSE_LEFT){
		mDrag=false;
	}
	else if(button==MOUSE_RIGHT){
		mZoom=false;
	}
}

void Preview::focusLost(){
	mDrag=false;
	mZoom=false;
}

