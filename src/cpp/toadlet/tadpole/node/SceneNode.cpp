/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright 2009, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer, Andrew Fischer
 *
 * This file is part of The Toadlet Engine.
 *
 * The Toadlet Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * The Toadlet Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with The Toadlet Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 ********** Copyright header - do not remove **********/

#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/Profile.h>
#include <toadlet/tadpole/node/CameraNode.h>
#include <toadlet/tadpole/node/Renderable.h>
#include <toadlet/tadpole/node/SceneNode.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/SpacialQuery.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::ribbit;

namespace toadlet{
namespace tadpole{
namespace node{

// Immediate TODO:
//	- Fix Bounding Volumes, so we have Logic & Render volumes properly updated with child volumes
//	- Test that SceneNode returns proper AABoxQuery results
//	- Modify SpacialQuery so it can specify Logic or Render volumes.  Logic volume will use Physics volumes.  Maybe I need to make that more clear somehow.
//	- Modify HopScene so it prunes the results to the Spacial volumes if the bit is set.
//	- Test that SceneNode returns proper AABoxQuery results for logic volumes

TOADLET_NODE_IMPLEMENT(SceneNode,Categories::TOADLET_TADPOLE_NODE+".SceneNode");

SceneNode::SceneNode():super(),
	mChildScene (NULL),

	mExcessiveDT(0),
	mMinLogicDT(0),
	mMaxLogicDT(0),
	mLogicTime(0),
	mLogicFrame(0),
	mAccumulatedDT(0),
	mRenderFrame(0),

	//mBackground,

	mUpdateListener(NULL),

	//mAmbientColor

