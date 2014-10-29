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

#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Scene.h>
#include "BulletManager.h"
#include "BulletComponent.h"

namespace toadlet{
namespace tadpole{

void physicsCallback(btDynamicsWorld *world, btScalar timeStep){
	((BulletManager*)world->getWorldUserInfo())->physicsUpdate(timeStep);
}

BulletManager::BulletManager(Scene *scene):
	mFixedDT(1.0/60.0),
	mWorld(NULL)
{
	Log::alert("BulletManager initializing");

	mScene=scene;

	btDefaultCollisionConfiguration *collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher *dispatcher = new btCollisionDispatcher(collisionConfiguration );
	btDbvtBroadphase *broadphase = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver *solver = new btSequentialImpulseConstraintSolver();

	mWorld=new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
	mWorld->setLatencyMotionStateInterpolation(true);
	mWorld->setInternalTickCallback(&physicsCallback,this,true); 

	setGravity(Vector3(0,0,-Math::fromMilli(9810)));
}

BulletManager::~BulletManager(){
	delete mWorld;
}

PhysicsComponent *BulletManager::createPhysicsComponent(){
	return new BulletComponent(this);
}

void BulletManager::addListener(PhysicsManagerListener *listener){
	mListeners.push_back(listener);
}

void BulletManager::removeListener(PhysicsManagerListener *listener){
	mListeners.erase(std::remove(mListeners.begin(),mListeners.end(),listener),mListeners.end());
}

void BulletManager::logicUpdate(int dt,int scope,Node *node){
	scalar fdt=Math::milliToReal(dt);

	int maxSteps=fdt/mFixedDT + 1;

	mWorld->stepSimulation(fdt,maxSteps,mFixedDT);
}

void BulletManager::frameUpdate(int dt,int scope,Node *node){
	float timeStep=mScene->getLogicFraction() * Math::fromMilli(mScene->getMinLogicDT());

	// Manually synchronize motion states, since there does not appear
	// to be a good way to force bullet to do it out of sync with its internal stepSimulation
	for(int i=0;i<mComponents.size();++i){
		btRigidBody *body=mComponents[i]->getBody();
		if(body!=NULL && body->isActive()){
			if(body->getMotionState()){
				btTransform interpolatedTransform;
				btTransformUtil::integrateTransform(body->getInterpolationWorldTransform(),
				body->getInterpolationLinearVelocity(),body->getInterpolationAngularVelocity(),
				timeStep,
				interpolatedTransform);
				body->getMotionState()->setWorldTransform(interpolatedTransform);
			}
		}
	}
}

void BulletManager::componentCreated(BulletComponent *component){
	mComponents.push_back(component);
}

void BulletManager::componentDestroyed(BulletComponent *component){
	mComponents.erase(std::remove(mComponents.begin(),mComponents.end(),component),mComponents.end());
}

void BulletManager::physicsUpdate(btScalar timeStep){
	int numManifolds=mWorld->getDispatcher()->getNumManifolds();
	for(int i=0;i<numManifolds;i++){
		btPersistentManifold *contactManifold=mWorld->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject *coA=static_cast<const btCollisionObject*>(contactManifold->getBody0());
		const btCollisionObject *coB=static_cast<const btCollisionObject*>(contactManifold->getBody1());
		BulletComponent *solidA=static_cast<BulletComponent*>(coA->getUserPointer());
		BulletComponent *solidB=static_cast<BulletComponent*>(coB->getUserPointer());

		int numContacts=contactManifold->getNumContacts();
		for(int j=0;j<numContacts;j++){
			const btManifoldPoint &pt = contactManifold->getContactPoint(j);
			if(pt.getDistance()<0.f){
				const btVector3& ptA=pt.getPositionWorldOnA();
				const btVector3& ptB=pt.getPositionWorldOnB();
				const btVector3& normalOnB=pt.m_normalWorldOnB;

				PhysicsCollision c;
				c.time=pt.getDistance();
				setVector3(c.point,ptA);
				setVector3(c.normal,normalOnB);
				c.velocity=solidA->getVelocity();
				c.collider=solidB->getParent();

				solidA->collision(c);

				setVector3(c.point,ptB);
				Math::neg(c.normal);
				c.velocity=solidB->getVelocity();
				c.collider=solidA->getParent();
				
				solidB->collision(c);
			}
		}
	}

	int i;
	for(i=0;i<mListeners.size();++i){
		mListeners[i]->physicsUpdate(timeStep);
	}
}

}
}
