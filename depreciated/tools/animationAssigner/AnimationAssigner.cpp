#include "AnimationAssigner.h"
#include <toadlet/egg/io/FileOutputStream.h>
#include <toadlet/Win32ChooseLibs.h>
#include <fox/FXGLVisual.h>

#pragma comment(lib,"opengl32.lib")

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::mesh;
using namespace toadlet::tadpole::skeleton;

#ifdef TOADLET_DEBUG
#	pragma comment(lib,"FOXD-1.6.lib")
#else
#	pragma comment(lib,"FOX-1.6.lib")
#endif

AnimationAssigner::AnimationAssigner():FXApp("Animation Assigner","Lightning Toads"){
	mEngine=Engine::make();
}

AnimationAssigner::~AnimationAssigner(){
	delete mFileMenu;
	delete mEngine;
}

void AnimationAssigner::init(int &argc,char **argv,bool connect){
	FXApp::init(argc,argv,connect);

	FXApp::create();

	mWindow=new FXMainWindow(this,"Animation Assigner",NULL,NULL,DECOR_ALL,200,200,640,480);
	mWindow->setTarget(this);

	FXMenuBar *menuBar=new FXMenuBar(mWindow,LAYOUT_SIDE_TOP|PACK_UNIFORM_WIDTH|PACK_UNIFORM_HEIGHT|FRAME_RAISED|LAYOUT_FILL_X);

	mFileMenu=new FXMenuPane(mWindow);
	new FXMenuTitle(menuBar,"&File",NULL,mFileMenu);
	new FXMenuCommand(mFileMenu,"&Load",NULL,this,ID_LOAD);
	new FXMenuCommand(mFileMenu,"&Save",NULL,this,ID_SAVE);
	new FXMenuCommand(mFileMenu,"&Quit",NULL,this,ID_QUIT);

	FXHorizontalFrame *hframe=new FXHorizontalFrame(mWindow,LAYOUT_FILL_X|LAYOUT_FILL_Y);

	FXGLVisual *visual=new FXGLVisual(this,VISUAL_DOUBLEBUFFER);
	mMeshViewer=new FXGLToadletMeshViewer(hframe,visual,this,0,LAYOUT_SIDE_LEFT|LAYOUT_FILL_X|LAYOUT_FILL_Y);
	mMeshViewer->initialize(mEngine);

	FXVerticalFrame *vframe=new FXVerticalFrame(hframe,LAYOUT_SIDE_RIGHT|LAYOUT_FILL_Y);

	mAnimationLabel=new FXLabel(vframe,"Animations",0,LAYOUT_CENTER_X);

	FXVerticalFrame *vframe3=new FXVerticalFrame(vframe,FRAME_SUNKEN|FRAME_THICK|LAYOUT_FILL_Y|LAYOUT_FILL_X,0,0,0,0,0,0,0,0);
	mAnimationList=new FXList(vframe3,this,ID_ANIMATION,LIST_EXTENDEDSELECT|LAYOUT_FILL_X|LAYOUT_FILL_Y);

	FXVerticalFrame *vframe2=new FXVerticalFrame(vframe,LAYOUT_CENTER_X|LAYOUT_SIDE_BOTTOM);
	FXHorizontalFrame *hframe2=new FXHorizontalFrame(vframe,LAYOUT_FILL_X,0,0,0,0,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING,DEFAULT_SPACING,10,DEFAULT_SPACING);
	new FXButton(hframe2," Move Up ",NULL,this,ID_UP,FRAME_RAISED|FRAME_THICK|LAYOUT_FILL_X);
	new FXButton(hframe2,"Move Down",NULL,this,ID_DOWN,FRAME_RAISED|FRAME_THICK|LAYOUT_FILL_X);
	new FXButton(hframe2,"   Add   ",NULL,this,ID_ADD,FRAME_RAISED|FRAME_THICK|LAYOUT_FILL_X);
	new FXButton(hframe2," Delete ",NULL,this,ID_DELETE,FRAME_RAISED|FRAME_THICK|LAYOUT_FILL_X);

	mWindow->create();
	mWindow->show(PLACEMENT_SCREEN);

	if(argc>1){
		load(argv[1]);
	}
}

bool AnimationAssigner::load(const String &name){
	mMeshData=mEngine->getMeshDataManager()->load(name);

	if(mMeshData!=NULL){
		mSkeletonData=mMeshData->getSkeletonData();
		if(mSkeletonData==NULL){
			FXMessageBox::error(mWindow,MBOX_OK,"Error","No skeleton exists in the selected mesh");
			return false;
		}

		int i1=name.rfind('/');
		int i2=name.rfind("\\");
		if(i1<i2){
			i1=i2;
		}
		String shortName=name.substr(i1+1,name.length());
		mWindow->setTitle(("Animation Assigner: "+shortName).c_str());

		mAnimationLabel->setText((shortName+" animations").c_str());

		updateAnimationList();

		mMeshViewer->setMeshData(mMeshData);
		mMeshViewer->setActiveAnimation(mAnimationList->getCurrentItem());
	}

	return (mMeshData!=NULL);
}

void AnimationAssigner::updateAnimationList(){
	mAnimationList->clearItems();

	int i;
	for(i=0;i<mSkeletonData->getNumAnimationDatas();++i){
		AnimationData::Ptr animation=mSkeletonData->getAnimationData(i);
		if(animation!=NULL && animation->simpleName.length()>0){
			mAnimationList->appendItem(animation->simpleName.c_str());
		}
		else{
			StringStream ss;
			ss << "Animation " << (i+1);
			mAnimationList->appendItem(ss.str().c_str());
		}
	}
}