	mCamera(NULL),
	mPreviousMaterial(NULL)
{}

SceneNode::~SceneNode(){
}

Node *SceneNode::create(Engine *engine){
	super::create(engine);

	setChildScene(this);
	setExcessiveDT(5000);
	setRangeLogicDT(0,0);
	setAmbientColor(Colors::GREY);

	mBackground=mEngine->createNodeType(ParentNode::type());

	return this;
}

void SceneNode::destroy(){
	if(mBackground!=NULL){
		mBackground->destroy();
		mBackground=NULL;
	}

	int i;
	for(i=0;i<mRenderLayers.size();++i){
		if(mRenderLayers[i]!=NULL){
			delete mRenderLayers[i];
		}
	}
	mRenderLayers.clear();

	super::destroy();
}

void SceneNode::setRangeLogicDT(int minDT,int maxDT){
	#if defined(TOADLET_DEBUG)
		if(minDT<0 || maxDT<0){
			Error::unknown(Categories::TOADLET_TADPOLE,
				"setRangeLogicDT must have min & max >= 0");
			return;
		}
	#endif

	mMinLogicDT=minDT;
	mMaxLogicDT=maxDT;
}

void SceneNode::setLogicTimeAndFrame(int time,int frame){
	mLogicTime=time;
	mLogicFrame=frame;
	mAccumulatedDT=0;
	mRenderFrame=0;
}

void SceneNode::update(int dt){
	#if defined(TOADLET_DEBUG)
		if(!created()){
			Error::unknown(Categories::TOADLET_TADPOLE,
				"update called on a destroyed Scene");
			return;
		}
	#endif

	if(mExcessiveDT>0 && dt>mExcessiveDT){
		Logger::alert(Categories::TOADLET_TADPOLE,
			String("skipping excessive dt:")+dt);
		return;
	}

	mAccumulatedDT+=dt;

	if(mAccumulatedDT>=mMinLogicDT){
		mChildScene->preLogicUpdateLoop(dt);

		if(mMaxLogicDT>0){
			while(mAccumulatedDT>0 && mAccumulatedDT>=mMinLogicDT){
				int logicDT=mAccumulatedDT;
				if(mAccumulatedDT>mMaxLogicDT){
					mAccumulatedDT-=mMaxLogicDT;
					logicDT=mMaxLogicDT;
				}
				else{
					mAccumulatedDT=0;
				}

				if(mUpdateListener!=NULL){
					mUpdateListener->preLogicUpdate(logicDT);
					mUpdateListener->logicUpdate(logicDT);
					mUpdateListener->postLogicUpdate(logicDT);
				}
				else{
					mChildScene->preLogicUpdate(logicDT);
					mChildScene->logicUpdate(logicDT);
					mChildScene->postLogicUpdate(logicDT);
				}
			}
		}
		else{
			if(mUpdateListener!=NULL){
				mUpdateListener->preLogicUpdate(dt);
				mUpdateListener->logicUpdate(dt);
				mUpdateListener->postLogicUpdate(dt);
			}
			else{
				mChildScene->preLogicUpdate(dt);
				mChildScene->logicUpdate(dt);
				mChildScene->postLogicUpdate(dt);
			}
		}

		mChildScene->postLogicUpdateLoop(dt);
	}

	if(mUpdateListener!=NULL){
		mUpdateListener->intraUpdate(dt);
	}
	else{
		mChildScene->intraUpdate(dt);
	}

	if(mUpdateListener!=NULL){
		mUpdateListener->preRenderUpdate(dt);
		mUpdateListener->renderUpdate(dt);
		mUpdateListener->postRenderUpdate(dt);
	}
	else{
		mChildScene->preRenderUpdate(dt);
		mChildScene->renderUpdate(dt);
		mChildScene->postRenderUpdate(dt);
	}

	AudioPlayer *audioPlayer=mEngine->getAudioPlayer();
	if(audioPlayer!=NULL){
		audioPlayer->update(dt);
	}
}

void SceneNode::preLogicUpdateLoop(int dt){
}

void SceneNode::preLogicUpdate(int dt){
}

void SceneNode::logicUpdate(int dt){
	logicUpdate(dt,-1);
}

void SceneNode::logicUpdate(int dt,int scope){
	mLogicTime+=dt;
	mLogicFrame++;

	if(mBackground->getNumChildren()>0){
		logicUpdate(mBackground,dt,scope);
	}
	logicUpdate(this,dt,scope);
}

void SceneNode::postLogicUpdate(int dt){
}

void SceneNode::logicUpdate(Node::ptr node,int dt,int scope){
	if((node->mScope&scope)==0){
		return;
	}

	if(node->mReceiveUpdates){
		node->logicUpdate(dt);
	}

	if(node->mParent==NULL){
		node->mWorldScale.set(node->mScale);
		node->mWorldRotate.set(node->mRotate);
		node->mWorldTranslate.set(node->mTranslate);
	}
	else if(node->mIdentityTransform){
		node->mWorldScale.set(node->mParent->mScale);
		node->mWorldRotate.set(node->mParent->mRotate);
		node->mWorldTranslate.set(node->mParent->mTranslate);
	}
	else{
		Math::mul(node->mWorldScale,node->mParent->mWorldScale,node->mScale);
		Math::mul(node->mWorldRotate,node->mParent->mWorldRotate,node->mRotate);
		Math::mul(node->mWorldTranslate,node->mParent->mWorldRotate,node->mTranslate);
		Math::mul(node->mWorldTranslate,node->mParent->mWorldScale);
		Math::add(node->mWorldTranslate,node->mParent->mWorldTranslate);
	}

	ParentNode *parent=node->isParent();
	bool childrenActive=false;
	if(parent!=NULL){
		if(parent->mShadowChildrenDirty){
			parent->updateShadowChildren();
		}

		mul(node->mWorldBound,node->mWorldTranslate,node->mLocalBound);

		int numChildren=parent->mShadowChildren.size();
		Node *child=NULL;
		int i;
		for(i=0;i<numChildren;++i){
			child=parent->mShadowChildren[i];
			if(parent->mActivateChildren){
				child->activate();
			}
			if(child->active()){
				logicUpdate(child,dt,scope);
				childrenActive=true;
			}

			merge(parent->mWorldBound,child->mWorldBound);
		}

		parent->mActivateChildren=false;
	}
	else{
		mul(node->mWorldBound,node->mWorldTranslate,node->mLocalBound);
	}
	
	if(node->mDeactivateCount>=0){
		if(childrenActive==false){
			node->mDeactivateCount++;
			if(node->mDeactivateCount>4){
				node->mActive=false;
				node->mDeactivateCount=0;
			}
		}
		else{
			node->mDeactivateCount=0;
		}
	}
}

void SceneNode::postLogicUpdateLoop(int dt){
}

void SceneNode::intraUpdate(int dt){
}

void SceneNode::preRenderUpdate(int dt){
}

void SceneNode::renderUpdate(int dt){
	renderUpdate(dt,-1);
}

void SceneNode::renderUpdate(int dt,int scope){
	mRenderFrame++;

	if(mBackground->getNumChildren()>0){
		renderUpdate(mBackground,dt,scope);
	}
	renderUpdate(this,dt,scope);
}

void SceneNode::postRenderUpdate(int dt){
}

void SceneNode::renderUpdate(Node::ptr node,int dt,int scope){
	if((node->mScope&scope)==0){
		return;
	}

	if(node->mReceiveUpdates){
		node->renderUpdate(dt);
	}

	if(node->mParent==NULL){
		node->mWorldRenderTransform.set(node->mRenderTransform);
	}
	else if(node->mIdentityTransform){
		node->mWorldRenderTransform.set(node->mParent->mWorldRenderTransform);
	}
	else{
		Math::mul(node->mWorldRenderTransform,node->mParent->mWorldRenderTransform,node->mRenderTransform);
	}

	ParentNode *parent=node->isParent();
	if(parent!=NULL){
		if(parent->mShadowChildrenDirty){
			parent->updateShadowChildren();
		}

		mul(node->mRenderWorldBound,node->mWorldRenderTransform,node->mLocalBound);

		Node *child;
		int numChildren=parent->mShadowChildren.size();
		int i;
		for(i=0;i<numChildren;++i){
			child=parent->mShadowChildren[i];
			if(child->active()){
				renderUpdate(child,dt,scope);
			}

			merge(parent->mRenderWorldBound,child->mRenderWorldBound);
		}
	}
	else{
		mul(node->mRenderWorldBound,node->mWorldRenderTransform,node->mLocalBound);
	}
}

void SceneNode::render(Renderer *renderer,CameraNode *camera,Node *node){
	#if defined(TOADLET_DEBUG)
		if(!created()){
			Error::unknown(Categories::TOADLET_TADPOLE,
				"render called on a desroyed Scene");
		}
	#endif

	if(node==NULL){
		node=this;
	}

	int i,j;

	mCamera=camera;

	mCamera->updateFramesPerSecond();
	mCamera->updateViewTransform();

	if(mCamera->getViewportSet()){
		renderer->setViewport(mCamera->getViewport());
	}
	else{
		RenderTarget *renderTarget=renderer->getRenderTarget();
		renderer->setViewport(cache_render_viewport.set(0,0,renderTarget->getWidth(),renderTarget->getHeight()));
	}

	int clearFlags=mCamera->getClearFlags();
	if(clearFlags>0 && !mCamera->getSkipFirstClear()){
		renderer->setDepthWrite(true);
		renderer->clear(clearFlags,mCamera->getClearColor());
	}

	renderer->setDefaultStates();
	renderer->setProjectionMatrix(mCamera->mProjectionTransform);
	renderer->setViewMatrix(mCamera->mViewTransform);
	renderer->setModelMatrix(Math::IDENTITY_MATRIX4X4);
	renderer->setAmbientColor(mAmbientColor);

	mCamera->mNumCulledEntities=0;

	mLight=NULL;

	// Only render background when rendering from the scene
	if(node==this && mBackground->getNumChildren()>0){
		mBackground->setTranslate(mCamera->getWorldRenderTranslate());
		renderUpdate(mBackground,0,mCamera->getScope());
		queueRenderables(mBackground);
	}
	queueRenderables();

	if(mLight!=NULL){
		renderer->setLight(0,mLight->internal_getLight());
		renderer->setLightEnabled(0,true);
		mLight=NULL;
	}
	else{
		renderer->setLightEnabled(0,false);
	}

	bool renderedLayer=false;
	for(i=0;i<mRenderLayers.size();++i){
		int layerNum=i+Material::MIN_LAYER;
		RenderLayer *layer=mRenderLayers[i];
		if(layer==NULL || (layer->renderables.size()==0 && layer->forceRender==false)){
			continue;
		}

		if(renderedLayer==true){
			if(layer->clearLayer && (clearFlags&Renderer::ClearFlag_DEPTH)>0){
				renderer->clear(Renderer::ClearFlag_DEPTH,Colors::BLACK);
			}
		}
		else{
			renderedLayer=true;
		}

		preLayerRender(renderer,layerNum);

		int numRenderables=layer->renderables.size();
		for(j=0;j<numRenderables;++j){
			Renderable *renderable=layer->renderables[j];
			Material *material=renderable->getRenderMaterial();
			if(material!=NULL && mPreviousMaterial!=material){
				material->setupRenderer(renderer,mPreviousMaterial);
			}
			mPreviousMaterial=material;
			renderer->setModelMatrix(renderable->getRenderTransform());
			renderable->render(renderer);
		}
		layer->renderables.clear();

		postLayerRender(renderer,layerNum);
	}

	mPreviousMaterial=NULL;
	mCamera=NULL;
}

void SceneNode::queueRenderable(Renderable *renderable){
	Material *material=renderable->getRenderMaterial();
	int layer=(material==NULL)?0:material->getLayer();
	getRenderLayer(layer)->renderables.add(renderable);
}

void SceneNode::queueLight(LightNode *light){
	// TODO: Find best light
	mLight=light;
}

void SceneNode::setUpdateListener(UpdateListener *updateListener){
	mUpdateListener=updateListener;
}

bool SceneNode::performAABoxQuery(SpacialQuery *query,const AABox &box,bool exact){
	SpacialQueryResultsListener *listener=query->getSpacialQueryResultsListener();

	// TODO: Child Nodes should have proper bounding shapes (either a generic Volume which can be Sphere, etc, or just an actual Sphere)
	int i;
	for(i=0;i<mChildren.size();++i){
		Node *child=mChildren[i];
		if(Math::testIntersection(child->mWorldBound,box)){
			listener->resultFound(child);
		}
	}
	return true;
}

void SceneNode::queueRenderables(){
	queueRenderables(this);
}

void SceneNode::queueRenderables(Node *node){
	if((node->mScope&mCamera->mScope)==0){
		return;
	}

	if(culled(node)){
		mCamera->mNumCulledEntities++;
		return;
	}

	if(node->mCameraAligned){
		if(true){//mCamera->mAlignmentCalculationsUseOrigin){
			Vector3 nodeWorldTranslate; Math::setTranslateFromMatrix4x4(nodeWorldTranslate,node->mWorldRenderTransform);
			Vector3 cameraWorldTranslate; Math::setTranslateFromMatrix4x4(cameraWorldTranslate,mCamera->mWorldRenderTransform);
			Matrix4x4 lookAtCamera; Math::setMatrix4x4FromLookAt(lookAtCamera,cameraWorldTranslate,nodeWorldTranslate,Math::Z_UNIT_VECTOR3,false);
			Matrix3x3 rotate; Math::setMatrix3x3FromMatrix4x4Transpose(rotate,lookAtCamera);

			// TODO: Switch this to RenderWorldScale when its available
			Vector3 worldScale; Math::setScaleFromMatrix4x4(worldScale,node->mWorldRenderTransform);
			Math::setMatrix4x4FromRotateScale(node->mWorldRenderTransform,rotate,worldScale);
		}
		else{
			Matrix3x3 rotate; Math::setMatrix3x3FromMatrix4x4Transpose(rotate,mCamera->getViewTransform());

			// TODO: Switch this to RenderWorldScale when its available
			Vector3 worldScale; Math::setScaleFromMatrix4x4(worldScale,node->mWorldRenderTransform);
			Math::setMatrix4x4FromRotateScale(node->mWorldRenderTransform,rotate,worldScale);
		}
	}

	ParentNode *parent=node->isParent();
	if(parent!=NULL){
		int numChildren=parent->mChildren.size();
		int i;
		for(i=0;i<numChildren;++i){
			queueRenderables(parent->mChildren[i]);
		}
	}
	else{
		Renderable *renderable=node->isRenderable();
		if(renderable!=NULL){
			renderable->queueRenderable(this,mCamera);
		}
	}
}

bool SceneNode::culled(Node *node){
	if(node->mRenderWorldBound.radius<0){
		return false;
	}

	return mCamera->culled(node->mRenderWorldBound);
}

bool SceneNode::preLayerRender(Renderer *renderer,int layer){
	return false;
}

bool SceneNode::postLayerRender(Renderer *renderer,int layer){
	return false;
}

}
}
}
