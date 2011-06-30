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
#include <toadlet/tadpole/Collision.h>
#include <toadlet/tadpole/Traceable.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/sensor/BoundingVolumeSensor.h>

namespace toadlet{
using namespace hop;
namespace tadpole{

class HopEntity;

class TOADLET_API HopScene:public Scene,public hop::Manager{
public:
	TOADLET_SHARED_POINTERS(HopScene);

	HopScene(Engine *engine);
	virtual ~HopScene();

	virtual void traceSegment(Collision &result,const Segment &segment,int collideWithBits,Node *ignore);
	virtual void traceEntity(Collision &result,HopEntity *entity,const Segment &segment,int collideWithBits);
	virtual void testEntity(Collision &result,HopEntity *entity1,const Segment &segment,HopEntity *entity2);

	virtual void setFluidVelocity(const Vector3 &fluidVelocity){mSimulator->setFluidVelocity(fluidVelocity);}
	virtual const Vector3 &getFluidVelocity() const{return mSimulator->getFluidVelocity();}

	virtual void setGravity(const Vector3 &gravity){mSimulator->setGravity(gravity);}
	virtual const Vector3 &getGravity() const{return mSimulator->getGravity();}

	#if defined(TOADLET_FIXED_POINT)
		virtual void setEpsilonBits(int epsilonBits){mSimulator->setEpsilonBits(epsilonBits);}
		virtual int getEpsilonBits() const{return mSimulator->getEpsilonBits();}
	#else
		virtual void setEpsilon(scalar epsilon){mSimulator->setEpsilon(epsilon);}
		virtual scalar getEpsilon() const{return mSimulator->getEpsilon();}
	#endif

	virtual void logicUpdate(int dt){Scene::logicUpdate(dt);}
	virtual void logicUpdate(int dt,int scope){logicUpdate(dt,scope,NULL);}
	virtual void logicUpdate(int dt,int scope,HopEntity *entity);

	virtual Solid::ptr getSolid(){return mSolid;}

	// Hop items
	inline Simulator *getSimulator(){return mSimulator;}

	virtual void setTraceable(Traceable *traceable){mTraceable=traceable;}
	virtual int findSolidsInAABox(const AABox &box,Solid *solids[],int maxSolids);
	virtual void traceSegment(hop::Collision &result,const Segment &segment,int collideWithBits);
	virtual void traceSolid(hop::Collision &result,Solid *solid,const Segment &segment,int collideWithBits);
	virtual void preUpdate(int dt,scalar fdt){}
	virtual void postUpdate(int dt,scalar fdt){}
	virtual void preUpdate(Solid *solid,int dt,scalar fdt){}
	virtual void intraUpdate(Solid *solid,int dt,scalar fdt){}
	virtual bool collisionResponse(Solid *solid,Vector3 &position,Vector3 &remainder,hop::Collision &collision){return false;}
	virtual void postUpdate(Solid *solid,int dt,scalar fdt){}

	static void set(tadpole::Collision &r,const hop::Collision &c);
	static void set(hop::Collision &r,const tadpole::Collision &c,Solid *collider,Solid *collidee);

	hop::Collision cache_traceSegment_collision;

protected:
	class SolidSensorResults:public SensorResultsListener{
	public:
		TOADLET_SHARED_POINTERS(SolidSensorResults);

		SolidSensorResults():
			mSolids(NULL),
			mMaxSolids(0),
			mCounter(0)
		{}
			
		void setSolids(Solid *solids[],int maxSolids){
			mSolids=solids;
			mMaxSolids=maxSolids;
		}

		int getCount() const{return mCounter;}

		void sensingBeginning(){mCounter=0;}

		bool resultFound(Node *result,scalar distance);

		void sensingEnding(){}

	protected:
		Solid **mSolids;
		int mMaxSolids;
		int mCounter;
	};

	Simulator *mSimulator;
	Traceable *mTraceable;
	Solid::ptr mSolid;

	BoundingVolumeSensor::ptr mVolumeSensor;
	SolidSensorResults::ptr mSensorResults;

	friend class HopEntity;
};

}
}

#endif
