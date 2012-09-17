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

#ifndef TOADLET_TADPOLE_HOPCOMPONENT_H
#define TOADLET_TADPOLE_HOPCOMPONENT_H

#include <toadlet/tadpole/PhysicsComponent.h>
#include <toadlet/tadpole/BaseComponent.h>
#include "HopManager.h"

namespace toadlet{
namespace tadpole{

class TOADLET_API HopComponent:public BaseComponent,public PhysicsComponent,public hop::CollisionListener,public hop::TraceCallback{
public:
	TOADLET_COMPONENT(HopComponent,PhysicsComponent);

	HopComponent(HopManager *manager);

	void destroy();

	void parentChanged(Node *node);
	void rootChanged(Node *root);

	void addCollisionListener(PhysicsCollisionListener *listener){mListeners.add(listener);}
	void removeCollisionListener(PhysicsCollisionListener *listener){mListeners.remove(listener);}

	void setPosition(const Vector3 &position);
	const Vector3 &getPosition() const{return mSolid->getPosition();}

	void setVelocity(const Vector3 &velocity){mSolid->setVelocity(velocity);}
	const Vector3 &getVelocity() const{return mSolid->getVelocity();}

	virtual void setMass(scalar mass){mSolid->setMass(mass);}
	virtual scalar getMass() const{return mSolid->getMass();}

	void setGravity(scalar gravity){mSolid->setCoefficientOfGravity(gravity);}
	scalar getGravity() const{return mSolid->getCoefficientOfGravity();}

	void setCollisionScope(int scope){mSolid->setCollisionScope(scope);mSolid->setCollideWithScope(scope);mSolid->setInternalScope(scope);}
	int getCollisionScope() const{return mSolid->getCollisionScope();}

	void setBound(Bound *bound);
	void setTraceable(Traceable *traceable);
	void addShape(hop::Shape *shape);
	void removeShape(hop::Shape *shape);

	void transformChanged();

	void logicUpdate(int dt,int scope){}
	void frameUpdate(int dt,int scope){}

	bool getActive() const{return mSolid->active();}
	Bound *getBound() const{return mBound;}

	inline hop::Solid *getSolid(){return mSolid;}

	void collision(const PhysicsCollision &collision);

	// CollisionListener
	void collision(const hop::Collision &c);

	// TraceCallback
	void getBound(AABox &result);
	void traceSegment(hop::Collision &result,const Vector3 &position,const Segment &segment);
	void traceSolid(hop::Collision &result,hop::Solid *solid,const Vector3 &position,const Segment &segment);

protected:
	friend class HopManager;

	void preSimulate();
	void postSimulate();
	void updatePosition(const Vector3 &position);
	void lerpPosition(scalar fraction);

	HopManager *mManager;
	hop::Solid::ptr mSolid;
	Collection<PhysicsCollisionListener*> mListeners;
	Vector3 mOldPosition,mNewPosition,mCurrentPosition;
	bool mTransformChanged;
	Bound::ptr mBound;
	Traceable::ptr mTraceable;
	hop::Shape::ptr mTraceableShape;
};

}
}

#endif
