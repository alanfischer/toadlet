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

#include "BulletComponent.h"
#include <toadlet/tadpole/Engine.h>

namespace toadlet{
namespace tadpole{

BulletComponent::BulletComponent(BulletManager *manager)
	//mManager
{
	mManager=manager;

	mShape=new btCompoundShape();
	mBody=new btRigidBody(1.0f,this,mShape);

	mBound=new Bound();
	mWorldBound=new Bound();
}

BulletComponent::~BulletComponent(){
	if(mBody!=NULL){
		delete mBody;
		mBody=NULL;
	}

	if(mShape!=NULL){
		delete mShape;
		mShape=NULL;
	}
}

void BulletComponent::destroy(){
	BaseComponent::destroy();
}

void BulletComponent::parentChanged(Node *node){
	if(mParent!=NULL){
		mParent->physicsRemoved(this);
		mParent->spacialRemoved(this);
	}

	BaseComponent::parentChanged(node);

	if(mParent!=NULL){
		mParent->physicsAttached(this);
		mParent->spacialAttached(this);

		mParent->boundChanged();
	}
}

void BulletComponent::rootChanged(Node *root){
	BaseComponent::rootChanged(root);

	if(mParent!=NULL && root!=NULL && mParent->getParent()==root){
		mManager->getWorld()->addRigidBody(mBody);
	}
	else{
		mManager->getWorld()->removeRigidBody(mBody);
	}
}

void BulletComponent::setPosition(const Vector3 &position){
	btTransform transform=mBody->getWorldTransform();
	btVector3 origin;
	setVector3(origin,position);
	transform.setOrigin(origin);
	mBody->setWorldTransform(transform);
}

void BulletComponent::setMass(scalar mass){
	mass=Math::maxVal(mass,0);
	mBody->setMassProps(mass,btVector3());
}

scalar BulletComponent::getMass() const{
	scalar mass=mBody->getInvMass();
	if(mass>0){
		mass=Math::div(Math::ONE,mass);
	}
	return mass;
}

void BulletComponent::setGravity(scalar amount){
	btVector3 gravity;
	setVector3(gravity,mManager->getGravity());
	mBody->setGravity(gravity*amount);
}

scalar BulletComponent::getGravity() const{
	btVector3 gravity;
	setVector3(gravity,mManager->getGravity());
	return gravity.dot(mBody->getGravity());
}

void BulletComponent::setBound(Bound *bound){
	mBody->setCollisionShape(NULL);
	while(mShape->getNumChildShapes()>0){
		mShape->removeChildShapeByIndex(0);
	}

	btTransform transform;
	transform.setIdentity();
	btCollisionShape *shape=NULL;
	switch(bound->getType()){
		case Bound::Type_AABOX:{
			Vector3 hsize;
			Math::sub(hsize,bound->getAABox().getMaxs(),bound->getAABox().getMins());
			Math::div(hsize,2);
			Vector3 origin;
			Math::add(origin,bound->getAABox().getMins(),hsize);

			btVector3 bextends;
			setVector3(bextends,hsize);
			shape=new btBoxShape(bextends);

			btVector3 borigin;
			setVector3(borigin,origin);
			transform.setOrigin(borigin);
		}break;
		case Bound::Type_SPHERE:{
			shape=new btSphereShape(bound->getSphere().getRadius());

			btVector3 borigin;
			setVector3(borigin,bound->getSphere().getOrigin());
			transform.setOrigin(borigin);
		}break;
	}

	mShape->addChildShape(transform,shape);
	mBody->setCollisionShape(mShape);
}

void BulletComponent::setTraceable(PhysicsTraceable *traceable){
}

void BulletComponent::transformChanged(Transform *transform){
	if(mParent==NULL){
		return;
	}

	Transform *parentTransform=mParent->getTransform();
	const Vector3 &translate=parentTransform->getTranslate();
	const Quaternion &rotate=parentTransform->getRotate();
	if(mCurrentPosition.equals(translate)==false || mCurrentOrientation.equals(rotate)==false){
		mCurrentPosition.set(translate);
		mCurrentOrientation.set(rotate);

		btVector3 bmin,bmax;
		mBody->getAabb(bmin,bmax);
		Vector3 min,max;
		setVector3(min,bmin);setVector3(max,bmax);
		mBound->set(AABox(min,max));

		mWorldBound->transform(mBound,mParent->getWorldTransform());

		btTransform worldTrans;
		getWorldTransform(worldTrans);
		mBody->setWorldTransform(worldTrans);
	}
}

void BulletComponent::collision(const PhysicsCollision &collision){
}

void BulletComponent::getWorldTransform(btTransform& worldTrans) const{
	if(mParent==NULL){
		return;
	}

	Transform *parentTransform=mParent->getTransform();
	Vector3 position=parentTransform->getTranslate();
	Quaternion orientation=parentTransform->getRotate();
	btVector3 origin;
	setVector3(origin,position);
	btQuaternion rotation;
	setQuaternion(rotation,orientation);
	worldTrans.setIdentity();
	worldTrans.setOrigin(origin);
	worldTrans.setRotation(rotation);
}

void BulletComponent::setWorldTransform(const btTransform& worldTrans){
	if(mParent==NULL){
		return;
	}

	setVector3(mCurrentPosition,worldTrans.getOrigin());
	setQuaternion(mCurrentOrientation,worldTrans.getRotation());
	mParent->getTransform()->setTranslate(mCurrentPosition);
	mParent->getTransform()->setRotate(mCurrentOrientation);
}

}
}