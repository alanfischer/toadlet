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

#if 0

#include <toadlet/egg/Logger.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Profile.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Collision.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/plugins/hop/HopScene.h>
#include <toadlet/tadpole/plugins/hop/HopEntity.h>

namespace toadlet{
namespace tadpole{

HopManager::HopManager(Engine *engine):
	mSimulator(NULL),
	mTraceable(NULL)
{
	mSimulator=new Simulator();
	mSimulator->setManager(this);

	mSolid=Solid::ptr(new Solid());
	mSolid->setCoefficientOfGravity(0);
	mSolid->setInfiniteMass();

	mEngine->registerNodeType(HopEntity::type());

	mVolumeSensor=new BoundingVolumeSensor(this);
	mSensorResults=SolidSensorResults::ptr(new SolidSensorResults());
}

HopManager::~HopManager(){
}

void HopManager::traceSegment(Collision &result,const Segment &segment,int collideWithBits,Node *ignore){
	result.reset();

	Solid *ignoreSolid=NULL;
	if(ignore!=NULL && ignore->isEntity()!=NULL){
		ignoreSolid=((HopEntity*)ignore)->getSolid();
	}

	hop::Collision &collision=cache_traceSegment_collision.reset();
	mSimulator->traceSegment(collision,segment,collideWithBits,ignoreSolid);
	set(result,collision);
}

void HopManager::traceEntity(Collision &result,HopEntity *entity,const Segment &segment,int collideWithBits){
	result.reset();

	Solid *solid=NULL;
	if(entity!=NULL){
		solid=entity->getSolid();
	}

	hop::Collision &collision=cache_traceSegment_collision.reset();
	mSimulator->traceSolid(collision,solid,segment,collideWithBits);
	set(result,collision);
}

void HopManager::testEntity(Collision &result,HopEntity *entity1,const Segment &segment,HopEntity *entity2){
	result.reset();

	Solid *solid1=entity1->getSolid();
	Solid *solid2=entity2->getSolid();

	hop::Collision &collision=cache_traceSegment_collision.reset();
	mSimulator->testSolid(collision,solid1,segment,solid2);
	set(result,collision);
}

void HopManager::logicUpdate(int dt,int scope,HopEntity *entity){
	if(entity!=NULL){
		entity->preSimulate();
		TOADLET_PROFILE_BEGINSECTION(Simulator::update);
		mSimulator->update(dt,scope);
		TOADLET_PROFILE_ENDSECTION(Simulator::update);
		entity->postSimulate();

		entity->logicUpdate(dt,scope);
	}
	else{
		int i;
		for(i=mSimulator->getNumSolids()-1;i>=0;--i){
			Solid *solid=mSimulator->getSolid(i);
			HopEntity *entity=(HopEntity*)(solid->getUserData());
			entity->preSimulate();
		}

		TOADLET_PROFILE_BEGINSECTION(Simulator::update);
		mSimulator->update(dt,scope);
		TOADLET_PROFILE_ENDSECTION(Simulator::update);

		for(i=mSimulator->getNumSolids()-1;i>=0;--i){
			Solid *solid=mSimulator->getSolid(i);
			HopEntity *entity=(HopEntity*)(solid->getUserData());
			entity->postSimulate();
		}

		Scene::logicUpdate(dt,scope);
	}
}

int HopManager::findSolidsInAABox(const AABox &box,Solid *solids[],int maxSolids){
	mSensorResults->setSolids(solids,maxSolids);
	mVolumeSensor->setBox(box);
	mVolumeSensor->sense(mSensorResults);
	return mSensorResults->getCount();
}

void HopManager::traceSegment(hop::Collision &result,const Segment &segment,int collideWithBits){
	if(mTraceable!=NULL && (collideWithBits&mSolid->getCollisionBits())!=0){
		tadpole::Collision collision;
		mTraceable->traceSegment(collision,Math::ZERO_VECTOR3,segment,Math::ZERO_VECTOR3);
		set(result,collision,NULL,NULL);
		result.collider=mSolid;
	}
}

void HopManager::traceSolid(hop::Collision &result,hop::Solid *solid,const Segment &segment,int collideWithBits){
	// Only trace shapes that aren't a callback
	if(mTraceable!=NULL && (collideWithBits&mSolid->getCollisionBits())!=0 && (solid->getShapeTypes()&Shape::Type_CALLBACK)==0){
		tadpole::Collision collision;
		const AABox &bound=solid->getLocalBound();
		Vector3 size;
		Math::sub(size,bound.maxs,bound.mins);
		mTraceable->traceSegment(collision,Math::ZERO_VECTOR3,segment,size);
		set(result,collision,NULL,solid);
		result.collider=mSolid;
	}
}

void HopManager::set(tadpole::Collision &r,const hop::Collision &c){
	r.time=c.time;
	r.point.set(c.point);
	r.normal.set(c.normal);
	if(c.collider!=NULL){r.collider=(HopEntity*)c.collider->getUserData();};
	r.scope=c.scope;
}

void HopManager::set(hop::Collision &r,const tadpole::Collision &c,Solid *collider,Solid *collidee){
	r.time=c.time;
	r.point.set(c.point);
	r.normal.set(c.normal);
	// Since the c.collider passed in could be any Node, not necessarily a HopEntity, we force a passing in of a Collider
	if(collider!=NULL){r.collider=collider;}
	if(collidee!=NULL){r.collidee=collidee;}
	r.scope=c.scope;
}

bool HopManager::SolidSensorResults::resultFound(Node *result,scalar distance){
	HopEntity *entity=(HopEntity*)result->isEntity();
	if(entity!=NULL && mCounter<mMaxSolids && entity->getNumShapes()>0){
		mSolids[mCounter++]=entity->getSolid();
	}
	return mCounter<mMaxSolids;
}

}
}

#endif