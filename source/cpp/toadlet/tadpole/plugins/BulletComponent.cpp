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

	btCollisionShape *mShape=new btSphereShape(1.0f);
	mBody=new btRigidBody(1.0f,this,mShape);

	mBound=new Bound();
	mWorldBound=new Bound();
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

void BulletComponent::setBound(Bound *bound){
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
