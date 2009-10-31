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
#include <toadlet/tadpole/entity/Scene.h>
#include <toadlet/tadpole/entity/CameraEntity.h>
#include <toadlet/tadpole/entity/RenderableEntity.h>
#include <toadlet/tadpole/Engine.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::ribbit;

namespace toadlet{
namespace tadpole{
namespace entity{

Scene::Scene():ParentEntity(),
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

Entity *Scene::create(Engine *engine){
	super::create(engine);

	if(mManaged){
		Error::unknown("Scene must be unmanaged until I clean it up");
		return this;
	}

	mBackground=(ParentEntity*)(new ParentEntity())->create(mEngine);

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

void Scene::resetModifiedFrames(Entity *entity){
	entity->mModifiedLogicFrame=-1;
	entity->mModifiedVisualFrame=-1;
	entity->mWorldModifiedLogicFrame=-1;
	entity->mWorldModifiedVisualFrame=-1;

	if(entity->isParent()){
		ParentEntity *parent=(ParentEntity*)entity;
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

	logicUpdate(this,dt);
	if(mBackground->getNumChildren()>0){
		logicUpdate(mBackground,dt);
	}
}

void Scene::logicUpdate(Entity::ptr entity,int dt){
	if(entity->mReceiveUpdates){
		entity->logicUpdate(dt);
	}

	entity->mWorldModifiedLogicFrame=entity->mModifiedLogicFrame;

	if(entity->isParent()){
		ParentEntity *parent=(ParentEntity*)entity.get();
		if(parent->mShadowChildrenDirty){
			parent->updateShadowChildren();
		}

		Entity *child;
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

	mLight=NULL;
	visualUpdate(this,dt);
	if(mBackground->getNumChildren()>0){
		visualUpdate(mBackground,dt);
	}
}

void Scene::visualUpdate(Entity::ptr entity,int dt){
	if(entity->mReceiveUpdates){
		entity->visualUpdate(dt);
	}

	if(entity->mParent==NULL){
		entity->mVisualWorldTransform.set(entity->mVisualTransform);
	}
	else if(entity->mIdentityTransform){
		entity->mVisualWorldTransform.set(entity->mParent->mVisualWorldTransform);
	}
	else{
		Math::mul(entity->mVisualWorldTransform,entity->mParent->mVisualWorldTransform,entity->mVisualTransform);
	}

	entity->mWorldModifiedVisualFrame=entity->mModifiedVisualFrame;

	if(entity->isParent()){
		ParentEntity *parent=(ParentEntity*)entity.get();
		if(parent->mShadowChildrenDirty){
			parent->updateShadowChildren();
		}

		Entity *child;
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
	else if(entity->isLight()){
		// TODO: Find the best light
		mLight=shared_static_cast<LightEntity>(entity);
	}
	else{
		Math::setVector3FromMatrix4x4(entity->mVisualWorldBound.origin,entity->mVisualWorldTransform);
		if(entity->mIdentityTransform==false){
			scalar scale=Math::maxVal(entity->getScale().x,Math::maxVal(entity->getScale().y,entity->getScale().z));
			entity->mVisualWorldBound.radius=Math::mul(scale,entity->mBoundingRadius);
		}
		else{
			entity->mVisualWorldBound.radius=entity->mBoundingRadius;
		}
	}
}

void Scene::render(Renderer *renderer,CameraEntity *camera){
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

	renderer->setProjectionMatrix(mCamera->mProjectionTransform);

	renderer->setViewMatrix(mCamera->mViewTransform);

	renderer->setModelMatrix(Math::IDENTITY_MATRIX4X4);

	// TODO: Gather lights in queueRenderables
	if(mLight!=NULL){
		renderer->setLight(0,mLight->internal_getLight());
		renderer->setLightEnabled(0,true);
	}
	else{
		renderer->setLightEnabled(0,false);
	}

	renderer->setAmbientColor(mAmbientColor);

	mCamera->mNumCulledEntities=0;

	if(mBackground->getNumChildren()>0){
		mBackground->setTranslate(mCamera->getVisualWorldTranslate());
		visualUpdate(mBackground,0);
		queueRenderables(mBackground);
	}
	queueRenderables(this);

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

		// This is necessary for some hardware accelerated devices to depth buffer clear properly
		renderer->setDepthWrite(true);
	}

	mCamera=NULL;
}

void Scene::queueRenderable(Renderable *renderable){
	Material *material=renderable->getRenderMaterial();
	int layer=(material==NULL)?0:material->getLayer();
	getRenderLayer(layer)->renderables.add(renderable);
}

void Scene::setUpdateListener(UpdateListener *updateListener){
	mUpdateListener=updateListener;
}

bool Scene::remove(Entity *entity){
	bool result=super::remove(entity);

	if(entity==mBackground){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"can not remove background entity");
		return false;
	}

	return result;
}

void Scene::queueRenderables(Entity *entity){
	if((entity->mScope&mCamera->mScope)==0){
		return;
	}

	if(culled(entity)){
		mCamera->mNumCulledEntities++;
		return;
	}

	if(entity->isParent()){
		ParentEntity *parent=(ParentEntity*)entity;
		int numChildren=parent->mChildren.size();
		int i;
		for(i=0;i<numChildren;++i){
			queueRenderables(parent->mChildren[i]);
		}
	}
	else if(entity->isRenderable()){
		RenderableEntity *renderable=(RenderableEntity*)entity;
		if(renderable->mVisible){
			renderable->queueRenderables(this);
		}
	}
}

bool Scene::culled(Entity *entity){
	if(entity->mVisualWorldBound.radius<0){
		return false;
	}

	return mCamera->culled(entity->mVisualWorldBound);
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
