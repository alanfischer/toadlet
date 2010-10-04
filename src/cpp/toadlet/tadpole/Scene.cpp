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
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/node/CameraNode.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;
using namespace toadlet::peeper;
using namespace toadlet::ribbit;
using namespace toadlet::tadpole::node;

namespace toadlet{
namespace tadpole{

Scene::Scene(Engine *engine):
	mUpdateListener(NULL),
	mExcessiveDT(0),
	mMinLogicDT(0),
	mMaxLogicDT(0),
	mLogicTime(0),
	mLogicFrame(0),
	mAccumulatedDT(0),
	mFrame(0),

	//mBackground,
	//mRoot,
	//mAmbientColor,
	//mBoundMesh,

	//mRenderQueue,
	mPreviousMaterial(NULL),
	mCountLastRendered(0)
{
	mEngine=engine;

	setExcessiveDT(5000);
	setRangeLogicDT(0,0);
	setAmbientColor(Colors::GREY);

	mHandles.resize(1); // Handle 0 is always NULL
	mBackground=mEngine->createNodeType(PartitionNode::type(),this);
	mRoot=mEngine->createNodeType(PartitionNode::type(),this);

	mRenderQueue=RenderQueue::ptr(new RenderQueue());

	mBoundMesh=mEngine->getMeshManager()->createSphere(Sphere(Math::ONE),8,8);
}

Scene::~Scene(){
	destroy();
}

void Scene::destroy(){
	if(mBackground!=NULL){
		mBackground->destroy();
		mBackground=NULL;
	}
	
	if(mRoot!=NULL){
		mRoot->destroy();
		mRoot=NULL;
	}
}

void Scene::setRoot(PartitionNode *root){
	if(mRoot!=NULL){
		mRoot->destroy();
	}

	mRoot=root;
}

Node *Scene::getNodeByHandle(int handle){
	if(handle>=0 && handle<mHandles.size()){
		return mHandles[handle];
	}
	else{
		return NULL;
	}
}

Node *Scene::findNodeByName(const String &name,Node *node){
	if(node==NULL){
		node=mRoot;
	}

	if(name.equals(node->getName())){
		return node;
	}
	else{
		ParentNode *parent=node->isParent();
		if(parent!=NULL){
			int i;
			for(i=0;i<parent->getNumChildren();++i){
				Node *found=findNodeByName(name,parent->getChild(i));
				if(found!=NULL){
					return found;
				}
			}
		}

		return NULL;
	}
}

void Scene::setRangeLogicDT(int minDT,int maxDT){
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

void Scene::setLogicTimeAndFrame(int time,int frame){
	mLogicTime=time;
	mLogicFrame=frame;
	mAccumulatedDT=0;
	mFrame=0;
}

void Scene::update(int dt){
	if(mExcessiveDT>0 && dt>mExcessiveDT){
		Logger::alert(Categories::TOADLET_TADPOLE,
			String("skipping excessive dt:")+dt);
		return;
	}

	mAccumulatedDT+=dt;

	if(mAccumulatedDT>=mMinLogicDT){
		if(mMaxLogicDT>0){
			bool firstRun=true;
			while(mAccumulatedDT>0 && mAccumulatedDT>=mMinLogicDT){
				if(firstRun==false){
					if(mUpdateListener!=NULL){
						mUpdateListener->preFrameUpdate(0);
						mUpdateListener->frameUpdate(0);
						mUpdateListener->postFrameUpdate(0);
					}
					else{
						preFrameUpdate(0);
						frameUpdate(0);
						postFrameUpdate(0);
					}
				}

				int logicDT=mAccumulatedDT;
				if(mAccumulatedDT>mMaxLogicDT){
					mAccumulatedDT-=mMaxLogicDT;
					logicDT=mMaxLogicDT;
				}
				else{
					mAccumulatedDT=0;
				}

				mLogicTime+=logicDT;
				mLogicFrame++;

				if(mUpdateListener!=NULL){
					mUpdateListener->preLogicUpdate(logicDT);
					mUpdateListener->logicUpdate(logicDT);
					mUpdateListener->postLogicUpdate(logicDT);
				}
				else{
					preLogicUpdate(logicDT);
					logicUpdate(logicDT);
					postLogicUpdate(logicDT);
				}
				
				firstRun=false;
			}
		}
		else{
			mAccumulatedDT=0;

			mLogicTime+=dt;
			mLogicFrame++;

			if(mUpdateListener!=NULL){
				mUpdateListener->preLogicUpdate(dt);
				mUpdateListener->logicUpdate(dt);
				mUpdateListener->postLogicUpdate(dt);
			}
			else{
				preLogicUpdate(dt);
				logicUpdate(dt);
				postLogicUpdate(dt);
			}
		}
	}

	mFrame++;

	if(mUpdateListener!=NULL){
		mUpdateListener->preFrameUpdate(dt);
		mUpdateListener->frameUpdate(dt);
		mUpdateListener->postFrameUpdate(dt);
	}
	else{
		preFrameUpdate(dt);
		frameUpdate(dt);
		postFrameUpdate(dt);
	}
}

void Scene::logicUpdate(int dt,int scope){
	mDependents.clear();

	mBackground->logicUpdate(dt,scope);
	mRoot->logicUpdate(dt,scope);

	/// @todo: dependents are currently broken if they're more than 1 deep.  This code doesnt check if the dependent is ready. That should be a function in node?
	/// @todo: Also, the world bound updating needs to go all the way to the root.
	while(mDependents.size()>0){
		Collection<Node::ptr> dependents=mDependents; mDependents.clear();
		for(int i=0;i<dependents.size();++i){
			Node *dependent=dependents[i];
			dependent->logicUpdate(dt,scope);
			dependent->getParent()->mergeWorldBound(dependents[i],false);
		}
		/// @todo: This isnt the best check, since we could have an equal amount of dependents added and removed in a frame, and thats not circular
		if(dependents.size()==mDependents.size()){
			Error::unknown("Circular dependencies detected!");
			return;
		}
	}
}

void Scene::frameUpdate(int dt,int scope){
	mDependents.clear();

	mBackground->frameUpdate(dt,scope);
	mRoot->frameUpdate(dt,scope);

	while(mDependents.size()>0){
		Collection<Node::ptr> dependents=mDependents; mDependents.clear();
		for(int i=0;i<dependents.size();++i){
			Node *dependent=dependents[i];
			dependent->frameUpdate(dt,scope);
			dependent->getParent()->mergeWorldBound(dependents[i],false);
		}
		/// @todo: This isnt the best check, since we could have an equal amount of dependents added and removed in a frame, and thats not circular
		if(dependents.size()==mDependents.size()){
			Error::unknown("Circular dependencies detected!");
			return;
		}
	}
}

void Scene::queueDependent(Node *dependent){
	mDependents.add(dependent);
}

void Scene::render(Renderer *renderer,CameraNode *camera,Node *node){
	camera->updateFramesPerSecond();

	// Reposition our background node & update it to update the world positions
	mBackground->setTranslate(camera->getWorldTranslate());
	mBackground->frameUpdate(0,-1);

	mRenderQueue->setCamera(camera);
	if(node!=NULL){
		node->queueRenderables(camera,mRenderQueue);
	}
	else{
		mBackground->queueRenderables(camera,mRenderQueue);
		mRoot->queueRenderables(camera,mRenderQueue);
	}

	renderRenderables(renderer,camera,mRenderQueue);
}

void Scene::renderRenderables(Renderer *renderer,CameraNode *camera,RenderQueue *queue){
	mCountLastRendered=0;

	if(camera->getViewportSet()){
		renderer->setViewport(camera->getViewport());
	}
	else{
		RenderTarget *renderTarget=renderer->getRenderTarget();
		renderer->setViewport(cache_render_viewport.set(0,0,renderTarget->getWidth(),renderTarget->getHeight()));
	}

	int clearFlags=camera->getClearFlags();
	if(clearFlags>0 && !camera->getSkipFirstClear()){
		renderer->setDepthWrite(true);
		renderer->clear(clearFlags,camera->getClearColor());
	}

	renderer->setDefaultStates();
	renderer->setProjectionMatrix(camera->getProjectionTransform());
	renderer->setViewMatrix(camera->getViewTransform());
	renderer->setModelMatrix(Math::IDENTITY_MATRIX4X4);
	renderer->setAmbientColor(mAmbientColor);

	if(queue->getLight()!=NULL){
		renderer->setLight(0,queue->getLight()->internal_getLight());
		renderer->setLightEnabled(0,true);
	}
	else{
		renderer->setLightEnabled(0,false);
	}

	bool renderedLayer=false;
	const egg::Collection<RenderQueue::RenderLayer*> &layers=queue->getRenderLayers();
	int i,j,k;
	for(i=0;i<layers.size();++i){
		int layerNum=i+Material::MIN_LAYER;
		RenderQueue::RenderLayer *layer=layers[i];
		if(layer==NULL || (layer->depthSortedRenderables.size()==0 && layer->materialSortedRenderables.size()==0 && layer->forceRender==false)){
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

		preLayerRender(renderer,camera,layerNum);

		int numRenderables=layer->materialSortedRenderables.size();
		for(j=0;j<numRenderables;++j){
			Material *material=layer->materialSortedRenderables[j].material;
			if(material!=NULL && mPreviousMaterial!=material){
				material->setupRenderer(renderer,mPreviousMaterial);
			}
			mPreviousMaterial=material;

			int numRenderables2=layer->materialSortedRenderables[j].renderables.size();
			for(k=0;k<numRenderables2;++k){
				Renderable *renderable=layer->materialSortedRenderables[j].renderables[k];
				renderer->setModelMatrix(renderable->getRenderTransform());
				material->setupRenderer(renderer,mPreviousMaterial);
				renderable->render(renderer);
				mCountLastRendered++;
			}
		}
		layer->materialSortedRenderables.clear();
		numRenderables=layer->depthSortedRenderables.size();
		for(j=0;j<numRenderables;++j){
			Renderable *renderable=layer->depthSortedRenderables[j].renderable;
			Material *material=renderable->getRenderMaterial();
			if(material!=NULL && mPreviousMaterial!=material){
				material->setupRenderer(renderer,mPreviousMaterial);
			}
			mPreviousMaterial=material;
			renderer->setModelMatrix(renderable->getRenderTransform());
			renderable->render(renderer);
			mCountLastRendered++;
		}
		layer->depthSortedRenderables.clear();

		postLayerRender(renderer,camera,layerNum);

		// Reset previous material each time, to avoid pre/postLayerRender messing up what we though the state of things were
		// We could also use the true/false return of pre/postLayerRender, but it could be easy to forget to change that.
		mPreviousMaterial=NULL;
	}

//Profile::getInstance()->collectionAllocations=0;
}

Image::ptr Scene::renderToImage(Renderer *renderer,CameraNode *camera,int format,int width,int height){
	Texture::ptr renderTexture=mEngine->getTextureManager()->createTexture(Texture::Usage_BIT_RENDERTARGET,Texture::Dimension_D2,format,width,height,0,1);
	SurfaceRenderTarget::ptr renderTarget=mEngine->getTextureManager()->createSurfaceRenderTarget();
	renderTarget->attach(renderTexture->getMipSurface(0,0),SurfaceRenderTarget::Attachment_COLOR_0);

	RenderTarget *oldTarget=renderer->getRenderTarget();
	renderer->setRenderTarget(renderTarget);
	Viewport oldView=camera->getViewport();
	camera->setViewport(Viewport(0,0,width,height));
	renderer->beginScene();
		render(renderer,camera,NULL);
	renderer->endScene();
	renderer->setRenderTarget(oldTarget);
	camera->setViewport(oldView);

	Image::ptr image=mEngine->getTextureManager()->createImage(renderTexture);

	renderTarget->destroy();
	renderTexture->release();

	return image;
}

int Scene::countActiveNodes(Node *node){
	if(node==NULL){
		return countActiveNodes(mRoot);
	}
	
	int count=node->active()?1:0;

	ParentNode *parent=node->isParent();
	if(parent!=NULL){
		int numChildren=parent->getNumChildren();
		int i;
		for(i=0;i<numChildren;++i){
			count+=countActiveNodes(parent->getChild(i));
		}
	}
	
	return count;
}

int Scene::countLastRendered(){
	return mCountLastRendered;
}

void Scene::renderBoundingVolumes(Renderer *renderer,Node *node){
	if(node==NULL){
		mBoundMesh->subMeshes[0]->material->setupRenderer(renderer,NULL);
		renderBoundingVolumes(renderer,mRoot);
		return;
	}

	if(node->getWorldBound().getSphere().radius>0){
		Matrix4x4 transform;
		Math::setMatrix4x4FromTranslate(transform,node->getWorldBound().getSphere().origin);
		Math::setMatrix4x4FromScale(transform,node->getWorldBound().getSphere().radius,node->getWorldBound().getSphere().radius,node->getWorldBound().getSphere().radius);
		renderer->setModelMatrix(transform);
		renderer->renderPrimitive(mBoundMesh->staticVertexData,mBoundMesh->subMeshes[0]->indexData);
	}

	ParentNode *parent=node->isParent();
	if(parent!=NULL){
		int numChildren=parent->getNumChildren();
		int i;
		for(i=0;i<numChildren;++i){
			renderBoundingVolumes(renderer,parent->getChild(i));
		}
	}
}

int Scene::nodeCreated(Node *node){
	int handle=-1;
	int size=mFreeHandles.size();
	if(size>0){
		handle=mFreeHandles.at(size-1);
		mFreeHandles.removeAt(size-1);
	}
	else{
		handle=mHandles.size();
		mHandles.resize(handle+1);
	}

	mHandles[handle]=node;

	return handle;
}

void Scene::nodeDestroyed(Node *node){
	int handle=node->getUniqueHandle();
	if(handle>=0){
		mHandles[handle]=NULL;
		mFreeHandles.add(handle);
	}
}

}
}
