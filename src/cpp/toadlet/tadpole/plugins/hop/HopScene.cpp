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
#include <toadlet/tadpole/Collision.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/plugins/hop/HopScene.h>
#include <toadlet/tadpole/plugins/hop/HopEntity.h>

using namespace toadlet::egg;
using namespace toadlet::hop;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::node;

namespace toadlet{
namespace tadpole{

HopScene::HopScene(Engine *engine):Scene(engine),
	mSimulator(NULL),
	mTraceable(NULL)
{
	mSimulator=new Simulator();
	mSimulator->setManager(this);

	mSolid=Solid::ptr(new Solid());
	mSolid->setCoefficientOfGravity(0);
	mSolid->setInfiniteMass();

	mEngine->registerNodeType(HopEntity::type());
}

HopScene::~HopScene(){
	if(mSimulator!=NULL){
		delete mSimulator;
		mSimulator=NULL;
	}
}

void HopScene::traceSegment(Collision &result,const Segment &segment,int collideWithBits,Node *ignore){
	result.reset();

	Solid *ignoreSolid=NULL;
	if(ignore!=NULL && ignore->isEntity()!=NULL){
		ignoreSolid=((HopEntity*)ignore)->getSolid();
	}

	hop::Collision &collision=cache_traceSegment_collision.reset();
	mSimulator->traceSegment(collision,segment,collideWithBits,ignoreSolid);
	set(result,collision);
}

void HopScene::traceEntity(Collision &result,HopEntity *entity,const Segment &segment,int collideWithBits){
	result.reset();

	Solid *solid=NULL;
	if(entity!=NULL){
		solid=entity->getSolid();
	}

	hop::Collision &collision=cache_traceSegment_collision.reset();
	mSimulator->traceSolid(collision,solid,segment,collideWithBits);
	set(result,collision);
}

void HopScene::testEntity(Collision &result,HopEntity *entity1,const Segment &segment,HopEntity *entity2){
	result.reset();

	Solid *solid1=entity1->getSolid();
	Solid *solid2=entity2->getSolid();

	hop::Collision &collision=cache_traceSegment_collision.reset();
	mSimulator->testSolid(collision,solid1,segment,solid2);
	set(result,collision);
}

void HopScene::logicUpdate(int dt,int scope){
	TOADLET_PROFILE_BEGINSECTION(Simulator::update);
	mSimulator->update(dt,scope);
	TOADLET_PROFILE_ENDSECTION(Simulator::update);

	// Reactivate any nodes that might have been deactivated
	int i;
	for(i=mSimulator->getNumSolids()-1;i>=0;--i){
		Solid *solid=mSimulator->getSolid(i);
		HopEntity *entity=(HopEntity*)(solid->getUserData());
		if(entity->active()==false && solid->active()==true){
			entity->activate();
		}
	}

	Scene::logicUpdate(dt,scope);
}

class SolidSensorResults:public SensorResultsListener{
public:
	SolidSensorResults(Solid *solids[],int maxSolids){
		mSolids=solids;
		mMaxSolids=maxSolids;
		mCounter=0;
	}

	bool resultFound(Node *result){
		HopEntity *entity=(HopEntity*)result->isEntity();
		if(entity!=NULL && mCounter<mMaxSolids && entity->getNumShapes()>0){
			mSolids[mCounter++]=entity->getSolid();
		}
		return true;
	}

	Solid **mSolids;
	int mMaxSolids;
	int mCounter;
};

int HopScene::findSolidsInAABox(const AABox &box,Solid *solids[],int maxSolids){
	/// @todo: just clean this up a bit, maybe cache the SolidSensorResults as a ::ptr
	SolidSensorResults results(solids,maxSolids);
	Sphere bound;
	Node::set(bound,box);
	mRoot->senseBoundingVolumes(&results,bound);
	return results.mCounter;
}

void HopScene::traceSegment(hop::Collision &result,const Segment &segment){
	if(mTraceable!=NULL){
		tadpole::Collision collision;
		mTraceable->traceSegment(collision,Math::ZERO_VECTOR3,segment,Math::ZERO_VECTOR3);
		set(result,collision,NULL);
		result.collider=mSolid;
	}
}

void HopScene::traceSolid(hop::Collision &result,const Segment &segment,const hop::Solid *solid){
	// Only trace shapes that aren't a callback
	if(mTraceable!=NULL && (solid->getShapeTypes()&Shape::Type_CALLBACK)==0){
		tadpole::Collision collision;
		const AABox &bound=solid->getLocalBound();
		Vector3 size;
		Math::sub(size,bound.maxs,bound.mins);
		mTraceable->traceSegment(collision,Math::ZERO_VECTOR3,segment,size);
		set(result,collision,NULL);
		result.collider=mSolid;
	}
}

void HopScene::set(tadpole::Collision &r,const hop::Collision &c){
	r.time=c.time;
	r.point.set(c.point);
	r.normal.set(c.normal);
	if(c.collider!=NULL){r.collider=(HopEntity*)c.collider->getUserData();};
	r.scope=c.scope;
}

void HopScene::set(hop::Collision &r,const tadpole::Collision &c,HopEntity *collider){
	r.time=c.time;
	r.point.set(c.point);
	r.normal.set(c.normal);
	// Since the c.collider passed in could be any Node, not necessarily a HopEntity, we force a passing in of a Collider
	if(collider!=NULL){r.collider=collider->getSolid();}
	r.scope=c.scope;
}

}
}
