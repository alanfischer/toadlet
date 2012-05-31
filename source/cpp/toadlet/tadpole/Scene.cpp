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

namespace toadlet{
namespace tadpole{

Scene::Scene(Engine *engine):Object(),
	mUpdateListener(NULL),
	mRenderListener(NULL),

	mExcessiveDT(0),
	mMinLogicDT(0),
	mMaxLogicDT(0),
	mLogicTime(0),
	mLogicFrame(0),
	mAccumulatedDT(0),
	mFrame(0),
	mResetFrame(false)

	//mBackground,
	//mRoot,
	//mAmbientColor,

	//mSceneRenderer,

	//mBoundMesh
{
Logger::alert("Scene create");
	mEngine=engine;
	mEngine->addContextListener(this);
Logger::alert("Scene set");
	setExcessiveDT(500);
	setRangeLogicDT(0,0);
	setAmbientColor(Colors::GREY);

Logger::alert("Scene node");
	mBackground=mEngine->createNodeType(PartitionNode::type(),this);
	mRoot=mEngine->createNodeType(PartitionNode::type(),this);

Logger::alert("Scene renderer");
	mSceneRenderer=SceneRenderer::ptr(new SceneRenderer(this));

Logger::alert("Scene meshes");
	mSphereMesh=mEngine->getMeshManager()->createSphereMesh(Sphere(Math::ONE));
Logger::alert("Scene sphere done");
	mAABoxMesh=mEngine->getMeshManager()->createAABoxMesh(AABox(-Math::ONE,-Math::ONE,-Math::ONE,Math::ONE,Math::ONE,Math::ONE));
Logger::alert("Scene done");
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

	if(mEngine!=NULL){
		mEngine->removeContextListener(this);
		mEngine=NULL;
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
	mBackground->logicUpdate(dt,scope);
	mRoot->logicUpdate(dt,scope);
}

void Scene::frameUpdate(int dt,int scope){
	mBackground->frameUpdate(dt,scope);
	mRoot->frameUpdate(dt,scope);
}

void Scene::render(RenderDevice *device,CameraNode *camera,Node *node){
	if(mRenderListener!=NULL){
		mRenderListener->preRender(device,camera,node);
	}

	if(node==NULL){
		node=mRoot;
	}

	// Reposition our background node & update it to update the world positions
	mBackground->setTranslate(camera->getWorldTranslate());
	mBackground->updateAllWorldTransforms();

	mSceneRenderer->renderScene(device,node,camera);

	if(mRenderListener!=NULL){
		mRenderListener->postRender(device,camera,node);
	}
}

int Scene::countActiveNodes(Node *node){
	if(node==NULL){
		return countActiveNodes(mRoot);
	}
	
	int count=node->getActive()?1:0;
	Node *child;
	for(child=node->getFirstChild();child!=NULL;child=child->getNext()){
		count+=countActiveNodes(child);
	}
	
	return count;
}
/// @todo
/*
void Scene::renderBoundingVolumes(RenderDevice *device,Node *node){
	if(node==NULL){
		mSphereMesh->getSubMesh(0)->material->setupRenderDevice(device);
		renderBoundingVolumes(device,mRoot);
		return;
	}

	Matrix4x4 transform;
	const Bound &bound=node->getWorldBound();
	switch(bound.getType()){
		case Bound::Type_SPHERE:{
			const Sphere &sphere=bound.getSphere();
			Math::setMatrix4x4FromTranslate(transform,sphere.origin);
			Math::setMatrix4x4FromScale(transform,sphere.radius,sphere.radius,sphere.radius);
			device->setMatrix(RenderDevice::MatrixType_MODEL,transform);
			device->renderPrimitive(mSphereMesh->getStaticVertexData(),mSphereMesh->getSubMesh(0)->indexData);
		}break;
		case Bound::Type_AABOX:{
			const AABox &box=bound.getAABox();
			Vector3 origin=(box.maxs - box.mins)/2 + box.mins;
			Math::setMatrix4x4FromTranslate(transform,origin);
			Math::setMatrix4x4FromScale(transform,origin.x-box.mins.x,origin.y-box.mins.y,origin.z-box.mins.z);
			device->setMatrix(RenderDevice::MatrixType_MODEL,transform);
			device->renderPrimitive(mAABoxMesh->getStaticVertexData(),mAABoxMesh->getSubMesh(0)->indexData);
		}break;
		default:
		break;
	}

	ParentNode *parent=node->isParent();
	if(parent!=NULL){
		for(node=parent->getFirstChild();node!=NULL;node=node->getNext()){
			renderBoundingVolumes(device,node);
		}
	}
}
*/
void Scene::postContextActivate(RenderDevice *device){
	mResetFrame=true;
	mRoot->activate();
	mRoot->frameUpdate(0,-1);
	mResetFrame=false;
}

}
}
