#ifndef FXGLTOADLETMESHVIEWER_H
#define FXGLTOADLETMESHVIEWER_H

#include <toadlet/peeper/RenderTarget.h>
#include <toadlet/peeper/plugins/glrenderer/GLRenderTargetPeer.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/sg/MeshEntity.h>
#include <fox/fx.h>
#include <fox/FXGLCanvas.h>

class FXGLToadletMeshViewer;
class Axis;

class FXGLToadletMeshViewerPeer:public toadlet::peeper::GLRenderTargetPeer{
public:
	FXGLToadletMeshViewerPeer(FXGLToadletMeshViewer *viewer);

	void getSize(int &width,int &height) const;

	void makeCurrent();

protected:
	FXGLToadletMeshViewer *mViewer;
};

class FXGLToadletMeshViewer:public FXGLCanvas,public toadlet::peeper::RenderTarget{
	FXDECLARE(FXGLToadletMeshViewer)

public:
	FXGLToadletMeshViewer();
	FXGLToadletMeshViewer(FXComposite* p,FXGLVisual *vis,FXObject* tgt=NULL,FXSelector sel=0,FXuint opts=0,FXint x=0,FXint y=0,FXint w=0,FXint h=0);
	virtual ~FXGLToadletMeshViewer();

	void create();

	void initialize(toadlet::tadpole::Engine *engine);

	long onPaint(FXObject*,FXSelector,void*);

	void setMeshData(toadlet::tadpole::mesh::MeshData::Ptr meshData);

	void setActiveAnimation(int i);
	int getActiveAnimation();

	void updateCamera();

	long onUpdate(FXObject*,FXSelector,void*);

protected:
	void init();

	FXGLToadletMeshViewerPeer *mPeer;
	toadlet::peeper::Renderer *mRenderer;
	toadlet::tadpole::Engine *mEngine;
	toadlet::tadpole::sg::SceneManager::Ptr mSceneManager;
	toadlet::tadpole::sg::MeshEntity::Ptr mMeshEntity;
	toadlet::tadpole::mesh::MeshData::Ptr mMeshData;
	toadlet::tadpole::skeleton::SkeletonController *mController;
	toadlet::tadpole::sg::CameraEntity::Ptr mCameraEntity;
	toadlet::tadpole::sg::LightEntity::Ptr mLightEntity;
	toadlet::egg::SharedPointer<Axis> mAxis;
	float mDistance;
	int mActiveAnimation;
};

#endif
