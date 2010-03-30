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

#ifndef TOADLET_TADPOLE_HOPPARTICLECONTROLLER_H
#define TOADLET_TADPOLE_HOPPARTICLECONTROLLER_H

#include <toadlet/egg/Random.h>
#include <toadlet/hop/Collision.h>
#include <toadlet/tadpole/node/ParticleNode.h>

namespace toadlet{
namespace tadpole{

class HopScene;

// THe particle simulator should be replace with whatever sort of 'animation' effect we have that we can attach to a node to trigger on renderUpdate
//  Then it'd just be a thingie we attach to a particle node, telling it "use this node". and boom it would be simulated.

class TOADLET_API HopParticleSimulator:public node::ParticleNode::ParticleSimulator{
public:
	HopParticleSimulator(HopScene *scene,node::ParticleNode *node);
	virtual ~HopParticleSimulator();
	
	void setParticleLife(int life);
	void setParticleInitialVelocity(const Vector3 &velocity,const Vector3 &variance);
	void setSpawnAABox(const AABox &box);
	void setCoefficientOfGravity(scalar cog){mCoefficientOfGravity=cog;}
	void setLife(int life);
	void setLocalGravity(const Vector3 &gravity){mLocalGravity.set(gravity);}

	void updateParticles(int dt,const Matrix4x4 &transform);

protected:
	HopScene *mScene;
	node::ParticleNode *mParticleNode;
	int mParticleLife;
	bool mSpawnExistingParticles;
	AABox mSpawnAABox;
	int mLife;

	scalar mCoefficientOfGravity;
	Vector3 mParticleInitialVelocity;
	Vector3 mParticleInitialVariance;
	Vector3 mLocalGravity;
	egg::Random mRandom;

	Segment mSegment;
	hop::Collision mResult;

	Vector3 cache_updateParticles_velocity;
	Vector3 cache_updateParticles_direction;
};

}
}

#endif
