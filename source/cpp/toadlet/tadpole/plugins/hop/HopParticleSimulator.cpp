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
#if 0
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/System.h>
#include <toadlet/tadpole/plugins/hop/HopParticleSimulator.h>
#include <toadlet/tadpole/plugins/hop/HopScene.h>
#include <toadlet/tadpole/plugins/hop/HopEntity.h>

namespace toadlet{
namespace tadpole{

HopParticleSimulator::HopParticleSimulator(HopScene *scene,ParticleNode *node):
	mScene(NULL),
	mParticleNode(NULL),
	mParticleLife(1000),
	mSpawnExistingParticles(true),
	//mSpawnAABox,
	mLife(-1),

	mCoefficientOfGravity(Math::ONE)
{
	mScene=scene;
	mParticleNode=node;

	mRandom.setSeed(System::mtime());
}

HopParticleSimulator::~HopParticleSimulator(){
}

void HopParticleSimulator::setParticleLife(int life){
	mParticleLife=life;
}

void HopParticleSimulator::setParticleInitialVelocity(const Vector3 &velocity,const Vector3 &variance){
	mParticleInitialVelocity.set(velocity);
	mParticleInitialVariance.set(variance);
}

void HopParticleSimulator::setSpawnAABox(const AABox &box){
	mSpawnAABox.set(box);
}

void HopParticleSimulator::setLife(int life){
	mLife=life;
}

void HopParticleSimulator::updateParticles(int dt,const Matrix4x4 &transform){
	int i;

	if(mLife>=0){
		mLife-=dt;
		if(mLife<=0){
			mParticleNode->setDestroyOnFinish(true);
			mLife=-1;
		}
	}

	bool alive=!mParticleNode->getDestroyOnFinish();
	bool allDead=!alive;

	scalar fdt=Math::fromMilli(dt);
	scalar pdt=Math::div(fdt,Math::fromMilli(mParticleLife));

	Vector3 &velocity=cache_updateParticles_velocity.reset();
	Vector3 &direction=cache_updateParticles_direction.reset();
	scalar length=0;
	for(i=mParticleNode->getNumParticles()-1;i>=0;--i){
		ParticleNode::Particle &p=mParticleNode->getParticle(i);
		if(p.age<Math::ONE && !mSpawnExistingParticles){
			allDead=false;
			p.age+=pdt;
			if(p.age>Math::ONE){
				p.age=Math::ONE;
			}

			Math::mul(velocity,mLocalGravity,Math::mul(mCoefficientOfGravity,fdt));
			p.vx+=velocity.x;p.vy+=velocity.y;p.vz+=velocity.z;
			direction.x=Math::mul(p.vx,fdt);direction.y=Math::mul(p.vy,fdt);direction.z=Math::mul(p.vz,fdt);
			mSegment.setStartDir(p.x,p.y,p.z,direction.x,direction.y,direction.z);
			mScene->getSimulator()->traceSegment(mResult,mSegment,-1,NULL);
			if(mResult.time>0){
				Math::mul(velocity,mResult.normal,2*(Math::mul(mResult.normal.x,p.vx)+Math::mul(mResult.normal.y,p.vy)+Math::mul(mResult.normal.z,p.vz)));
				p.vx-=velocity.x;p.vy-=velocity.y;p.vz-=velocity.z;
			}
			else{
				p.x+=direction.x;p.y+=direction.y;p.z+=direction.z;
			}
		}
		else if(alive || mSpawnExistingParticles){
			allDead=false;
			if(mSpawnExistingParticles==false || mParticleNode->hadAges()==false){
				p.age=0;
				p.visible=true;
			}

			if(mSpawnExistingParticles==false || mParticleNode->hadPoints()==false){
				p.x=transform.at(0,3);p.y=transform.at(1,3);p.z=transform.at(2,3);
			}
			p.x+=mRandom.nextScalar(mSpawnAABox.mins.x,mSpawnAABox.maxs.x);
			p.y+=mRandom.nextScalar(mSpawnAABox.mins.y,mSpawnAABox.maxs.y);
			p.z+=mRandom.nextScalar(mSpawnAABox.mins.z,mSpawnAABox.maxs.z);

			p.vx=mParticleInitialVelocity.x;p.vy=mParticleInitialVelocity.y;p.vz=mParticleInitialVelocity.z;
			p.vx+=mRandom.nextScalar(-mParticleInitialVariance.x,mParticleInitialVariance.x);
			p.vy+=mRandom.nextScalar(-mParticleInitialVariance.y,mParticleInitialVariance.y);
			p.vz+=mRandom.nextScalar(-mParticleInitialVariance.z,mParticleInitialVariance.z);
			velocity.set(p.vx,p.vy,p.vz);
			Math::mul(velocity,transform);
			p.vx=velocity.x;p.vy=velocity.y;p.vz=velocity.z;
		}
		if(mParticleNode->getOrientation()){
			length=Math::sqrt(Math::square(p.vx)+Math::square(p.vy)+Math::square(p.vz));
			if(length>0){
				p.ox=Math::div(p.vx,length);
				p.oy=Math::div(p.vy,length);
				p.oz=Math::div(p.vz,length);
			}
		}
	}

	mSpawnExistingParticles=false;
}

}
}
#endif