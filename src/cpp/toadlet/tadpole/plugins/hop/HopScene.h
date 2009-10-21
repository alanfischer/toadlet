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
#include <toadlet/tadpole/entity/Scene.h>

namespace toadlet{
namespace tadpole{

class HopEntity;
class HopCollision;
class HopEntityMessage;

class TOADLET_API HopScene:public entity::Scene{
public:
	TOADLET_ENTITY(HopScene,entity::Scene);

	HopScene();
	virtual ~HopScene();
	virtual Entity *create(Engine *engine);

	virtual void setFluidVelocity(const Vector3 &fluidVelocity);
	virtual const Vector3 &getFluidVelocity() const;

	virtual void setGravity(const Vector3 &gravity);
	virtual const Vector3 &getGravity() const;

	virtual void findHopEntitiesInAABox(const AABox &box,egg::Collection<egg::IntrusivePointer<HopEntity,entity::Entity> > &entities);
	virtual void findHopEntitiesInSphere(const Sphere &sphere,egg::Collection<egg::IntrusivePointer<HopEntity,entity::Entity> > &entities);
	virtual void findHopEntitiesInSolids(egg::Collection<egg::IntrusivePointer<HopEntity,entity::Entity> > &entities,hop::Solid *solids[],int numSolids);

	virtual void traceSegment(HopCollision &result,const Segment &segment,int collideWithBits,HopEntity *ignore);

	inline int getNumHopEntities() const{return mHopEntities.size();}
	inline HopEntity *getHopEntity(int i) const{return mHopEntities[i];}
	HopEntity *getHopEntityFromNetworkID(int id) const;
	void setHopEntityNetworkID(HopEntity *entity,int id);
	inline void addFreeNetworkID(int id){mFreeNetworkIDs.add(id);}
	virtual void resetNetworkIDs();

	virtual void setHopEntityFactory(HopEntityFactory *factory);

	virtual void setRenderCollisionVolumes(bool volumes,bool interpolate,int layer);

	inline hop::Simulator *getSimulator(){return mSimulator;}

	virtual bool postLayerRender(peeper::Renderer *renderer,int layer);

	virtual entity::ParticleEntity::ParticleSimulator::ptr newParticleSimulator(entity::ParticleEntity *particleEntity);
	virtual void registerHopEntity(HopEntity *entity);
	virtual void unregisterHopEntity(HopEntity *entity);

	virtual void preLogicUpdateLoop(int dt);
	virtual void logicUpdate(int dt);
	virtual void postLogicUpdateLoop(int dt);
	virtual void visualUpdate(int dt);

	hop::Collision cache_traceSegment_collision;

protected:
	virtual void defaultRegisterHopEntity(HopEntity *entity);
	virtual void defaultUnregisterHopEntity(HopEntity *entity);

	void castShadow(peeper::Renderer *renderer,HopEntity *entity);

	egg::Collection<HopEntity*> mHopEntities;

	int mExcessiveDT;
	hop::Simulator *mSimulator;

	egg::Collection<int> mFreeNetworkIDs;
	egg::Collection<egg::IntrusivePointer<HopEntity,entity::Entity> > mNetworkIDMap;
	HopEntityFactory *mHopEntityFactory;

	egg::Collection<hop::Solid*> mSolidCollection;

	bool mRenderCollisionVolumes;
	bool mInterpolateCollisionVolumes;
	peeper::VertexData::ptr mCollisionVolumeVertexData;
	peeper::IndexData::ptr mCollisionVolumeIndexData;
	int mCollisionVolumeLayer;
	Matrix4x4 mCollisionVolumeMatrix;
	Material::ptr mCollisionVolumeMaterial;

	friend class HopEntity;
};

}
}

#endif
