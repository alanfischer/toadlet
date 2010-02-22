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

#include <toadlet/egg/Logger.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Profile.h>
#include <toadlet/peeper/CapabilitySet.h> 
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/node/SceneNode.h>
#include <toadlet/tadpole/node/PhysicallyTraceable.h>
#include <toadlet/tadpole/plugins/hop/HopScene.h>
#include <toadlet/tadpole/plugins/hop/HopEntity.h>
#include <toadlet/tadpole/plugins/hop/HopCollision.h>
#include <toadlet/tadpole/plugins/hop/HopParticleSimulator.h>

using namespace toadlet::egg;
using namespace toadlet::hop;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::node;

namespace toadlet{
namespace tadpole{

HopScene::HopScene(Scene::ptr scene):
	mCounter(new PointerCounter(0)),
	//mScene,
	//mChildScene,
	mTraceable(NULL),

	//mHopEntities,
	mShowCollisionVolumes(false),
	mInterpolateCollisionVolumes(false),

	mExcessiveDT(0),
	mSimulator(NULL),
	
	//mFreeNetworkIDs,
	//mNetworkIDMap,
	mHopEntityFactory(NULL)

	//mSolidCollection
{
	mSimulator=new Simulator();
	mSimulator->setManager(this);
	mScene=scene;
	mScene->setChildScene(this);
	mTraceable=mScene->getRootNode()->isPhysicallyTraceable();

	resetNetworkIDs();

	mWorld=Solid::ptr(new Solid());
	mWorld->setLocalGravity(Math::ZERO_VECTOR3);
	mWorld->setInfiniteMass();
	mSimulator->addSolid(mWorld);
	
	mScene->getRootNode()->getEngine()->registerNodeType(HopEntity::type());
}

HopScene::~HopScene(){
	if(mSimulator!=NULL){
		delete mSimulator;
		mSimulator=NULL;
	}
}

void HopScene::findHopEntitiesInAABox(const AABox &box,Collection<HopEntity::ptr> &entities){
	if(mSolidCollection.size()<mSimulator->getNumSolids()){
		mSolidCollection.resize(mSimulator->getNumSolids());
	}
	int numSolids=mSimulator->findSolidsInAABox(box,mSolidCollection.begin(),mSolidCollection.size());
	findHopEntitiesInSolids(entities,mSolidCollection.begin(),numSolids);
}

void HopScene::findHopEntitiesInSphere(const Sphere &sphere,Collection<HopEntity::ptr> &entities){
	if(mSolidCollection.size()<mSimulator->getNumSolids()){
		mSolidCollection.resize(mSimulator->getNumSolids());
	}
	int numSolids=mSimulator->findSolidsInSphere(sphere,mSolidCollection.begin(),mSolidCollection.size());
	findHopEntitiesInSolids(entities,mSolidCollection.begin(),numSolids);
}

void HopScene::findHopEntitiesInSolids(Collection<HopEntity::ptr> &entities,Solid *solids[],int numSolids){
	entities.clear();
	int i;
	for(i=numSolids-1;i>=0;--i){
		Solid *solid=solids[i];
		HopEntity *entity=static_cast<HopEntity*>(solid->getUserData());
		if(entity!=NULL){
			entities.add(entity);
		}
	}
}

void HopScene::traceSegment(HopCollision &result,const Segment &segment,int collideWithBits,HopEntity *ignore){
	result.reset();

	Solid *ignoreSolid=NULL;
	if(ignore!=NULL){
		ignoreSolid=ignore->getSolid();
	}

	Collision &collision=cache_traceSegment_collision.reset();
	mSimulator->traceSegment(collision,segment,collideWithBits,ignoreSolid);

	if(collision.time!=-Math::ONE){
		result.time=collision.time;
		result.point=collision.point;
		result.normal=collision.normal;

		if(collision.collider!=NULL){
			HopEntity *entity=static_cast<HopEntity*>(collision.collider->getUserData());
			if(entity!=NULL){
				result.collider=HopEntity::ptr(entity);
			}
		}
	}
}

HopEntity *HopScene::getHopEntityFromNetworkID(int id) const{
	if(id>=0 && id<mNetworkIDMap.size()){
		return mNetworkIDMap[id];
	}
	else{
		return NULL;
	}
}

void HopScene::setHopEntityNetworkID(HopEntity *entity,int id){
	if(entity->getNetworkID()>=0 && entity->getNetworkID()<mNetworkIDMap.size()){
		mFreeNetworkIDs.add(entity->mNetworkID);
		mNetworkIDMap[entity->mNetworkID]=NULL;

//		Logger::alert(Categories::TOADLET_TADPOLE,String("Freeing NetworkID:")+entity->mNetworkID);
	}
	if(id>=0){
		if(id>=mNetworkIDMap.size()){
			mNetworkIDMap.resize(id+1);
		}
		mNetworkIDMap[id]=entity;
	}

	entity->mNetworkID=id;
//	Logger::alert(Categories::TOADLET_TADPOLE,String("Assigning NetworkID:")+entity->mNetworkID);
}

void HopScene::resetNetworkIDs(){
	mNetworkIDMap.clear();
	mNetworkIDMap.add(NULL);
	mFreeNetworkIDs.clear();
}

void HopScene::setHopEntityFactory(HopEntityFactory *factory){
	mHopEntityFactory=factory;
}

void HopScene::showCollisionVolumes(bool show,bool interpolate){
	mShowCollisionVolumes=show;
	mInterpolateCollisionVolumes=interpolate;

	int i;
	for(i=0;i<mHopEntities.size();++i){
		mHopEntities[i]->showCollisionVolumes(mShowCollisionVolumes);
	}
}

//ParticleNode::ParticleSimulator::ptr HopScene::newParticleSimulator(ParticleNode *particleNode){
//	return ParticleNode::ParticleSimulator::ptr(new HopParticleSimulator(this,particleNode));
//}

void HopScene::registerHopEntity(HopEntity *entity){
	if(mHopEntities.contains(entity)==false){
		mHopEntities.add(entity);

		if(mShowCollisionVolumes){
			entity->showCollisionVolumes(true);
		}

		if(entity->getNetworkID()!=HopEntity::NETWORKID_NOT_NETWORKED){
			defaultRegisterHopEntity(entity);
		}
	}
}

void HopScene::unregisterHopEntity(HopEntity *entity){
	if(mHopEntities.contains(entity)){
		defaultUnregisterHopEntity(entity);

		if(mShowCollisionVolumes){
			entity->showCollisionVolumes(false);
		}

		mHopEntities.remove(entity);
	}
}

void HopScene::preLogicUpdateLoop(int dt){
	int i;
	for(i=mHopEntities.size()-1;i>=0;--i){
		HopEntity *entity=mHopEntities[i];
		entity->preLogicUpdateLoop(dt);
	}

	mScene->preLogicUpdateLoop(dt);
}

void HopScene::preLogicUpdate(int dt){
	mScene->preLogicUpdate(dt);
}

void HopScene::logicUpdate(int dt){
	mScene->logicUpdate(dt);

	TOADLET_PROFILE_BEGINSECTION(Simulator::update);

	mSimulator->update(dt);

	TOADLET_PROFILE_ENDSECTION(Simulator::update);
}

void HopScene::postLogicUpdate(int dt){
	mScene->postLogicUpdate(dt);

	int i;
	for(i=mHopEntities.size()-1;i>=0;--i){
		HopEntity *entity=mHopEntities[i];
		entity->postLogicUpdate(dt);
	}
}

void HopScene::postLogicUpdateLoop(int dt){
	mScene->postLogicUpdateLoop(dt);
}

void HopScene::intraUpdate(int dt){
	mScene->intraUpdate(dt);
}

void HopScene::preRenderUpdate(int dt){
	int i;
	bool active,activePrevious;

	scalar f=mScene->getLogicFraction();

	for(i=mHopEntities.size()-1;i>=0;--i){
		HopEntity *entity=mHopEntities[i];
		active=entity->mSolid->getActive();
		activePrevious=entity->mActivePrevious;
		if(active || activePrevious){
			// TODO: Add an option to either use strict interpolation, or fuzzy interpolation
			// If we are deactivating, then make sure we are at our rest point
#if 0
			if(active==false && activePrevious){
				entity->interpolatePhysicalParameters(Math::ONE);
			}
			else{
				entity->interpolatePhysicalParameters(f);
			}
#elif 1
			Vector3 last;Math::setTranslateFromMatrix4x4(last,entity->getRenderTransform());
			Vector3 translate;Math::lerp(translate,last,entity->getTranslate(),0.3);
			entity->setRenderTransformTranslate(translate);
#else
			entity->setRenderTransformTranslate(entity->getTranslate());
#endif

			if(entity->mShadowMesh!=NULL){
				entity->castShadow();
			}
			if(entity->mVolumeNode!=NULL){
				entity->updateVolumes(mInterpolateCollisionVolumes);
			}
		}
	}

	mScene->preRenderUpdate(dt);
}

void HopScene::renderUpdate(int dt){
	mScene->renderUpdate(dt);
}

void HopScene::postRenderUpdate(int dt){
	mScene->postRenderUpdate(dt);
}

void HopScene::traceSegment(Collision &result,const Segment &segment){
	if(mTraceable!=NULL){
		result.time=mTraceable->traceSegment(result.normal,segment);
		if(result.time<0){
			Math::add(result.point,segment.origin,segment.direction);
		}
		else{
			Math::madd(result.point,segment.direction,result.time,segment.origin);
			result.collider=mWorld;
		}
	}
}

void HopScene::traceSolid(hop::Collision &result,const Segment &segment,const hop::Solid *solid){
	if(mTraceable!=NULL){
		if(solid->getShape(0)->getType()==hop::Shape::Type_AABOX){
			result.time=mTraceable->traceAABox(result.normal,segment,solid->getShape(0)->getAABox());
		}
		else if(solid->getShape(0)->getType()==hop::Shape::Type_SPHERE){
			result.time=mTraceable->traceSphere(result.normal,segment,solid->getShape(0)->getSphere());
		}
		if(result.time<0){
			Math::add(result.point,segment.origin,segment.direction);
		}
		else{
			Math::madd(result.point,segment.direction,result.time,segment.origin);
			result.collider=mWorld;
		}
	}
}

void HopScene::defaultRegisterHopEntity(HopEntity *entity){
	int networkID=0;
	if(mFreeNetworkIDs.size()>0){
		networkID=mFreeNetworkIDs[0];
		mFreeNetworkIDs.removeAt(0);
	}
	else{
		networkID=mNetworkIDMap.size();
	}

	entity->mNetworkID=networkID;
	if(networkID>=mNetworkIDMap.size()){
		mNetworkIDMap.resize(networkID+1);
	}
	mNetworkIDMap[networkID]=entity;
}

void HopScene::defaultUnregisterHopEntity(HopEntity *entity){
	if(entity->getNetworkID()>0){
		mFreeNetworkIDs.add(entity->getNetworkID());
		mNetworkIDMap[entity->getNetworkID()]=NULL;
	}
}

}
}
