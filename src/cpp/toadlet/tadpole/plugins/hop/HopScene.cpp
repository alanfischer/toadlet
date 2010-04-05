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
#include <toadlet/tadpole/node/SceneNode.h>
#include <toadlet/tadpole/plugins/hop/HopScene.h>
#include <toadlet/tadpole/plugins/hop/HopEntity.h>

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

	//mHopEntities,
	mShowCollisionVolumes(false),
	mInterpolateCollisionVolumes(false),

	mExcessiveDT(0),
	mSimulator(NULL),
	mTraceable(NULL)
{
	mSimulator=new Simulator();
	mSimulator->setManager(this);
	mScene=scene;
	mScene->setChildScene(this);

	mScene->getRootNode()->getEngine()->registerNodeType(HopEntity::type());
}

HopScene::~HopScene(){
	if(mSimulator!=NULL){
		delete mSimulator;
		mSimulator=NULL;
	}
}

void HopScene::traceSegment(Collision &result,const Segment &segment,int collideWithBits,HopEntity *ignore){
	result.reset();

	Solid *ignoreSolid=NULL;
	if(ignore!=NULL){
		ignoreSolid=ignore->getSolid();
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

void HopScene::showCollisionVolumes(bool show,bool interpolate){
	mShowCollisionVolumes=show;
	mInterpolateCollisionVolumes=interpolate;

	int i;
	for(i=0;i<mHopEntities.size();++i){
		mHopEntities[i]->showCollisionVolumes(mShowCollisionVolumes);
	}
}

int HopScene::nodeCreated(HopEntity *entity){
	if(mHopEntities.contains(entity)==false){
		mHopEntities.add(entity);

		if(mShowCollisionVolumes){
			entity->showCollisionVolumes(true);
		}
	}

	return entity->getHandle();
}

void HopScene::nodeDestroyed(HopEntity *entity){
	if(mHopEntities.contains(entity)){
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
	TOADLET_PROFILE_BEGINSECTION(Simulator::update);
	mSimulator->update(dt);
	TOADLET_PROFILE_ENDSECTION(Simulator::update);

	mScene->logicUpdate(dt);
}

void HopScene::postLogicUpdate(int dt){
	mScene->postLogicUpdate(dt);
}

void HopScene::postLogicUpdateLoop(int dt){
	mScene->postLogicUpdateLoop(dt);
}

void HopScene::preFrameUpdate(int dt){
	mScene->preFrameUpdate(dt);
}

void HopScene::frameUpdate(int dt){
	mScene->frameUpdate(dt);
}

void HopScene::postFrameUpdate(int dt){
	mScene->postFrameUpdate(dt);
}

class QueryListener:public SpacialQueryResultsListener{
public:
	QueryListener(Solid *solids[],int maxSolids){
		mSolids=solids;
		mMaxSolids=maxSolids;
		mCounter=0;
	}

	void resultFound(Node *result){
		HopEntity *entity=(HopEntity*)result->isEntity();
		if(entity!=NULL && mCounter<mMaxSolids && entity->getNumShapes()>0){
			mSolids[mCounter++]=entity->getSolid();
		}
	}

	Solid **mSolids;
	int mMaxSolids;
	int mCounter;
};

int HopScene::findSolidsInAABox(const AABox &box,Solid *solids[],int maxSolids){
	QueryListener listener(solids,maxSolids);
	SpacialQuery query;
	query.setResultsListener(&listener);
	performAABoxQuery(&query,box,false);
	return listener.mCounter;
}

void HopScene::traceSegment(hop::Collision &result,const Segment &segment){
	if(mTraceable!=NULL){
		tadpole::Collision collision;
		mTraceable->traceSegment(collision,segment,Math::ZERO_VECTOR3);
		set(result,collision,NULL);
	}
}

void HopScene::traceSolid(hop::Collision &result,const Segment &segment,const hop::Solid *solid){
	if(mTraceable!=NULL){
		tadpole::Collision collision;
		const AABox &bound=solid->getLocalBound();
		Vector3 size;
		Math::sub(size,bound.maxs,bound.mins);
		mTraceable->traceSegment(collision,segment,size);
		set(result,collision,NULL);
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
