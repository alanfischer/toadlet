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

package com.lightningtoads.toadlet.tadpole.plugins.hop;

#include <com/lightningtoads/toadlet/tadpole/Types.h>

import com.lightningtoads.toadlet.hop.*;
import com.lightningtoads.toadlet.tadpole.*;
import com.lightningtoads.toadlet.tadpole.entity.*;
import java.util.Random;

#if defined(TOADLET_FIXED_POINT)
	#include <com/lightningtoads/toadlet/egg/mathfixed/Inline.h>

	import static com.lightningtoads.toadlet.egg.mathfixed.Math.*;

	#define mul_ss(s1,s2)	TOADLET_MUL_XX(s1,s2)
	#define div_ss(s1,s2)	TOADLET_DIV_XX(s1,s2)
#else
	import static com.lightningtoads.toadlet.egg.math.Math.*;

	#define mul_ss(s1,s2)	(s1*s2)
	#define div_ss(s1,s2)	(s1/s2)
#endif

public class HopParticleSimulator implements ParticleEntity.ParticleSimulator{
	public HopParticleSimulator(HopScene scene,ParticleEntity entity){
		mScene=scene;
		mParticleEntity=entity;

		mRandom.setSeed(System.currentTimeMillis());
	}
	
	public void setParticleLife(int life){mParticleLife=life;}
	
	public void setParticleInitialVelocity(Vector3 velocity,Vector3 variance){
		mParticleInitialVelocity.set(velocity);
		mParticleInitialVariance.set(variance);
	}
	
	public void setSpawnAABox(AABox box){mSpawnAABox.set(box);}
	
	public void setCoefficientOfGravity(scalar cog){mCoefficientOfGravity=cog;}

	public void updateParticles(int dt,Matrix4x4 transform){
		int i;

		boolean alive=!mParticleEntity.getDestroyOnFinish();
		boolean allDead=!alive;

		scalar fdt=fromMilli(dt);
		scalar pdt=div_ss(fdt,fromMilli(mParticleLife));

		Vector3 velocity=cache_updateParticles_velocity.reset();
		Vector3 direction=cache_updateParticles_direction.reset();
		scalar length=0;
		for(i=mParticleEntity.getNumParticles()-1;i>=0;--i){
			ParticleEntity.Particle p=mParticleEntity.getParticle(i);
			if(p.age<ONE && !mSpawnExistingParticles){
				allDead=false;
				p.age+=pdt;
				if(p.age>ONE){
					p.age=ONE;
				}

				mul(velocity,mScene.getGravity(),mul_ss(mCoefficientOfGravity,fdt));
				p.vx+=velocity.x;p.vy+=velocity.y;p.vz+=velocity.z;
				direction.x=mul_ss(p.vx,fdt);direction.y=mul_ss(p.vy,fdt);direction.z=mul_ss(p.vz,fdt);
				// Removed for now for speed concerns
				//mSegment.setStartDir(p.x,p.y,p.z,direction.x,direction.y,direction.z);
				//mScene.getSimulator().traceSegment(mResult,mSegment,-1,null);
				//if(mResult.time>0){
				//	mul(velocity,mResult.normal,2*(mul_ss(mResult.normal.x,p.vx)+mul_ss(mResult.normal.y,p.vy)+mul_ss(mResult.normal.z,p.vz)));
				//	p.vx-=velocity.x;p.vy-=velocity.y;p.vz-=velocity.z;
				//}
				//else{
					p.x+=direction.x;p.y+=direction.y;p.z+=direction.z;
				//}
			}
			else if(alive || mSpawnExistingParticles){
				allDead=false;
				if(mSpawnExistingParticles==false || mParticleEntity.hadAges()==false){
					p.age=0;
					p.visible=true;
				}

				if(mSpawnExistingParticles==false || mParticleEntity.hadPoints()==false){
					p.x=transform.at(0,3);p.y=transform.at(1,3);p.z=transform.at(2,3);
				}

				p.vx=mParticleInitialVelocity.x;p.vy=mParticleInitialVelocity.y;p.vz=mParticleInitialVelocity.z;
				#if defined(TOADLET_FIXED_POINT)
					// We add 1 to the nextInt in java because a nextInt(0) throws an exception
					p.x+=mRandom.nextInt(mSpawnAABox.maxs.x-mSpawnAABox.mins.x+1)+mSpawnAABox.mins.x;
					p.y+=mRandom.nextInt(mSpawnAABox.maxs.y-mSpawnAABox.mins.y+1)+mSpawnAABox.mins.y;
					p.z+=mRandom.nextInt(mSpawnAABox.maxs.z-mSpawnAABox.mins.z+1)+mSpawnAABox.mins.z;

					p.vx+=mRandom.nextInt(mParticleInitialVariance.x*2+1)-mParticleInitialVariance.x;
					p.vy+=mRandom.nextInt(mParticleInitialVariance.y*2+1)-mParticleInitialVariance.y;
					p.vz+=mRandom.nextInt(mParticleInitialVariance.z*2+1)-mParticleInitialVariance.z;
				#else
					p.x+=mRandom.nextFloat()*(mSpawnAABox.maxs.x-mSpawnAABox.mins.x)+mSpawnAABox.mins.x;
					p.y+=mRandom.nextFloat()*(mSpawnAABox.maxs.y-mSpawnAABox.mins.y)+mSpawnAABox.mins.y;
					p.z+=mRandom.nextFloat()*(mSpawnAABox.maxs.z-mSpawnAABox.mins.z)+mSpawnAABox.mins.z;

					p.vx+=mRandom.nextFloat()*mParticleInitialVariance.x*2-mParticleInitialVariance.x;
					p.vy+=mRandom.nextFloat()*mParticleInitialVariance.y*2-mParticleInitialVariance.y;
					p.vz+=mRandom.nextFloat()*mParticleInitialVariance.z*2-mParticleInitialVariance.z;
				#endif
				velocity.set(p.vx,p.vy,p.vz);
				mul(velocity,transform);
				p.vx=velocity.x;p.vy=velocity.y;p.vz=velocity.z;
			}
			if(mParticleEntity.getOrientation()){
				length=sqrt(mul_ss(p.vx,p.vx)+mul_ss(p.vy,p.vy)+mul_ss(p.vz,p.vz));
				if(length>0){
					p.ox=div_ss(p.vx,length);
					p.oy=div_ss(p.vy,length);
					p.oz=div_ss(p.vz,length);
				}
			}
		}

		mSpawnExistingParticles=false;

		if(allDead){
			mParticleEntity.destroyNextLogicFrame();
		}
	}

	public Vector3 cacheVector3_1=new Vector3();
	public Vector3 cacheVector3_2=new Vector3();

	protected HopScene mScene=null;
	protected ParticleEntity mParticleEntity=null;
	protected int mParticleLife=1000;
	protected boolean mSpawnExistingParticles=true;
	protected AABox mSpawnAABox=new AABox();

	protected scalar mCoefficientOfGravity=ONE;
	protected Vector3 mParticleInitialVelocity=new Vector3();
	protected Vector3 mParticleInitialVariance=new Vector3();
	protected Random mRandom=new Random();

	protected Segment mSegment=new Segment();
	protected Collision mResult=new Collision();

	Vector3 cache_updateParticles_velocity=new Vector3();
	Vector3 cache_updateParticles_direction=new Vector3();
}
