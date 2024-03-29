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

#include <toadlet/tadpole/Colors.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Scene.h>

/// @todo: We need to differentiate between a Manager and a System.  The Engine can keep hold of all Managers, but the Systems are Scene specific.
#include <toadlet/tadpole/plugins/SimpleRenderManager.h>
#ifdef TOADLET_HAS_BULLET
#include <toadlet/tadpole/plugins/BulletManager.h>
#endif
#include <toadlet/tadpole/plugins/HopManager.h>

namespace toadlet{
namespace tadpole{

Scene::Scene(Engine *engine,int options,scalar epsilon):Object(),
	mUpdateListener(NULL),

	mExcessiveDT(0),
	mMinLogicDT(0),
	mMaxLogicDT(0),
	mLogicTime(0),
	mLogicFrame(0),
	mAccumulatedDT(0),
	mFrame(0),
	mResetFrame(false),
	mEpsilon(epsilon),

	//mBackground,
	//mRoot,
	//mAmbientColor,

	mCreateAudio(true)
{
	mEngine=engine;
	mEngine->addContextListener(this);

	setExcessiveDT(500);
	setRangeLogicDT(50,50);
	setAmbientColor(Colors::GREY);

	mNodeManager=new NodeManager(this);

	mRenderManager=new SimpleRenderManager(this);

#if defined(TOADLET_HAS_BULLET)
	if((options&Option_BIT_NOBULLET)==0){
		mPhysicsManager=new BulletManager(this);
	} else
#endif
	mPhysicsManager=new HopManager(this);

	mBackground=new PartitionNode(this);

	mRoot=new PartitionNode(this);
	mRoot->rootChanged(mRoot);
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
	mRoot->rootChanged(mRoot);
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
		Log::alert(Categories::TOADLET_TADPOLE,
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

void Scene::logicUpdate(int dt,int scope,Node *node){
	TOADLET_PROFILE_AUTOSCOPE();

	if(node!=NULL){
		if(mPhysicsManager!=NULL){
			mPhysicsManager->logicUpdate(dt,scope,node);
		}

		node->logicUpdate(dt,scope);
	}
	else{
		if(mPhysicsManager!=NULL){
			mPhysicsManager->logicUpdate(dt,scope,NULL);
		}

		if(mBackground!=NULL){
			mBackground->logicUpdate(dt,scope);
		}
		mRoot->logicUpdate(dt,scope);
	}

	int i;
	for(i=0;i<mDestroyComponents.size();++i){
		mDestroyComponents[i]->destroy();
	}
	mDestroyComponents.clear();
}

void Scene::frameUpdate(int dt,int scope,Node *node){
	if(node!=NULL){
		if(mPhysicsManager!=NULL){
			mPhysicsManager->frameUpdate(dt,scope,node);
		}

		node->frameUpdate(dt,scope);
	}
	else{
		if(mPhysicsManager!=NULL){
			mPhysicsManager->frameUpdate(dt,scope,NULL);
		}

		if(mBackground!=NULL){
			mBackground->frameUpdate(dt,scope);
		}
		mRoot->frameUpdate(dt,scope);
	}
}

void Scene::render(RenderDevice *device,Camera *camera,Node *node){
	if(node==NULL){
		node=mRoot;
	}

	if(mBackground!=NULL){
		// Reposition our background node & update it to update the world positions
		mBackground->setTranslate(camera->getPosition());
	}

	mRenderManager->renderScene(device,node,camera);
}

void Scene::traceSegment(PhysicsCollision &result,const Segment &segment,int collideWithBits,Node *ignore){
	result.time=Math::ONE;
	if(mPhysicsManager!=NULL){
		mPhysicsManager->traceSegment(result,segment,collideWithBits,ignore);
	}
}

void Scene::destroy(Component *component){
	if(stlit::find(mDestroyComponents.begin(),mDestroyComponents.end(),component)==mDestroyComponents.end()){
		mDestroyComponents.push_back(component);
	}
}

void Scene::postContextReset(RenderDevice *device){
	/// @todo: Should this be replaced with individual items registering for context notifications?
	mResetFrame=true;
	mRoot->activate();
	mRoot->frameUpdate(0,-1);
	mResetFrame=false;
}

}
}
