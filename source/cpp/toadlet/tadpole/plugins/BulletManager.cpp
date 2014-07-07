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

BulletManager::BulletManager(Scene *scene):
	mWorld(NULL)
{
	Log::alert("BulletManager initializing");

	mScene=scene;

	btDefaultCollisionConfiguration *collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher *dispatcher = new btCollisionDispatcher(collisionConfiguration );
	btDbvtBroadphase *broadphase = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver *solver = new btSequentialImpulseConstraintSolver();

	mWorld=new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
}

BulletManager::~BulletManager(){
	delete mWorld;
}

PhysicsComponent *BulletManager::createPhysicsComponent(){
	return new BulletComponent(this);
}

void BulletManager::logicUpdate(int dt,int scope,Node *node){
	scalar fdt=Math::milliToReal(dt);

	mWorld->stepSimulation(fdt);
}

void BulletManager::frameUpdate(int dt,int scope,Node *node){
}

}
}
