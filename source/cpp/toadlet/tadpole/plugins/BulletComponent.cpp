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

BulletComponent::BulletComponent(BulletManager *manager):
	//mManager
	mCollisionScope(-1),
	mCollideWithScope(-1)
{
	mManager=manager;

	mShape=new btCompoundShape();
	mBody=new btRigidBody(1.0f,this,mShape);
	mBody->setUserPointer(this);

	mBound=new Bound();
	mWorldBound=new Bound();

	if(mManager!=NULL){
		mManager->componentCreated(this);
	}
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
	if(mManager!=NULL){
		mManager->componentDestroyed(this);
	}

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
		if(mCollisionScope!=-1 || mCollideWithScope!=-1){
			mManager->getWorld()->addRigidBody(mBody,mCollisionScope,mCollideWithScope);
		}
		else{
			mManager->getWorld()->addRigidBody(mBody,btBroadphaseProxy::DefaultFilter,btBroadphaseProxy::AllFilter);
		}
	}
	else{
		mManager->getWorld()->removeRigidBody(mBody);
	}
}

void BulletComponent::addCollisionListener(PhysicsCollisionListener *listener){
	mListeners.push_back(listener);
}

void BulletComponent::removeCollisionListener(PhysicsCollisionListener *listener){
	mListeners.erase(stlit::remove(mListeners.begin(),mListeners.end(),listener),mListeners.end());
}

void BulletComponent::setPosition(const Vector3 &position){
	if((mBody->getCollisionFlags() & btCollisionObject::CF_KINEMATIC_OBJECT)!=0){
		mKinematicPosition=position;
	}
	else{
		btTransform transform=mBody->getWorldTransform();
		btVector3 origin;
		setVector3(origin,position);
		transform.setOrigin(origin);
		mBody->setWorldTransform(transform);
	}
}

void BulletComponent::setOrientation(const Quaternion &orientation){
	if((mBody->getCollisionFlags() & btCollisionObject::CF_KINEMATIC_OBJECT)!=0){
		mKinematicOrientation=orientation;
	}
	else{
		btTransform transform=mBody->getWorldTransform();
		btQuaternion rotation;
		setQuaternion(rotation,orientation);
		transform.setRotation(rotation);
		mBody->setWorldTransform(transform);
	}
}

void BulletComponent::setMass(scalar mass){
	if(mass<0){
		mBody->setCollisionFlags(mBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		mBody->setActivationState(DISABLE_DEACTIVATION);
	}
	else{
		mBody->setCollisionFlags(mBody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
		mBody->setActivationState(0);

		btVector3 inertia;
		if(mShape!=NULL){
			mShape->calculateLocalInertia(mass,inertia);
		}

		mBody->setMassProps(mass,inertia);
	}
}

void BulletComponent::addForce(const Vector3 &force,const Vector3 &offset){
	btVector3 bforce,boffset;
	setVector3(bforce,force);
	setVector3(boffset,offset);
	mBody->applyForce(bforce,boffset);
	mBody->activate();
}

scalar BulletComponent::getMass() const{
	if((mBody->getCollisionFlags() & btCollisionObject::CF_KINEMATIC_OBJECT)!=0){
		return -Math::ONE;
	}
	else{
		scalar mass=mBody->getInvMass();
		if(mass>0){
			mass=Math::div(Math::ONE,mass);
		}
		return mass;
	}
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

	setMass(getMass()); // Update inertia
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
		if((mBody->getCollisionFlags() & btCollisionObject::CF_KINEMATIC_OBJECT)!=0){
			mKinematicPosition.set(translate);
			mKinematicOrientation.set(rotate);
		}

		mCurrentPosition.set(translate);
		mCurrentOrientation.set(rotate);

		btVector3 bmin,bmax;
		mBody->getAabb(bmin,bmax);
		Vector3 min,max;
		setVector3(min,bmin);setVector3(max,bmax);
		mBound->set(AABox(min,max));

		mWorldBound->transform(mBound,mParent->getWorldTransform());

		btTransform worldTrans;
		worldTrans.setIdentity();
		getWorldTransform(worldTrans);
		mBody->setWorldTransform(worldTrans);
	}
}

void BulletComponent::collision(const PhysicsCollision &collision){
	int i;
	for(i=0;i<mListeners.size();++i){
		mListeners[i]->collision(collision);
	}
}

void BulletComponent::getWorldTransform(btTransform& worldTrans) const{
	if(mParent==NULL){
		return;
	}

	if((mBody->getCollisionFlags() & btCollisionObject::CF_KINEMATIC_OBJECT)!=0){
		btVector3 origin;
		setVector3(origin,mKinematicPosition);
		btQuaternion rotation;
		setQuaternion(rotation,mKinematicOrientation);
		worldTrans.setOrigin(origin);
		worldTrans.setRotation(rotation);
	}
	else{
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
}

void BulletComponent::setWorldTransform(const btTransform& worldTrans){
	if(mParent==NULL){
		return;
	}

	setVector3(mCurrentPosition,worldTrans.getOrigin());
	setQuaternion(mCurrentOrientation,worldTrans.getRotation());
	mParent->getTransform()->setTranslateRotate(mCurrentPosition,mCurrentOrientation);
}

}
}
