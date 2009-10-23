#include "FXGLToadletMeshViewer.h"
#include <fox/FXGLVisual.h>
#include <toadlet/peeper/LightEffect.h>
#define TOADLET_USE_GLRENDERER
#include <toadlet/Win32ChooseLibs.h>

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::egg::math;
using namespace toadlet::egg::math::Math;
using namespace toadlet::peeper;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::sg;
using namespace toadlet::tadpole::mesh;
using namespace toadlet::tadpole::material;

#define UPDATE_TIME 20
#define ID_UPDATE 500

extern "C" {
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
		Super::setScale(Vector3(s,s,s));
	}

	void render(RenderQueue *q,Frustum *f) const{
		q->addRenderable(this);
	}

	void render(Renderer *r) const{
		r->setLighting(false);
		r->disableTextureStage(0);
		LightEffect le;
		le.trackColor=true;
		r->setLightEffect(le);

		mIndexBuffer.index(0)=0;
		mIndexBuffer.index(1)=1;
		r->setColor(Color::COLOR_RED);
		r->renderPrimitive(PRIMITIVE_LINES,&mVertexBuffer,&mIndexBuffer);

		mIndexBuffer.index(0)=0;
		mIndexBuffer.index(1)=2;
		r->setColor(Color::COLOR_GREEN);
		r->renderPrimitive(PRIMITIVE_LINES,&mVertexBuffer,&mIndexBuffer);

		mIndexBuffer.index(0)=0;
		mIndexBuffer.index(1)=3;
		r->setColor(Color::COLOR_BLUE);
		r->renderPrimitive(PRIMITIVE_LINES,&mVertexBuffer,&mIndexBuffer);
	}

	const Transform &getTransform() const{return mWorldTransform;}
	const Material *getMaterial() const{return NULL;}
	bool isTransparent() const{return true;}
	bool castsShadows() const{return false;}

protected:
	mutable VertexBuffer mVertexBuffer;
	mutable IndexBuffer mIndexBuffer;
};

FXGLToadletMeshViewerPeer::FXGLToadletMeshViewerPeer(FXGLToadletMeshViewer *viewer):mViewer(viewer){
}

void FXGLToadletMeshViewerPeer::getSize(int &width,int &height) const{
	width=mViewer->getWidth();
	height=mViewer->getHeight();
}

void FXGLToadletMeshViewerPeer::makeCurrent(){
	mViewer->makeCurrent();
}

FXGLToadletMeshViewer::FXGLToadletMeshViewer(){
	mPeer=new FXGLToadletMeshViewerPeer(this);

	internal_setRenderTargetPeer(mPeer);

	mController=NULL;
}

FXGLToadletMeshViewer::FXGLToadletMeshViewer(FXComposite* p,FXGLVisual *vis,FXObject* tgt,FXSelector sel,FXuint opts,FXint x,FXint y,FXint w,FXint h):FXGLCanvas(p,vis,tgt,sel,opts,x,y,w,h){
	mPeer=new FXGLToadletMeshViewerPeer(this);

	internal_setRenderTargetPeer(mPeer);

	mController=NULL;
}

void FXGLToadletMeshViewer::initialize(Engine *engine){
	mEngine=engine;

	mSceneManager=SceneManager::make(mEngine);
	mEngine->setSceneManager(mSceneManager);

	mMeshEntity=MeshEntity::make(mEngine);
	mSceneManager->attach(mMeshEntity);

	mCameraEntity=CameraEntity::make(mEngine);
	mSceneManager->attach(mCameraEntity);

	mAxis=Axis::make(mEngine);
	mSceneManager->attach(mAxis);

	mLightEntity=LightEntity::make(mEngine);
	mLightEntity->setType(Light::LT_DIRECTION);
	mLightEntity->setDirection(Vector3(1,0,1));
	mLightEntity->setDiffuse(Color::COLOR_WHITE);
	mLightEntity->setSpecular(Color::COLOR_WHITE);
	mSceneManager->attach(mLightEntity);

	mSceneManager->getSceneStates()->ambientLight=Vector4(0.25,0.25,0.25,1.0);

	mRenderer=new_Renderer();
}

