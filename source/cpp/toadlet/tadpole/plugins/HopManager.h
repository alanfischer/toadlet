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
#include <toadlet/tadpole/PhysicsCollision.h>
#include <toadlet/tadpole/PhysicsManager.h>
#include <toadlet/tadpole/PhysicsManagerListener.h>
#include <toadlet/tadpole/PhysicsTraceable.h>
#include <toadlet/tadpole/sensor/BoundingVolumeSensor.h>

namespace toadlet{
namespace tadpole{

class HopComponent;

class TOADLET_API HopManager:public Object,public PhysicsManager,public hop::Manager{
public:
	TOADLET_IOBJECT(HopManager);

	HopManager(Scene *scene);
	virtual ~HopManager(){}

	PhysicsComponent *createPhysicsComponent();

	void addListener(PhysicsManagerListener *listener);
	void removeListener(PhysicsManagerListener *listener);
	
	void setFixedDT(float fdt){}
	float getFixedDT() const{return 0;}

	void setGravity(const Vector3 &gravity);
	const Vector3 &getGravity() const;

	void setTraceable(PhysicsTraceable *traceable);
	void setTraceableCollisionScope(int scope);
	int getTraceableCollisionScope() const;

	void traceSegment(PhysicsCollision &result,const Segment &segment,int collideWithScope,Node *ignore);
	void traceNode(PhysicsCollision &result,Node *node,const Segment &segment,int collideWithScope);
	void testNode(PhysicsCollision &result,Node *node,const Segment &segment,Node *node2);

	void logicUpdate(int dt,int scope,Node *node);
	void frameUpdate(int dt,int scope,Node *node);

	hop::Solid *getSolid(){return mSolid;}

	// Hop items
	inline hop::Simulator *getSimulator(){return mSimulator;}

	int findSolidsInAABox(const AABox &box,hop::Solid *solids[],int maxSolids);
	void traceSegment(hop::Collision &result,const Segment &segment,int collideWithScope);
	void traceSolid(hop::Collision &result,hop::Solid *solid,const Segment &segment,int collideWithScope);
	void preUpdate(int dt,scalar fdt);
	void postUpdate(int dt,scalar fdt){}
	void preUpdate(hop::Solid *solid,int dt,scalar fdt){}
	void intraUpdate(hop::Solid *solid,int dt,scalar fdt){}
	bool collisionResponse(hop::Solid *solid,Vector3 &position,Vector3 &remainder,hop::Collision &collision){return false;}
	void postUpdate(hop::Solid *solid,int dt,scalar fdt){}

	static void set(PhysicsCollision &r,const hop::Collision &c);
	static void set(hop::Collision &r,const PhysicsCollision &c,hop::Solid *collider,hop::Solid *collidee);

protected:
	class SolidSensorResults:public SensorResultsListener{
	public:
		TOADLET_SPTR(SolidSensorResults);

		SolidSensorResults():
			mSolids(NULL),
			mMaxSolids(0),
			mCounter(0)
		{}
			
		void setSolids(hop::Solid *solids[],int maxSolids){
			mSolids=solids;
			mMaxSolids=maxSolids;
		}

		int getCount() const{return mCounter;}

		void sensingBeginning(){mCounter=0;}

		bool resultFound(Node *result,scalar distance);

		void sensingEnding(){}

	protected:
		hop::Solid **mSolids;
		int mMaxSolids;
		int mCounter;
	};

	Scene::ptr mScene;
	hop::Simulator::ptr mSimulator;
	PhysicsTraceable *mTraceable;
	hop::Solid::ptr mSolid;
	Collection<PhysicsManagerListener*> mListeners;

	BoundingVolumeSensor::ptr mVolumeSensor;
	SolidSensorResults::ptr mSensorResults;
};

}
}

#endif
