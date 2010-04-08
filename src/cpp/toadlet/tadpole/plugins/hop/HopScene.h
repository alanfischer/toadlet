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

namespace toadlet{
namespace tadpole{

class HopEntity;

class TOADLET_API HopScene:public Scene,public hop::Manager{
public:
	TOADLET_SHARED_POINTERS(HopScene);

	HopScene(Engine *engine);
	virtual ~HopScene();

	virtual void traceSegment(Collision &result,const Segment &segment,int collideWithBits,HopEntity *ignore);
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

	virtual void logicUpdate(int dt);

	// Hop items
	inline hop::Simulator *getSimulator(){return mSimulator;}

	virtual void setTraceable(Traceable *traceable){mTraceable=traceable;}
	virtual int findSolidsInAABox(const AABox &box,hop::Solid *solids[],int maxSolids);
	virtual void traceSegment(hop::Collision &result,const Segment &segment);
	virtual void traceSolid(hop::Collision &result,const Segment &segment,const hop::Solid *solid);
	virtual void preUpdate(int dt,scalar fdt){}
	virtual void postUpdate(int dt,scalar fdt){}
	virtual void preUpdate(hop::Solid *solid,int dt,scalar fdt){}
	virtual void intraUpdate(hop::Solid *solid,int dt,scalar fdt){}
	virtual bool collisionResponse(hop::Solid *solid,Vector3 &position,Vector3 &remainder,hop::Collision &collision){return false;}
	virtual void postUpdate(hop::Solid *solid,int dt,scalar fdt){}

	static void set(tadpole::Collision &r,const hop::Collision &c);
	static void set(hop::Collision &r,const tadpole::Collision &c,HopEntity *collider);

	hop::Collision cache_traceSegment_collision;

protected:
	hop::Simulator *mSimulator;
	Traceable *mTraceable;

	friend class HopEntity;
};

}
}

#endif
