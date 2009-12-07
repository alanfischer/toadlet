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
#include <toadlet/tadpole/node/Scene.h>
#include <toadlet/tadpole/node/CameraNode.h>
#include <toadlet/tadpole/node/Renderable.h>
#include <toadlet/tadpole/Engine.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::ribbit;

namespace toadlet{
namespace tadpole{
namespace node{

Scene::Scene():super(),
	mExcessiveDT(5000),
	mLogicDT(100), // 10 fps
	mLogicTime(0),
	mLogicFrame(0),
	mAccumulatedDT(0),
	mRenderFrame(0),

	//mBackground,

	mUpdateListener(NULL),

	mAmbientColor(Math::HALF,Math::HALF,Math::HALF,Math::ONE),

	mCamera(NULL),
	mPreviousMaterial(NULL)
{}

Scene::~Scene(){
	int i;
	for(i=0;i<mRenderLayers.size();++i){
		if(mRenderLayers[i]!=NULL){
			delete mRenderLayers[i];
		}
	}
	mRenderLayers.clear();
}

Node *Scene::create(Engine *engine){
	super::create(engine);

	if(mManaged){
		Error::unknown("Scene must be unmanaged until I clean it up");
		return this;
	}

	mBackground=mEngine->createNodeType(ParentNode::type());

	return this;
}

void Scene::destroy(){
	if(mBackground!=NULL){
		mBackground->destroy();
		mBackground=NULL;
	}

	super::destroy();
}

void Scene::setAmbientColor(Color ambientColor){
	mAmbientColor=ambientColor;
}

void Scene::setLayerClearing(int layer,bool clear){
	getRenderLayer(layer)->clearLayer=clear;
}

void Scene::setLogicDT(int dt){
	#if defined(TOADLET_DEBUG)
		if(dt<=0){
			Error::unknown(Categories::TOADLET_TADPOLE,
				"setLogicDT must be > 0");
			return;
		}
	#endif

	mLogicDT=dt;
}

void Scene::setLogicTimeAndFrame(int time,int frame){
	mLogicTime=time;
	mLogicFrame=frame;
	mAccumulatedDT=0;
	mRenderFrame=0;

	resetModifiedFrames(this);
}

void Scene::resetModifiedFrames(Node *node){
	node->mModifiedLogicFrame=-1;
	node->mModifiedRenderFrame=-1;
	node->mWorldModifiedLogicFrame=-1;
	node->mWorldModifiedRenderFrame=-1;

	ParentNode *parent=node->isParent();
	if(parent!=NULL){
		int numChildren=parent->mChildren.size();
		int i;
		for(i=0;i<numChildren;++i){
			resetModifiedFrames(parent->mChildren[i]);
		}
	}
}

void Scene::update(int dt){
	#if defined(TOADLET_DEBUG)
		if(destroyed()){
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

	if(mAccumulatedDT>=mLogicDT){
		preLogicUpdateLoop(dt);

		while(mAccumulatedDT>=mLogicDT){
			mAccumulatedDT-=mLogicDT;

			if(mUpdateListener!=NULL){
				mUpdateListener->logicUpdate(mLogicDT);
			}
			else{
				logicUpdate(mLogicDT);
			}
		}

		postLogicUpdateLoop(dt);
	}

	if(mUpdateListener!=NULL){
		mUpdateListener->renderUpdate(dt);
	}
	else{
		renderUpdate(dt);
	}

	AudioPlayer *audioPlayer=mEngine->getAudioPlayer();
	if(audioPlayer!=NULL){
		audioPlayer->update(dt);
	}
}

void Scene::preLogicUpdateLoop(int dt){
}

void Scene::logicUpdate(int dt){
	mLogicTime+=dt;
	mLogicFrame++;

	if(mBackground->getNumChildren()>0){
		logicUpdate(mBackground,dt);
	}
	logicUpdate(this,dt);
}

void Scene::logicUpdate(Node::ptr node,int dt){
	if(node->mReceiveUpdates){
		node->logicUpdate(dt);
	}

	node->mWorldModifiedLogicFrame=node->mModifiedLogicFrame;

	ParentNode *parent=node->isParent();
	if(parent!=NULL){
		if(parent->mShadowChildrenDirty){
			parent->updateShadowChildren();
		}

		Node *child;
		int numChildren=parent->mShadowChildren.size();
		int i;
		for(i=0;i<numChildren;++i){
			child=parent->mShadowChildren[i];

			logicUpdate(child,dt);

			parent->mWorldModifiedLogicFrame=parent->mWorldModifiedLogicFrame>child->mWorldModifiedLogicFrame?parent->mWorldModifiedLogicFrame:child->mWorldModifiedLogicFrame;
		}
	}
}

void Scene::postLogicUpdateLoop(int dt){
}

void Scene::renderUpdate(int dt){
	mRenderFrame++;

	if(mBackground->getNumChildren()>0){
		renderUpdate(mBackground,dt);
	}
	renderUpdate(this,dt);
}

void Scene::renderUpdate(Node::ptr node,int dt){
	if(node->mReceiveUpdates){
		node->renderUpdate(dt);
	}

	if(node->mParent==NULL){
		node->mRenderWorldTransform.set(node->mRenderTransform);
	}
	else if(node->mIdentityTransform){
		node->mRenderWorldTransform.set(node->mParent->mRenderWorldTransform);
	}
	else{
		Math::mul(node->mRenderWorldTransform,node->mParent->mRenderWorldTransform,node->mRenderTransform);
	}

	node->mWorldModifiedRenderFrame=node->mModifiedRenderFrame;

	ParentNode *parent=node->isParent();
	if(parent!=NULL){
		if(parent->mShadowChildrenDirty){
			parent->updateShadowChildren();
		}

		Node *child;
		int numChildren=parent->mShadowChildren.size();
		int i;
		for(i=0;i<numChildren;++i){
			child=parent->mShadowChildren[i];

			renderUpdate(child,dt);

			if(parent->mRenderWorldBound.radius>=0){
				if(child->mRenderWorldBound.radius>=0){
					scalar d=Math::length(parent->mRenderWorldBound.origin,child->mRenderWorldBound.origin) + child->mRenderWorldBound.radius;
					parent->mRenderWorldBound.radius=Math::maxVal(parent->mRenderWorldBound.radius,d);
				}
				else{
					parent->mRenderWorldBound.radius=-Math::ONE;
				}
			}

			parent->mWorldModifiedRenderFrame=parent->mWorldModifiedRenderFrame>child->mWorldModifiedRenderFrame?parent->mWorldModifiedRenderFrame:child->mWorldModifiedRenderFrame;
		}
	}
	else{
		Math::setVector3FromMatrix4x4(node->mRenderWorldBound.origin,node->mRenderWorldTransform);
		if(node->mIdentityTransform==false){
			scalar scale=Math::maxVal(node->getScale().x,Math::maxVal(node->getScale().y,node->getScale().z));
			node->mRenderWorldBound.radius=Math::mul(scale,node->mBoundingRadius);
		}
		else{
			node->mRenderWorldBound.radius=node->mBoundingRadius;
		}
	}
}

void Scene::render(Renderer *renderer,CameraNode *camera,Node *node){
	#if defined(TOADLET_DEBUG)
		if(destroyed()){
			Error::unknown(Categories::TOADLET_TADPOLE,
				"render called on a destroyed Scene");
		}
	#endif

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
		mBackground->setTranslate(mCamera->getRenderWorldTranslate());
		renderUpdate(mBackground,0);
		queueRenderables(mBackground);
	}
	queueRenderables(node);

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
				mPreviousMaterial=material;
			}
			renderer->setModelMatrix(renderable->getRenderTransform());
			renderable->render(renderer);
		}
		layer->renderables.clear();