bool AnimationAssigner::save(const String &name){
	FileOutputStream fout(name);
	bool result=mEngine->getMeshDataManager()->findHandlerForName(name)->save(mMeshData,&fout,name);
	return result;
}

long AnimationAssigner::onLoad(FXObject*,FXSelector,void*){
	bool result=false;
	FXFileDialog dialog(this,"Load Mesh");
	dialog.setPatternList("*.xmsh\n*.*");
	if(dialog.execute()){
		result=load(dialog.getFilename().text());
	}

	if(result){
		return 1;
	}
	else{
		return -1;
	}
}

long AnimationAssigner::onSave(FXObject*,FXSelector,void*){
	bool result=false;
	FXFileDialog dialog(this,"Save Mesh");
	dialog.setPatternList("*.xmsh\n*.*");
	if(dialog.execute()){
		result=save(dialog.getFilename().text());
	}

	if(result){
		return 1;
	}
	else{
		return -1;
	}
}

long AnimationAssigner::onUp(FXObject*,FXSelector,void*){
	int current=mMeshViewer->getActiveAnimation();

	int i;
	for(i=0;i<mAnimationList->getNumItems();++i){
		if(mAnimationList->isItemSelected(i)){
			if(i>0){
				if(i==current){
					current--;
				}

				AnimationData::Ptr temp=mSkeletonData->getAnimationData(i-1);
				mSkeletonData->setAnimationData(i-1,mSkeletonData->getAnimationData(i));
				mSkeletonData->setAnimationData(i,temp);

				mAnimationList->moveItem(i,i-1);
			}
			else{
				return -1;	
			}
		}
	}

	mMeshViewer->setMeshData(mMeshData);
	mMeshViewer->setActiveAnimation(current);

	return 1;
}

long AnimationAssigner::onDown(FXObject*,FXSelector,void*){
	int current=mMeshViewer->getActiveAnimation();

	int i;
	for(i=mAnimationList->getNumItems()-1;i>=0;--i){
		if(mAnimationList->isItemSelected(i)){
			if(i<mAnimationList->getNumItems()-1){
				if(i==current){
					current++;
				}

				AnimationData::Ptr temp=mSkeletonData->getAnimationData(i+1);
				mSkeletonData->setAnimationData(i+1,mSkeletonData->getAnimationData(i));
				mSkeletonData->setAnimationData(i,temp);

				mAnimationList->moveItem(i,i+1);
			}
			else{
				return -1;
			}
		}
	}

	mMeshViewer->setMeshData(mMeshData);
	mMeshViewer->setActiveAnimation(current);

	return 1;
}

long AnimationAssigner::onAdd(FXObject*,FXSelector,void*){
	if(mSkeletonData==NULL){
		return -1;
	}

	FXFileDialog dialog(this,"Load Animation");
	dialog.setPatternList("*.xanm\n*.*");
	if(dialog.execute()){
		AnimationData::Ptr animationData=mEngine->getAnimationDataManager()->load(dialog.getFilename().text());
		if(animationData!=NULL){
			if(animationData->tracks.size()!=mSkeletonData->getNumBoneDatas()){
				FXMessageBox::error(mWindow,MBOX_OK,"Error","Number of bones in current mesh & animation do not match");
			}
			else{
				mSkeletonData->addAnimationData(animationData);
			}
		}
	}

	updateAnimationList();

	mMeshViewer->setMeshData(mMeshData);
	mMeshViewer->setActiveAnimation(mAnimationList->getCurrentItem());

	return 1;
}

long AnimationAssigner::onDelete(FXObject*,FXSelector,void*){
	if(mSkeletonData==NULL){
		return -1;
	}

	int i;
	for(i=0;i<mAnimationList->getNumItems();++i){
		if(mAnimationList->isItemSelected(i)){
			mSkeletonData->removeAnimationData(i);
			mAnimationList->removeItem(i);
			i--;
		}
	}

	updateAnimationList();

	mMeshViewer->setMeshData(mMeshData);
	mMeshViewer->setActiveAnimation(mAnimationList->getCurrentItem());

	return 1;
}

long AnimationAssigner::onAnimation(FXObject*,FXSelector,void*){
	mMeshViewer->setActiveAnimation(mAnimationList->getCurrentItem());
	return 1;
}

FXDEFMAP(AnimationAssigner) AnimationAssignerMap[]={
	FXMAPFUNC(SEL_COMMAND,AnimationAssigner::ID_LOAD,AnimationAssigner::onLoad),
	FXMAPFUNC(SEL_COMMAND,AnimationAssigner::ID_SAVE,AnimationAssigner::onSave),
	FXMAPFUNC(SEL_COMMAND,AnimationAssigner::ID_UP,AnimationAssigner::onUp),
	FXMAPFUNC(SEL_COMMAND,AnimationAssigner::ID_DOWN,AnimationAssigner::onDown),
	FXMAPFUNC(SEL_COMMAND,AnimationAssigner::ID_ADD,AnimationAssigner::onAdd),
	FXMAPFUNC(SEL_COMMAND,AnimationAssigner::ID_DELETE,AnimationAssigner::onDelete),
	FXMAPFUNC(SEL_COMMAND,AnimationAssigner::ID_ANIMATION,AnimationAssigner::onAnimation),
};

FXIMPLEMENT(AnimationAssigner,FXApp,AnimationAssignerMap,ARRAYNUMBER(AnimationAssignerMap))
