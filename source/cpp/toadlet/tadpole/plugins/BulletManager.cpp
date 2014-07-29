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
//				if(!body->isStaticOrKinematicObject()){
					btTransform interpolatedTransform;
					btTransformUtil::integrateTransform(body->getInterpolationWorldTransform(),
					body->getInterpolationLinearVelocity(),body->getInterpolationAngularVelocity(),
					timeStep,
					interpolatedTransform);
					body->getMotionState()->setWorldTransform(interpolatedTransform);
				}
	//		}
		}
	}
}

void BulletManager::physicsUpdate(btScalar timeStep){
	int i;
	for(i=0;i<mListeners.size();++i){
		mListeners[i]->physicsUpdate(timeStep);
	}
}

}
}
