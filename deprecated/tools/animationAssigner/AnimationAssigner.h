#ifndef ANIMATIONASSIGNER_H
#define ANIMATIONASSIGNER_H

#include <toadlet/tadpole/Engine.h>
#include <fox/fx.h>
#include "FXGLToadletMeshViewer.h"

class AnimationAssigner:public FXApp{
	FXDECLARE(AnimationAssigner);

public:
	enum{
		ID_LOAD=FXApp::ID_LAST,
		ID_SAVE,
		ID_UP,
		ID_DOWN,
		ID_ADD,
		ID_DELETE,
		ID_ANIMATION,
	};

	AnimationAssigner();
	virtual ~AnimationAssigner();

	virtual void init(int &argc,char **argv,bool connect=true);
	virtual bool load(const toadlet::egg::String &name);
	virtual bool save(const toadlet::egg::String &name);

	long onLoad(FXObject*,FXSelector,void*);
	long onSave(FXObject*,FXSelector,void*);
	long onUp(FXObject*,FXSelector,void*);
	long onDown(FXObject*,FXSelector,void*);
	long onAdd(FXObject*,FXSelector,void*);
	long onDelete(FXObject*,FXSelector,void*);
	long onAnimation(FXObject*,FXSelector,void*);

protected:
	void updateAnimationList();

	FXMainWindow *mWindow;
	FXMenuPane *mFileMenu;
	FXLabel *mAnimationLabel;
	FXList *mAnimationList;
	FXGLToadletMeshViewer *mMeshViewer;

	toadlet::tadpole::Engine *mEngine;
	toadlet::tadpole::mesh::MeshData::Ptr mMeshData;
	toadlet::tadpole::skeleton::SkeletonData::Ptr mSkeletonData;
};

#endif
