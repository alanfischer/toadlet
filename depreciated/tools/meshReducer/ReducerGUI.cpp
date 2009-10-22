#include "ReducerGUI.h"
#include <toadlet/egg/System.h>
#include <toadlet/egg/io/FileOutputStream.h>
#include <toadlet/egg/math/MathTextSTDStream.h>
#include <toadlet/peeper/BlendFunction.h>
#include <toadlet/tadpole/resource/handler/XMSHHandler.h>
#include <toadlet/tadpole/sg/LightEntity.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::math;
using namespace toadlet::egg::math::Math;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::mesh;
using namespace toadlet::tadpole::resource::handler;
using namespace toadlet::tadpole::sg;
using namespace toadlet::goo;
using namespace toadlet::goo;

extern "C"{
	RenderWindow *new_RenderWindow();
	Renderer *new_Renderer();
}

ReducerGUI::ReducerGUI(Engine *engine,MeshEntity::Ptr mesh,const String &mshFileName){
	int i;
	mThread=0;
	mMshFileName=mshFileName;

	try{
		mEngine = engine;
		mScene=NodeEntity::make(mEngine);
		mEngine->getSceneManager()->attach(mScene);

		mScene->attach(mesh);
	}
	catch(std::exception &e){
		TOADLET_LOG(NULL,Logger::LEVEL_ERROR) <<
			e.what();
	}

	// Determine starting zoom distance
	Vector3 maxs = mesh->getTightBox().getMaxs();
	Vector3 mins = mesh->getTightBox().getMins();
	float d=0;
	for(i=0; i<3; ++i){
		if(d<abs(maxs[i])){
			d=abs(maxs[i]);
		}
		if(d<abs(mins[i])){
			d=abs(mins[i]);
		}
	}
	mDistance=2.5f*d;

	// Setup lights
	LightEntity::Ptr l=LightEntity::make(mEngine);
	l->setDirection(Vector3(0.0,-1.0,1.0));
	l->setDiffuse(Color::COLOR_WHITE);
	l->setType(Light::LT_DIRECTION);
	l->setLinearAttenuation(0);
	mEngine->getSceneManager()->attach(l);

	// Get total number of triangles in mesh
	mMeshData=mesh->getMeshData(); 
	mNumTriangles=0;
	for(i=0;i<mMeshData->getNumSubMeshDatas();++i){
		mNumTriangles+=(mMeshData->getSubMeshData(i)->getIndexBuffer()->getNumIndexes())/3;
	}

	// Setup GUI
	mGUIRenderer=new GUIRenderer(mEngine);
	mGUI=new Container();
	mGUI->setVisible(true);
	{
		mSlider=new SliderControl(0,mNumTriangles,mNumTriangles*0.25f,mNumTriangles);
		mSlider->setLocation(Vector2(0.05f,0.925f));
		mSlider->setSize(Vector2(0.7f,0.05f));
		mSlider->setColor(Vector4(1.0,0.0,0.0,1.0));
		mSlider->setVisible(true);
		mSlider->setSliderChangedHandler(new toadlet::goo::AnonymousMemberEventHandler<ReducerGUI>(this,&ReducerGUI::sliderHandler));
	}
	mGUI->add(mSlider);
	{
		mButton=new ButtonControl("Save");
		mButton->setLocation(Vector2(0.8f,0.925));
		mButton->setSize(Vector2(0.15f,0.05f));
		mButton->setVisible(true);
		mButton->setButtonClickHandler(new toadlet::goo::AnonymousMemberEventHandler<ReducerGUI>(this,&ReducerGUI::buttonHandler));
	}
	mGUI->add(mButton);

	mCamera=CameraEntity::make(mEngine);
	mEngine->getSceneManager()->attach(mCamera);

	// Setup Reducer
	mReducer=new reducer::Reducer(mEngine);
	mReducer->computeProgressiveMesh(mMeshData);
	
	mMouseX=0;
	mMouseY=0;
	mDrag=false;
	mZoom=false;
	mLastTime=0;
}

ReducerGUI::~ReducerGUI(){
	delete mGUIRenderer;
	delete mThread;
}

void ReducerGUI::start(){
	mThread=new Thread(this);
	mThread->start();
}

