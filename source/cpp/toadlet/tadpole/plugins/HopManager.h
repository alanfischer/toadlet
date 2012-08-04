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

#ifndef TOADLET_TADPOLE_HOPMANAGER_H
#define TOADLET_TADPOLE_HOPMANAGER_H

#include <toadlet/hop/Simulator.h>
#include <toadlet/tadpole/PhysicsManager.h>
#include <toadlet/tadpole/Collision.h>
#include <toadlet/tadpole/Traceable.h>
#include <toadlet/tadpole/sensor/BoundingVolumeSensor.h>

namespace toadlet{
using namespace hop;
namespace tadpole{

class HopComponent;

class TOADLET_API HopManager:public Object,public PhysicsManager,public hop::Manager{
public:
	TOADLET_OBJECT(HopManager);

	HopManager(Scene *scene);
	virtual ~HopManager(){}

	PhysicsComponent *createPhysicsComponent();

	void traceSegment(Collision &result,const Segment &segment,int collideWithBits,Node *ignore);
	void traceNode(Collision &result,Node *entity,const Segment &segment,int collideWithBits);
	void testNode(Collision &result,Node *entity1,const Segment &segment,Node *entity2);

	void logicUpdate(int dt,int scope){logicUpdate(dt,scope,NULL);}
	void logicUpdate(int dt,int scope,HopComponent *component);
	void frameUpdate(int dt,int scope);

	Solid *getSolid(){return mSolid;}

	// Hop items
	inline Simulator *getSimulator(){return mSimulator;}

	void setTraceable(Traceable *traceable){mTraceable=traceable;}
	int findSolidsInAABox(const AABox &box,Solid *solids[],int maxSolids);
	void traceSegment(hop::Collision &result,const Segment &segment,int collideWithBits);
	void traceSolid(hop::Collision &result,Solid *solid,const Segment &segment,int collideWithBits);
	void preUpdate(int dt,scalar fdt){}
	void postUpdate(int dt,scalar fdt){}
	void preUpdate(Solid *solid,int dt,scalar fdt){}
	void intraUpdate(Solid *solid,int dt,scalar fdt){}
	bool collisionResponse(Solid *solid,Vector3 &position,Vector3 &remainder,hop::Collision &collision){return false;}
	void postUpdate(Solid *solid,int dt,scalar fdt){}

	static void set(tadpole::Collision &r,const hop::Collision &c);
	static void set(hop::Collision &r,const tadpole::Collision &c,Solid *collider,Solid *collidee);

protected:
	class SolidSensorResults:public SensorResultsListener{
	public:
		TOADLET_SPTR(SolidSensorResults);

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

	Scene::ptr mScene;
	Simulator::ptr mSimulator;
	Traceable *mTraceable;
	Solid::ptr mSolid;

	BoundingVolumeSensor::ptr mVolumeSensor;
	SolidSensorResults::ptr mSensorResults;
};

}
}

#endif
