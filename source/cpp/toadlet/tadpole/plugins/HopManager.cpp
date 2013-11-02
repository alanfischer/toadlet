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

#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Scene.h>
#include "HopManager.h"
#include "HopComponent.h"

namespace toadlet{
namespace tadpole{

HopManager::HopManager(Scene *scene):
	//mSimulator,
	mTraceable(NULL)
{
	mScene=scene;

	mSimulator=new hop::Simulator();
	mSimulator->setManager(this);
	mSimulator->setEpsilon(scene->getEpsilon());

	mSolid=new hop::Solid();
	mSolid->setCoefficientOfGravity(0);
	mSolid->setInfiniteMass();

	mVolumeSensor=new BoundingVolumeSensor(mScene);
	mSensorResults=SolidSensorResults::ptr(new SolidSensorResults());

	// Update the MicroCollisionThreshold
	setGravity(getGravity());
}

PhysicsComponent *HopManager::createPhysicsComponent(){
	return new HopComponent(this);
}

void HopManager::setGravity(const Vector3 &gravity){
	mSimulator->setGravity(gravity);

	// Estimate a good MicroCollisionThreshold for this gravity
	mSimulator->setMicroCollisionThreshold(Math::length(gravity)/8);
}

const Vector3 &HopManager::getGravity() const{
	return mSimulator->getGravity();
}

void HopManager::setTraceable(PhysicsTraceable *traceable){
	mTraceable=traceable;
}

void HopManager::setTraceableCollisionScope(int scope){
	mSolid->setCollisionScope(scope);
}

int HopManager::getTraceableCollisionScope() const{
	return mSolid->getCollisionScope();
}

void HopManager::traceSegment(PhysicsCollision &result,const Segment &segment,int collideWithScope,Node *ignore){
	result.reset();

	hop::Solid *ignoreSolid=NULL;
	if(ignore!=NULL && ignore->getPhysics()!=NULL){
		ignoreSolid=((HopComponent*)ignore->getPhysics())->getSolid();
	}

	hop::Collision collision;
	mSimulator->traceSegment(collision,segment,collideWithScope,ignoreSolid);
	set(result,collision);
}

void HopManager::traceNode(PhysicsCollision &result,Node *node,const Segment &segment,int collideWithScope){
	result.reset();

	hop::Solid *solid=NULL;
	if(node!=NULL && node->getPhysics()!=NULL){
		solid=((HopComponent*)node->getPhysics())->getSolid();
	}

	hop::Collision collision;
	mSimulator->traceSolid(collision,solid,segment,collideWithScope);
	set(result,collision);
}

void HopManager::testNode(PhysicsCollision &result,Node *node,const Segment &segment,Node *node2){
	result.reset();

	hop::Solid *solid1=NULL,*solid2=NULL;
	if(node!=NULL && node->getPhysics()!=NULL){
		solid1=((HopComponent*)node->getPhysics())->getSolid();
	}
	if(node2!=NULL && node2->getPhysics()!=NULL){
		solid2=((HopComponent*)node2->getPhysics())->getSolid();
	}

	hop::Collision collision;
	mSimulator->testSolid(collision,solid1,segment,solid2);
	set(result,collision);
}

void HopManager::logicUpdate(int dt,int scope,Node *node){
	HopComponent *component=node!=NULL?(HopComponent*)node->getPhysics():NULL;

	if(component!=NULL){
		component->preSimulate();
		TOADLET_PROFILE_BEGINSECTION(Simulator::update);
		mSimulator->update(dt,scope,component->getSolid());
		TOADLET_PROFILE_ENDSECTION(Simulator::update);
		component->postSimulate();
	}
	else{
		int i;
		for(i=mSimulator->getNumSolids()-1;i>=0;--i){
			hop::Solid *solid=mSimulator->getSolid(i);
			HopComponent *component=(HopComponent*)(solid->getUserData());
			component->preSimulate();
		}

		TOADLET_PROFILE_BEGINSECTION(Simulator::update);
		mSimulator->update(dt,scope);
		TOADLET_PROFILE_ENDSECTION(Simulator::update);

		for(i=mSimulator->getNumSolids()-1;i>=0;--i){
			hop::Solid *solid=mSimulator->getSolid(i);
			HopComponent *component=(HopComponent*)(solid->getUserData());
			component->postSimulate();
		}
	}
}

void HopManager::frameUpdate(int dt,int scope,Node *node){
	HopComponent *component=node!=NULL?(HopComponent*)node->getPhysics():NULL;

	if(component!=NULL){
		component->lerpPosition(mScene->getLogicFraction());
	}
	else{
		int i;
		for(i=mSimulator->getNumSolids()-1;i>=0;--i){
			hop::Solid *solid=mSimulator->getSolid(i);
			if(solid->active()){
				HopComponent *component=(HopComponent*)(solid->getUserData());
				component->lerpPosition(mScene->getLogicFraction());
			}
		}
	}
}

int HopManager::findSolidsInAABox(const AABox &box,hop::Solid *solids[],int maxSolids){
	mSensorResults->setSolids(solids,maxSolids);
	mVolumeSensor->setBox(box);
	mVolumeSensor->sense(mSensorResults);
	return mSensorResults->getCount();
}

void HopManager::traceSegment(hop::Collision &result,const Segment &segment,int collideWithScope){
	if(mTraceable!=NULL && (collideWithScope&mSolid->getCollisionScope())!=0){
		PhysicsCollision collision;
		mTraceable->traceSegment(collision,Math::ZERO_VECTOR3,segment,Math::ZERO_VECTOR3);
		set(result,collision,NULL,NULL);
		result.collider=mSolid;
	}
}

void HopManager::traceSolid(hop::Collision &result,hop::Solid *solid,const Segment &segment,int collideWithScope){
	// Only trace shapes that aren't a callback
	if(mTraceable!=NULL && (collideWithScope&mSolid->getCollisionScope())!=0 && (solid->getShapeTypes()&hop::Shape::Type_TRACEABLE)==0){
		PhysicsCollision collision;
		const AABox &bound=solid->getLocalBound();
		Vector3 size;
		Math::sub(size,bound.maxs,bound.mins);
		mTraceable->traceSegment(collision,Math::ZERO_VECTOR3,segment,size);
		set(result,collision,NULL,solid);
		result.collider=mSolid;
	}
}

void HopManager::set(PhysicsCollision &r,const hop::Collision &c){
	r.time=c.time;
	r.point.set(c.point);
	r.normal.set(c.normal);
	r.velocity.set(c.velocity);
	if(c.collider!=NULL && c.collider->getUserData()!=NULL){r.collider=((HopComponent*)c.collider->getUserData())->getParent();}
	r.scope=c.scope;
}

void HopManager::set(hop::Collision &r,const PhysicsCollision &c,hop::Solid *collider,hop::Solid *collidee){
	r.time=c.time;
	r.point.set(c.point);
	r.normal.set(c.normal);
	r.velocity.set(c.velocity);
	if(collider!=NULL){r.collider=collider;}
	if(collidee!=NULL){r.collidee=collidee;}
	r.scope=c.scope;
}

bool HopManager::SolidSensorResults::resultFound(Node *result,scalar distance){
	HopComponent *component=(HopComponent*)result->getPhysics();
	if(component!=NULL && mCounter<mMaxSolids && component->getSolid()->getNumShapes()>0){
		mSolids[mCounter++]=component->getSolid();
	}
	return mCounter<mMaxSolids;
}

}
}