void ReducerGUI::run(){
	try{
		mRenderWindow=new_RenderWindow();
		mRenderWindow->setRenderWindowListener(this);
		mRenderWindow->startup("ReducerGUI",600,600,false,Visual());

		mRenderer=new_Renderer();
		mRenderer->startup(mRenderWindow);
		mEngine->registerRenderer(mRenderer);

		mRenderWindow->startEventLoop();

		mRenderer->shutdown();

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
	}
	catch(std::exception &e){
		TOADLET_LOG(NULL,Logger::LEVEL_ERROR) <<
			e.what();
	}

	delete mThread;
	mThread=0;
}

void ReducerGUI::update(){
	long t=System::mtime();
	if(t-mLastTime>0){
		long dt=t-mLastTime;
		if(dt>100){
			dt=100;
		}
		mEngine->update(dt/1000.0f);
		mLastTime=t;
	}
}

void ReducerGUI::render(){
	mEngine->updateRendererResources(mRenderer);

	mRenderer->clearBuffer(BUFFER_COLOR|BUFFER_DEPTH,Color::COLOR_BLACK);
	
	// TODO: Find out why model is back lit instead	
	mRenderer->setLighting(true);
	mRenderer->setDepthWrite(true);
	mRenderer->setDepthFunction(DF_LEQUAL);
	mRenderer->setFaceCulling(FC_BACK);
	mRenderer->setBlendFunction(DISABLED_BLENDFUNCTION);

	mCamera->setOriginalFrustum(Frustum(54,1,4,mDistance*3.0));
	mCamera->setAutogenViewport();
	mCamera->setRenderTarget(mRenderWindow);

	Matrix3x3 offset;
	Vector3 eye=offset*Vector3(0,1,0);
	normalize(eye);
	eye*=-mDistance;
	mCamera->setLookAt(eye,ZERO_VECTOR3,Z_UNIT_VECTOR3);

	mEngine->getSceneManager()->setActiveCamera(mCamera);
	mEngine->getSceneManager()->render(mRenderer);

	mRenderer->beginScene();
	mGUIRenderer->render(mRenderer,(Viewport*)&mCamera->getCamera()->getViewport(),mGUI);
	mRenderer->endScene();
	
	mRenderer->swapBuffers();
}

bool ReducerGUI::running(){
	if(mThread){
		return true;
	}
	else{
		return false;
	}
}

void ReducerGUI::mouseMove(unsigned int x,unsigned int y){
	int diffx=mMouseX-x;
	int diffy=mMouseY-y;
	
	if(mDrag){
		float dx=((float)diffx)*0.01f;
		float dy=((float)diffy)*0.01f;

		EulerAngleXYZ ex(-dy,0.0,-dx);
		Matrix3x3 rot=mScene->getRotate();
		rot=makeMatrix3x3FromEulerAngleXYZ(ex)*rot;
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

	mGUI->mouseMove(Vector2((float)mMouseX/(float)mWidth,(float)mMouseY/(float)mHeight));
}

void ReducerGUI::mouseDown(Mouse button){
	if(button==MOUSE_LEFT){
		if(((float)mMouseY/(float)mHeight)>=0.9){
			mGUI->mouseEvent(Vector2((float)mMouseX/(float)mWidth,(float)mMouseY/(float)mHeight),Component::BUTTON_LEFT,Component::STATE_DOWN);
		}
		else{
			mDrag=true;
		}
	}
	else if(button==MOUSE_RIGHT){
		mZoom=true;
	}
}
	
void ReducerGUI::mouseUp(Mouse button){
	if(button==MOUSE_LEFT){
		if(((float)mMouseY/(float)mHeight)>=0.9){
			mGUI->mouseEvent(Vector2((float)mMouseX/(float)mWidth,(float)mMouseY/(float)mHeight),Component::BUTTON_LEFT,Component::STATE_UP);
		}
		else{
			mDrag=false;
		}
	}
	else if(button==MOUSE_RIGHT){
		mZoom=false;
	}
}

void ReducerGUI::focusLost(){
	mDrag=false;
	mZoom=false;
}

void ReducerGUI::sliderHandler(){
	// Reduce the mesh
	float percentReduction=(float)(mSlider->getValue())/(float)mNumTriangles;
	mReducer->doProgressiveMesh(percentReduction);
}

void ReducerGUI::buttonHandler(){
	// Create the output file
	int loc=mMshFileName.find_last_of(".");
	StringStream ss;
	ss << (int)(100*((float)(mSlider->getValue())/(float)mNumTriangles));
	String reducedFileName=mMshFileName.substr(0,loc)+String("_reduced_")+ss.str()+String(".xmsh");
	FileOutputStream fout(reducedFileName);
	XMSHHandler handler;
	handler.save(mMeshData,&fout,reducedFileName);
}
