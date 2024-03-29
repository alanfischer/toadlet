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

#ifndef TOADLET_TADPOLE_BULLETCOMPONENT_H
#define TOADLET_TADPOLE_BULLETCOMPONENT_H

#include <toadlet/tadpole/PhysicsComponent.h>
#include <toadlet/tadpole/BaseComponent.h>
#include "BulletManager.h"

namespace toadlet{
namespace tadpole{

class TOADLET_API BulletComponent:public BaseComponent,public PhysicsComponent,public btMotionState{
public:
	TOADLET_COMPONENT2(BulletComponent,PhysicsComponent);

	BulletComponent(BulletManager *manager);
	~BulletComponent();

	void destroy();

	void parentChanged(Node *node);
	void rootChanged(Node *root);

	void addCollisionListener(PhysicsCollisionListener *listener);
	void removeCollisionListener(PhysicsCollisionListener *listener);

	void setPosition(const Vector3 &position);
	const Vector3 &getPosition() const{setVector3(mBulletPosition,mBody->getCenterOfMassPosition());return mBulletPosition;}

	void setOrientation(const Quaternion &orientation);
	const Quaternion &getOrientation() const{setQuaternion(mBulletOrientation,mBody->getOrientation());return mBulletOrientation;}

	void setVelocity(const Vector3 &velocity){btVector3 v;setVector3(v,velocity);mBody->setLinearVelocity(v);}
	const Vector3 &getVelocity() const{return reinterpret_cast<const Vector3&>(mBody->getLinearVelocity());}

	void setAngularVelocity(const Vector3 &velocity){btVector3 v;setVector3(v,velocity);mBody->setAngularVelocity(v);}
	const Vector3 &getAngularVelocity() const{return reinterpret_cast<const Vector3&>(mBody->getAngularVelocity());}

	void addForce(const Vector3 &force,const Vector3 &offset);

	void setMass(scalar mass);
	scalar getMass() const;

	void setGravity(scalar gravity);
	scalar getGravity() const;

	void setBound(Bound *bound);
	void setTraceable(PhysicsTraceable *traceable);

	bool getActive() const{return mBody->isActive();}

	void collision(const PhysicsCollision &collision);

	void logicUpdate(int dt, int scope){BaseComponent::logicUpdate(dt,scope);}
	void frameUpdate(int dt,int scope){BaseComponent::frameUpdate(dt,scope);}

	void setScope(int scope){}
	int getScope() const{return -1;}
	void setCollisionScope(int scope){mCollisionScope=scope;}
	int getCollisionScope() const{return mCollisionScope;}
	void setCollideWithScope(int scope){mCollideWithScope=scope;}
	int getCollideWithScope() const{return mCollideWithScope;}

	// Spacial
	Transform *getTransform() const{return NULL;}
	Bound *getBound() const{return mBound;}
	Transform *getWorldTransform() const{return NULL;}
	Bound *getWorldBound() const{return mWorldBound;}
	void transformChanged(Transform *transform);

	// btMotionState
	void getWorldTransform(btTransform& worldTrans) const;
	void setWorldTransform(const btTransform& worldTrans);

	btRigidBody *getBody() const{return mBody;}
	btCompoundShape *getShape() const{return mShape;}

protected:
	friend class BulletManager;

	void preSimulate();
	void postSimulate();
	void updatePosition(const Vector3 &position);

	BulletManager *mManager;
	btRigidBody *mBody;
	btCompoundShape *mShape;
	int mCollisionScope;
	int mCollideWithScope;
	Collection<PhysicsCollisionListener*> mListeners;
	Vector3 mKinematicPosition;
	Quaternion mKinematicOrientation;
	Vector3 mCurrentPosition;
	Quaternion mCurrentOrientation;
	Bound::ptr mBound;
	Bound::ptr mWorldBound;
	PhysicsTraceable::ptr mTraceable;

	mutable Vector3 mBulletPosition;
	mutable Quaternion mBulletOrientation;
};

}
}

#endif