		postLayerRender(renderer,layerNum);
	}

	mPreviousMaterial=NULL;
	mCamera=NULL;
}

void Scene::queueRenderable(Renderable *renderable){
	Material *material=renderable->getRenderMaterial();
	int layer=(material==NULL)?0:material->getLayer();
	getRenderLayer(layer)->renderables.add(renderable);
}

void Scene::queueLight(LightNode *light){
	// TODO: FInd best light
	mLight=light;
}

void Scene::setUpdateListener(UpdateListener *updateListener){
	mUpdateListener=updateListener;
}

void Scene::queueRenderables(Node *node){
	if((node->mScope&mCamera->mScope)==0){
		return;
	}

	if(culled(node)){
		mCamera->mNumCulledEntities++;
		return;
	}

	// TODO: Fix these alignment calculations so it actually aligns per axis, and preserves parent scale
	if(node->mAlignXAxis || node->mAlignYAxis || node->mAlignZAxis){
		if(mCamera->mAlignmentCalculationsUseOrigin){
			Matrix4x4 &nodeWorldTransform=node->mRenderWorldTransform;
			Vector3 nodeWorldTranslate; Math::setVector3FromMatrix4x4(nodeWorldTranslate,node->mRenderWorldTransform);
			Vector3 cameraWorldTranslate; Math::setVector3FromMatrix4x4(cameraWorldTranslate,mCamera->mRenderWorldTransform);
			Matrix4x4 lookAtCamera; Math::setMatrix4x4FromLookAt(lookAtCamera,nodeWorldTranslate,cameraWorldTranslate,Math::Z_UNIT_VECTOR3,false);
			const Vector3 &nodeScale=node->mScale;
			nodeWorldTransform.setAt(0,0,Math::mul(lookAtCamera.at(0,0),nodeScale.x));
			nodeWorldTransform.setAt(1,0,lookAtCamera.at(0,1));
			nodeWorldTransform.setAt(2,0,lookAtCamera.at(0,2));
			nodeWorldTransform.setAt(0,1,lookAtCamera.at(1,0));
			nodeWorldTransform.setAt(1,1,Math::mul(lookAtCamera.at(1,1),nodeScale.y));
			nodeWorldTransform.setAt(2,1,lookAtCamera.at(1,2));
			nodeWorldTransform.setAt(0,2,lookAtCamera.at(2,0));
			nodeWorldTransform.setAt(1,2,lookAtCamera.at(2,1));
			nodeWorldTransform.setAt(2,2,Math::mul(lookAtCamera.at(2,2),nodeScale.z));
		}
		else{
			const Matrix4x4 &viewTransform=mCamera->getViewTransform();
			Matrix4x4 &nodeWorldTransform=node->mRenderWorldTransform;
			const Vector3 &nodeScale=node->mScale;
			nodeWorldTransform.setAt(0,0,Math::mul(viewTransform.at(0,0),nodeScale.x));
			nodeWorldTransform.setAt(1,0,viewTransform.at(0,1));
			nodeWorldTransform.setAt(2,0,viewTransform.at(0,2));
			nodeWorldTransform.setAt(0,1,viewTransform.at(1,0));
			nodeWorldTransform.setAt(1,1,Math::mul(viewTransform.at(1,1),nodeScale.y));
			nodeWorldTransform.setAt(2,1,viewTransform.at(1,2));
			nodeWorldTransform.setAt(0,2,viewTransform.at(2,0));
			nodeWorldTransform.setAt(1,2,viewTransform.at(2,1));
			nodeWorldTransform.setAt(2,2,Math::mul(viewTransform.at(2,2),nodeScale.z));
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
			renderable->queueRenderable(this);
		}
	}
}

bool Scene::culled(Node *node){
	if(node->mRenderWorldBound.radius<0){
		return false;
	}

	return mCamera->culled(node->mRenderWorldBound);
}

bool Scene::preLayerRender(Renderer *renderer,int layer){
	return false;
}

bool Scene::postLayerRender(Renderer *renderer,int layer){
	return false;
}

}
}
}
