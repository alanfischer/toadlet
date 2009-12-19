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

#ifndef TOADLET_TADPOLE_HOPSCENE_H
#define TOADLET_TADPOLE_HOPSCENE_H

#include <toadlet/hop/Simulator.h>
#include <toadlet/tadpole/plugins/hop/HopEntityFactory.h>
#include <toadlet/tadpole/node/Scene.h> HACK

namespace toadlet{
namespace tadpole{

class HopNode;
class HopCollision;
class HopEntityMessage;

class TOADLET_API HopScene:public node::Scene,public hop::Manager{
public:
	HopScene();
	virtual ~HopScene();

	virtual void setScene(node::Scene *scene);

	virtual void setFluidVelocity(const Vector3 &fluidVelocity);
	virtual const Vector3 &getFluidVelocity() const;

	virtual void setGravity(const Vector3 &gravity);
	virtual const Vector3 &getGravity() const;

	virtual void findHopEntitiesInAABox(const AABox &box,egg::Collection<egg::IntrusivePointer<HopEntity> > &entities);
	virtual void findHopEntitiesInSphere(const Sphere &sphere,egg::Collection<egg::IntrusivePointer<HopEntity> > &entities);
	virtual void findHopEntitiesInSolids(egg::Collection<egg::IntrusivePointer<HopEntity> > &entities,hop::Solid *solids[],int numSolids);

	virtual void traceSegment(HopCollision &result,const Segment &segment,int collideWithBits,HopEntity *ignore);

	inline int getNumHopEntities() const{return mHopEntities.size();}
	inline HopEntity *getHopEntity(int i) const{return mHopEntities[i];}
	HopEntity *getHopEntityFromNetworkID(int id) const;
	void setHopEntityNetworkID(HopEntity *entity,int id);
	inline void addFreeNetworkID(int id){mFreeNetworkIDs.add(id);}
	virtual void resetNetworkIDs();

	virtual void setHopEntityFactory(HopEntityFactory *factory);

	virtual void showCollisionVolumes(bool show,bool interpolate);

	inline hop::Simulator *getSimulator(){return mSimulator;}

	virtual node::ParticleNode::ParticleSimulator::ptr newParticleSimulator(node::ParticleNode *particleNode);
	virtual void registerHopEntity(HopEntity *entity);
	virtual void unregisterHopEntity(HopEntity *entity);

	virtual void preLogicUpdateLoop(int dt);
	virtual void logicUpdate(int dt);
	virtual void postLogicUpdateLoop(int dt);
	virtual void renderUpdate(int dt);

	hop::Collision cache_traceSegment_collision;

	// HACK:
hop::Solid *mWorld;
void traceSegment(hop::Collision &result,const Segment &segment){
	result.time=super::traceSegment(result.normal,segment);
	if(result.time==1.0){result.time=-1;}
	if(result.time<0) result.point=segment.origin+segment.direction;
	else{result.point=segment.origin+segment.direction*result.time;
		result.collider=mWorld;
	}
}
void traceSolid(hop::Collision &result,const Segment &segment,const hop::Solid *solid){
	if(solid->getShape(0)->getType()==hop::Shape::Type_AABOX){
		result.time=super::traceAABox(result.normal,segment,solid->getShape(0)->getAABox());
	}
	else if(solid->getShape(0)->getType()==hop::Shape::Type_SPHERE){
		result.time=super::traceSphere(result.normal,segment,solid->getShape(0)->getSphere());
	}
	if(result.time==1.0){result.time=-1;}
	if(result.time<0) result.point=segment.origin+segment.direction;
	else{result.point=segment.origin+segment.direction*result.time;
		result.collider=mWorld;
	}
}
void preUpdate(int,scalar){}
void preUpdate(hop::Solid*,int,scalar){}
void intraUpdate(hop::Solid*,int,scalar){}
bool collisionResponse(hop::Solid*,Vector3&,Vector3&,hop::Collision&){return false;}
void postUpdate(hop::Solid*,int,scalar){}
void postUpdate(int,scalar){}

protected:
	virtual void defaultRegisterHopEntity(HopEntity *entity);
	virtual void defaultUnregisterHopEntity(HopEntity *entity);

	void castShadow(peeper::Renderer *renderer,HopEntity *entity);

	egg::Collection<HopEntity*> mHopEntities;
	bool mShowCollisionVolumes;
	bool mInterpolateCollisionVolumes;

	int mExcessiveDT;
	hop::Simulator *mSimulator;

	egg::Collection<int> mFreeNetworkIDs;
	egg::Collection<egg::IntrusivePointer<HopEntity> > mNetworkIDMap;
	HopEntityFactory *mHopEntityFactory;

	egg::Collection<hop::Solid*> mSolidCollection;

	friend class HopEntity;
};

}
}

#endif
