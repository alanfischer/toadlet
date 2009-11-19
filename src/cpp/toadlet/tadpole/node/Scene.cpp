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
	mVisualFrame(0),

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
	mVisualFrame=0;

	resetModifiedFrames(this);
}

void Scene::resetModifiedFrames(Node *node){
	node->mModifiedLogicFrame=-1;
	node->mModifiedVisualFrame=-1;
	node->mWorldModifiedLogicFrame=-1;
	node->mWorldModifiedVisualFrame=-1;

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
		Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_ALERT,String("skipping excessive dt:")+dt);
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
		mUpdateListener->visualUpdate(dt);
	}
	else{
		visualUpdate(dt);
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

void Scene::visualUpdate(int dt){
	mVisualFrame++;

	if(mBackground->getNumChildren()>0){
		visualUpdate(mBackground,dt);
	}
	visualUpdate(this,dt);
}

void Scene::visualUpdate(Node::ptr node,int dt){
	if(node->mReceiveUpdates){
		node->visualUpdate(dt);
	}

	if(node->mParent==NULL){
		node->mVisualWorldTransform.set(node->mVisualTransform);
	}
	else if(node->mIdentityTransform){
		node->mVisualWorldTransform.set(node->mParent->mVisualWorldTransform);
	}
	else{
		Math::mul(node->mVisualWorldTransform,node->mParent->mVisualWorldTransform,node->mVisualTransform);
	}

	node->mWorldModifiedVisualFrame=node->mModifiedVisualFrame;

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

			visualUpdate(child,dt);

			if(parent->mVisualWorldBound.radius>=0){
				if(child->mVisualWorldBound.radius>=0){
					scalar d=Math::length(parent->mVisualWorldBound.origin,child->mVisualWorldBound.origin) + child->mVisualWorldBound.radius;
					parent->mVisualWorldBound.radius=Math::maxVal(parent->mVisualWorldBound.radius,d);
				}
				else{
					parent->mVisualWorldBound.radius=-Math::ONE;
				}
			}

			parent->mWorldModifiedVisualFrame=parent->mWorldModifiedVisualFrame>child->mWorldModifiedVisualFrame?parent->mWorldModifiedVisualFrame:child->mWorldModifiedVisualFrame;
		}
	}
	else{
		Math::setVector3FromMatrix4x4(node->mVisualWorldBound.origin,node->mVisualWorldTransform);
		if(node->mIdentityTransform==false){
			scalar scale=Math::maxVal(node->getScale().x,Math::maxVal(node->getScale().y,node->getScale().z));
			node->mVisualWorldBound.radius=Math::mul(scale,node->mBoundingRadius);
		}
		else{
			node->mVisualWorldBound.radius=node->mBoundingRadius;
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
		mBackground->setTranslate(mCamera->getVisualWorldTranslate());
		visualUpdate(mBackground,0);
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
		mPreviousMaterial=NULL;

		postLayerRender(renderer,layerNum);
	}

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
	if(node->mVisualWorldBound.radius<0){
		return false;
	}

	return mCamera->culled(node->mVisualWorldBound);
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
