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
#include <toadlet/tadpole/Colors.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/SceneRenderer.h>
#include <toadlet/tadpole/node/CameraNode.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::ribbit;
using namespace toadlet::tadpole::node;

namespace toadlet{
namespace tadpole{

Scene::Scene(Engine *engine):
	mUpdateListener(NULL),
	mRenderListener(NULL),

	mExcessiveDT(0),
	mMinLogicDT(0),
	mMaxLogicDT(0),
	mLogicTime(0),
	mLogicFrame(0),
	mAccumulatedDT(0),
	mFrame(0)

	//mBackground,
	//mRoot,
	//mAmbientColor,
	//mFogState,

	//mSceneRenderer,

	//mBoundMesh
{
	mEngine=engine;

	setExcessiveDT(5000);
	setRangeLogicDT(0,0);
	setAmbientColor(Colors::GREY);

	mBackground=mEngine->createNodeType(PartitionNode::type(),this);
	mRoot=mEngine->createNodeType(PartitionNode::type(),this);

	mSceneRenderer=SceneRenderer::ptr(new SceneRenderer(this));

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
//	if(mExcessiveDT>0 && dt>mExcessiveDT){
//		Logger::alert(Categories::TOADLET_TADPOLE,
//			String("skipping excessive dt:")+dt);
//		return;
//	}

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
	if(mRenderListener!=NULL){
		mRenderListener->preRender(renderer,camera,node);
	}

	if(node==NULL){
		node=mRoot;
	}

	// Reposition our background node & update it to update the world positions
	mBackground->setTranslate(camera->getWorldTranslate());
	mBackground->frameUpdate(0,-1);

	mSceneRenderer->renderScene(renderer,node,camera);

	if(mRenderListener!=NULL){
		mRenderListener->postRender(renderer,camera,node);
	}
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

void Scene::renderBoundingVolumes(Renderer *renderer,Node *node){
	if(node==NULL){
		mBoundMesh->subMeshes[0]->material->setupRenderer(renderer,NULL);
		renderBoundingVolumes(renderer,mRoot);
		return;
	}

	if(node->getWorldBound().getSphere().radius>0){
		const Sphere &sphere=node->getWorldBound().getSphere();
		Matrix4x4 transform;
		Math::setMatrix4x4FromTranslate(transform,sphere.origin);
		Math::setMatrix4x4FromScale(transform,sphere.radius,sphere.radius,sphere.radius);
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

}
}