FXGLToadletMeshViewer::~FXGLToadletMeshViewer(){
	if(mRenderer!=NULL){
		mRenderer->shutdown();
		delete mRenderer;
		mRenderer=NULL;
	}

	if(mPeer!=NULL){
		delete mPeer;
		mPeer=NULL;
	}
}

void FXGLToadletMeshViewer::create(){
	FXGLCanvas::create();

	// If I could pass a RenderTarget to the renderer, then this would not be necessary
	makeCurrent();
	mRenderer->startup(NULL);
	mEngine->registerRenderer(mRenderer);

	mCameraEntity->setAutogenFrustum(90,0.1f,1024.0f);
	mCameraEntity->setRenderTarget(this);
	mCameraEntity->setAutogenViewport();

	updateCamera();

	getApp()->addTimeout(this,ID_UPDATE,UPDATE_TIME);
}

long FXGLToadletMeshViewer::onPaint(FXObject*,FXSelector,void*){
	makeCurrent();

	mEngine->updateRendererResources(mRenderer);

	mRenderer->clearBuffer(BUFFER_COLOR|BUFFER_DEPTH,Color::COLOR_BLACK);

	mSceneManager->setActiveCamera(mCameraEntity);
	mSceneManager->render(mRenderer);

    if(((FXGLVisual*)getVisual())->isDoubleBuffer()){
		swapBuffers();
	}

	return 0;
}

void FXGLToadletMeshViewer::setMeshData(MeshData::Ptr meshData){
	mMeshData=meshData;
	mMeshEntity->load(mMeshData);

	if(mMeshEntity->getSkeletonSystem()!=NULL){
		mController=mMeshEntity->getSkeletonSystem()->addController("main");
	}
	else{
		mController=NULL;
	}

	Vector3 maxs=mMeshEntity->getTightBox().getMaxs();
	Vector3 mins=mMeshEntity->getTightBox().getMins();
	float d=0;
	for(int i=0; i<3; ++i){
		if(d<abs(maxs[i])){
			d=abs(maxs[i]);
		}
		if(d<abs(mins[i])){
			d=abs(mins[i]);
		}
	}
	mDistance=1.5f*d;

	mAxis->setScale(0.5*d);

	updateCamera();
}

void FXGLToadletMeshViewer::setActiveAnimation(int i){
	if(mController!=NULL){
		if(i>=0 && i <mMeshData->getSkeletonData()->getNumAnimationDatas()){
			mController->stopAllAnimations();
			bool result=mController->startAnimation(i,true);
		}
	}
	mActiveAnimation=i;
}

int FXGLToadletMeshViewer::getActiveAnimation(){
	return mActiveAnimation;
}

void FXGLToadletMeshViewer::updateCamera(){
	Matrix3x3 offset;
	Vector3 eye=offset*Vector3(-1,0,0);
	normalize(eye);
	eye*=-mDistance;
	mCameraEntity->setLookAt(eye,Vector3(0,0,0),Vector3(0,0,1));
}

long FXGLToadletMeshViewer::onUpdate(FXObject*,FXSelector,void*){
	mEngine->update((float)UPDATE_TIME/1000.0f);

	update();

	getApp()->addTimeout(this,ID_UPDATE,UPDATE_TIME);

	return 1;
}

FXDEFMAP(FXGLToadletMeshViewer) FXGLToadletMeshViewerMap[]={
	FXMAPFUNC(SEL_PAINT,0,FXGLToadletMeshViewer::onPaint),
	FXMAPFUNC(SEL_TIMEOUT,ID_UPDATE,FXGLToadletMeshViewer::onUpdate),
};

FXIMPLEMENT(FXGLToadletMeshViewer,FXGLCanvas,FXGLToadletMeshViewerMap,ARRAYNUMBER(FXGLToadletMeshViewerMap))
