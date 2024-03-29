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

class TOADLET_API HopComponent:public BaseComponent,public PhysicsComponent,public hop::CollisionListener,public hop::Traceable{
public:
	TOADLET_COMPONENT2(HopComponent,PhysicsComponent);

	HopComponent(HopManager *manager);

	void destroy();

	void parentChanged(Node *node);
	void rootChanged(Node *root);

	void addCollisionListener(PhysicsCollisionListener *listener);
	void removeCollisionListener(PhysicsCollisionListener *listener);

	void setPosition(const Vector3 &position);
	const Vector3 &getPosition() const{return mSolid->getPosition();}

	void setOrientation(const Quaternion &orientation){}
	const Quaternion &getOrientation() const{return Math::IDENTITY_QUATERNION;}

	void setVelocity(const Vector3 &velocity){mSolid->setVelocity(velocity);}
	const Vector3 &getVelocity() const{return mSolid->getVelocity();}

	void setAngularVelocity(const Vector3 &velocity){}
	const Vector3 &getAngularVelocity() const{return Math::ZERO_VECTOR3;}

	void addForce(const Vector3 &force,const Vector3 &offset){mSolid->addForce(force);}

	void setMass(scalar mass){mSolid->setMass(mass);}
	scalar getMass() const{return mSolid->getMass();}

	void setGravity(scalar gravity){mSolid->setCoefficientOfGravity(gravity);}
	scalar getGravity() const{return mSolid->getCoefficientOfGravity();}

	void setScope(int scope){mSolid->setCollisionScope(scope);mSolid->setCollideWithScope(scope);mSolid->setInternalScope(scope);}
	int getScope() const{return mSolid->getCollisionScope();}

	void setCollisionScope(int scope){mSolid->setCollisionScope(scope);}
	int getCollisionScope() const{return mSolid->getCollisionScope();}

	void setCollideWithScope(int scope){mSolid->setCollideWithScope(scope);}
	int getCollideWithScope() const{return mSolid->getCollideWithScope();}

	void setBound(Bound *bound);
	void setTraceable(PhysicsTraceable *traceable);
	void addShape(hop::Shape *shape);
	void removeShape(hop::Shape *shape);

	void logicUpdate(int dt,int scope){}
	void frameUpdate(int dt,int scope){}

	bool getActive() const{return mSolid->active();}

	inline hop::Solid *getSolid(){return mSolid;}

	void collision(const PhysicsCollision &collision);

	// CollisionListener
	void collision(const hop::Collision &c);

	// TraceCallback
	void getBound(AABox &result);
	void traceSegment(hop::Collision &result,const Vector3 &position,const Segment &segment);
	void traceSolid(hop::Collision &result,hop::Solid *solid,const Vector3 &position,const Segment &segment);

	// Spacial
	Transform *getTransform() const{return NULL;}
	Bound *getBound() const{return mBound;}
	Transform *getWorldTransform() const{return NULL;}
	Bound *getWorldBound() const{return mWorldBound;}
	void transformChanged(Transform *transform);

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
	Bound::ptr mWorldBound;
	PhysicsTraceable::ptr mTraceable;
	hop::Shape::ptr mTraceableShape;
};

}
}

#endif
