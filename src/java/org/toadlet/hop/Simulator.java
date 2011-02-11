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

package org.toadlet.hop;

#include <org/toadlet/hop/Types.h>

import org.toadlet.egg.*;
import org.toadlet.egg.Error;
#if defined(TOADLET_FIXED_POINT)
	#include <org/toadlet/egg/mathfixed/Inline.h>

	import static org.toadlet.egg.mathfixed.Math.*;

	#define add_vvv(r,v1,v2) r.x=v1.x+v2.x;r.y=v1.y+v2.y;r.z=v1.z+v2.z
	#define add_vv(r,v)		r.x+=v.x;r.y+=v.y;r.z+=v.z
	#define add_av(r,v)		r.mins.x+=v.x;r.mins.y+=v.y;r.mins.z+=v.z;r.maxs.x+=v.x;r.maxs.y+=v.y;r.maxs.z+=v.z
	#define add_sv(r,v)		r.origin.x+=v.x;r.origin.y+=v.y;r.origin.z+=v.z
	#define add_cv(r,v)		r.origin.x+=v.x;r.origin.y+=v.y;r.origin.z+=v.z

	#define sub_vv(r,v)		r.x-=v.x;r.y-=v.y;r.z-=v.z
	#define sub_vvv(r,v1,v2) r.x=v1.x-v2.x;r.y=v1.y-v2.y;r.z=v1.z-v2.z

	#define mul_ss(s1,s2)	TOADLET_MUL_XX(s1,s2)
	#define mul_vvs(r,v,s)	r.x=TOADLET_MUL_XX(v.x,s);r.y=TOADLET_MUL_XX(v.y,s);r.z=TOADLET_MUL_XX(v.z,s)
	#define mul_vs(r,s)		r.x=TOADLET_MUL_XX(r.x,s);r.y=TOADLET_MUL_XX(r.y,s);r.z=TOADLET_MUL_XX(r.z,s)

	// Don't inline div since our implementation would need a temporary
	
	#define neg(r)			r.x=-r.x;r.y=-r.y;r.z=-r.z

	#define dot(v1,v2)		(TOADLET_MUL_XX(v1.x,v2.x)+TOADLET_MUL_XX(v1.y,v2.y)+TOADLET_MUL_XX(v1.z,v2.z))

	#define testIntersection_aa(box1,box2) (!(box1.mins.x>box2.maxs.x || box1.mins.y>box2.maxs.y || box1.mins.z>box2.maxs.z || box2.mins.x>box1.maxs.x || box2.mins.y>box1.maxs.y || box2.mins.z>box1.maxs.z))
	#define testIntersection_sa(sphere,box) testIntersection(sphere,box)
#else
	import static org.toadlet.egg.math.Math.*;

	#define add_vvv(r,v1,v2) r.x=v1.x+v2.x;r.y=v1.y+v2.y;r.z=v1.z+v2.z
	#define add_vv(r,v)		r.x+=v.x;r.y+=v.y;r.z+=v.z
	#define add_av(r,v)		r.mins.x+=v.x;r.mins.y+=v.y;r.mins.z+=v.z;r.maxs.x+=v.x;r.maxs.y+=v.y;r.maxs.z+=v.z
	#define add_sv(r,v)		r.origin.x+=v.x;r.origin.y+=v.y;r.origin.z+=v.z
	#define add_cv(r,v)		r.origin.x+=v.x;r.origin.y+=v.y;r.origin.z+=v.z

	#define sub_vv(r,v)		r.x-=v.x;r.y-=v.y;r.z-=v.z
	#define sub_vvv(r,v1,v2) r.x=v1.x-v2.x;r.y=v1.y-v2.y;r.z=v1.z-v2.z

	#define mul_ss(s1,s2)	(s1*s2)
	#define mul_vvs(r,v,s)	r.x=(v.x*s);r.y=(v.y*s);r.z=(v.z*s)
	#define mul_vs(r,s)		r.x=(r.x*s);r.y=(r.y*s);r.z=(r.z*s)

	// Don't inline div since our implementation would need a temporary

	#define neg(r)			r.x=-r.x;r.y=-r.y;r.z=-r.z

	#define dot(v1,v2)		((v1.x*v2.x)+(v1.y*v2.y)+(v1.z*v2.z))

	#define testIntersection_aa(box1,box2) (!(box1.mins.x>box2.maxs.x || box1.mins.y>box2.maxs.y || box1.mins.z>box2.maxs.z || box2.mins.x>box1.maxs.x || box2.mins.y>box1.maxs.y || box2.mins.z>box1.maxs.z))
	#define testIntersection_sa(sphere,box) testIntersection(sphere,box)
#endif

public final class Simulator{
	public enum Integrator{
		EULER,
		VERLET,
		IMPROVED,
		HEUN,
		RUNGE_KUTTA,
	};

	public Simulator(){
		setGravity(new Vector3(0,0,-fromMilli(9810)));
		#if defined(TOADLET_FIXED_POINT)
			setEpsilonBits(4);
			setMaxVelocityComponent(intToFixed(104)); // A vector above 104,104,104 can have its length overflow
			setMaxForceComponent(intToFixed(104));
			setDeactivateSpeed(1<<8); // Some default
			setDeactivateCount(4); // 4 frames
		#else
			setEpsilon(.0001f);
			setMaxVelocityComponent(1000);
			setMaxForceComponent(1000);
			setDeactivateSpeed(mEpsilon*2); // Twice epsilon
			setDeactivateCount(4); // 4 frames
		#endif
	}

	#if defined(TOADLET_FIXED_POINT)
		public void setEpsilonBits(int epsilonBits){
			mEpsilonBits=epsilonBits;
			mEpsilon=1<<mEpsilonBits;
			mHalfEpsilon=mEpsilon>>1;
			mQuarterEpsilon=mEpsilon>>2;
		}
		
		public int getEpsilonBits(){return mEpsilonBits;}
	#else
		public void setEpsilon(scalar epsilon){
			mEpsilon=epsilon;
			mOneOverEpsilon=1.0f/mEpsilon;
			mHalfEpsilon=mEpsilon*0.5f;
			mQuarterEpsilon=mEpsilon*0.25f;
		}

		public scalar getEpsilon(){return mEpsilon;}
	#endif

	public void setIntegrator(Integrator integrator){mIntegrator=integrator;}
	public Integrator getIntegrator(){return mIntegrator;}

	public void setMaxVelocityComponent(scalar maxVelocityComponent){mMaxVelocityComponent=maxVelocityComponent;}
	public scalar getMaxVelocityComponent(){return mMaxVelocityComponent;}

	public void setMaxForceComponent(scalar maxForceComponent){mMaxForceComponent=maxForceComponent;}
	public scalar getMaxForceComponent(){return mMaxForceComponent;}

	public void setFluidVelocity(Vector3 fluidVelocity){mFluidVelocity.set(fluidVelocity);}
	public Vector3 getFluidVelocity(){return mFluidVelocity;}

	public void setGravity(Vector3 gravity){
		mGravity.set(gravity);
		int i;
		for(i=0;i<mNumSolids;++i){
			mSolids[i].activate();
		}
	}

	public Vector3 getGravity(){return mGravity;}

	public void setManager(Manager manager){mManager=manager;}
	public Manager getManager(){return mManager;}

	public void setMicroCollisionThreshold(scalar threshold){mMicroCollisionThreshold=threshold;}
	public scalar getMicroCollisionThreshold(){return mMicroCollisionThreshold;}

	public void setDeactivateSpeed(scalar speed){mDeactivateSpeed=speed;}
	public void setDeactivateCount(int count){mDeactivateCount=count;}

	public void addSolid(Solid solid){
		int i;
		for(i=0;i<mNumSolids;++i){
			if(mSolids[i]==solid){return;}
		}
	
		if(mSolids.length<mNumSolids+1){
			Solid[] solids=new Solid[mNumSolids+1];
			System.arraycopy(mSolids,0,solids,0,mSolids.length);
			mSolids=solids;
		}
		mSolids[mNumSolids++]=solid;

		solid.internal_setSimulator(this);
		solid.activate();

		if(mSpacialCollection.length<mNumSolids){
			mSpacialCollection=new Solid[mNumSolids];
		}
	}

	public void removeSolid(Solid solid){
		// We can't touch anything if we're not in the simulation
		// This will help in some logic code
		solid.mTouching=null;
		solid.mTouched1=null;
		solid.mTouched2=null;

		int i;
		for(i=0;i<mNumSolids;++i){
			Solid s=mSolids[i];
			if(s.mTouching==solid){
				s.mTouching=null;
			}
			if(s.mTouched1==solid){
				s.mTouched1=null;
			}
			if(s.mTouched2==solid){
				s.mTouched2=null;
			}
		}

		// In case a solid is removed in the collision code, null it out
		if(mReportingCollisions){
			for(i=0;i<mCollisions.length;++i){
				Collision c=mCollisions[i];
				if(c.collider==solid){
					c.collider=null;
				}
				if(c.collidee==solid){
					c.collidee=null;
				}
			}
		}

		solid.internal_setSimulator(null);

		for(i=0;i<mNumSolids;++i){
			if(mSolids[i]==solid)break;
		}
		if(i!=mNumSolids){
			System.arraycopy(mSolids,i+1,mSolids,i,mNumSolids-(i+1));
			mSolids[--mNumSolids]=null;
		}
	}

	public int getNumSolids(){return mNumSolids;}
	public Solid getSolid(int i){return mSolids[i];}

	public void addConstraint(Constraint constraint){
		int i;
		for(i=0;i<mNumConstraints;++i){
			if(mConstraints[i]==constraint){return;}
		}
	
		if(mConstraints.length<mNumConstraints+1){
			Constraint[] constraints=new Constraint[mNumConstraints+1];
			System.arraycopy(mConstraints,0,constraints,0,mConstraints.length);
			mConstraints=constraints;
		}
		mConstraints[mNumConstraints++]=constraint;

		constraint.internal_setSimulator(this);
		constraint.activate();
	}

	public void removeConstraint(Constraint constraint){
		constraint.internal_setSimulator(null);

		int i;
		for(i=0;i<mNumConstraints;++i){
			if(mConstraints[i]==constraint)break;
		}
		if(i!=mNumConstraints){
			System.arraycopy(mConstraints,i+1,mConstraints,i,mNumConstraints-(i+1));
			mConstraints[--mNumConstraints]=null;
		}
	}
	
	public void update(int dt){ // milliseconds
		Vector3 oldPosition=cache_update_oldPosition;
		Vector3 newPosition=cache_update_newPosition;
		Vector3 velocity=cache_update_velocity;
		Vector3 temp=cache_update_temp;
		Vector3 t=cache_update_t;
		Vector3 leftOver=cache_update_leftOver;
		Vector3 dx1=cache_update_dx1;
		Vector3 dx2=cache_update_dx2;
		Vector3 dv1=cache_update_dv1;
		Vector3 dv2=cache_update_dv2;
		Segment path=cache_update_path;
		scalar cor=0,impulse=0,oneOverMass=0,oneOverHitMass=0;
		int loop=0;
		Solid solid=null,hitSolid=null;
		Collision c=cache_update_c.reset();
		int numCollisions=0;
		int i,j;

		scalar fdt=fromMilli(dt);
		scalar hfdt=fdt/2;
		scalar qfdt=fdt/4;
		scalar sfdt=fdt/6;
		scalar ttfdt=fdt*2/3;

		if(mManager!=null){
			mManager.preUpdate(dt,fdt);
		}

		for(i=0;i<mNumSolids;i++){
			solid=mSolids[i];

			if(solid.mActive==false){
				continue;
			}

			if(solid.mDoUpdateCallback && mManager!=null){
				mManager.preUpdate(solid,dt,fdt);
			}

			if(solid.mLocalGravityOverride==false){
				solid.mLocalGravity.set(mGravity);
			}

			oldPosition.set(solid.mPosition);

			// The cheapest, fastest, and worst integrator, it's 1st order
			//  It may have trouble settling, so it is not recommended
			if(mIntegrator==Integrator.EULER){
				/*
					integrationStep(solid,oldPosition,solid.mVelocity,ZERO_VECTOR3,ZERO_VECTOR3,fdt,dx1,dv1);
					newPosition=oldPosition+dx1*fdt;
					velocity=solid.mVelocity+dv1*fdt;
				*/

				integrationStep(solid,oldPosition,solid.mVelocity,ZERO_VECTOR3,ZERO_VECTOR3,fdt,dx1,dv1);

				mul_vvs(newPosition,dx1,fdt);
				add_vv(newPosition,oldPosition);
				mul_vvs(velocity,dv1,fdt);
				add_vv(velocity,solid.mVelocity);
			}
			// Verlet is unimplemented yet
			else if(mIntegrator==Integrator.VERLET){
			}
			// Improved is 2nd order and a nice balance between speed and accuracy
			else if(mIntegrator==Integrator.IMPROVED){
				/*
					integrationStep(solid,oldPosition,solid.mVelocity,ZERO_VECTOR3,ZERO_VECTOR3,fdt,dx1,dv1);
					integrationStep(solid,oldPosition,solid.mVelocity,dx1,dv1,fdt,dx2,dv2)
					newPosition=oldPosition+0.5*fdt*(dx1+dx2);
					velocity=solid.mVelocity+0.5*fdt*(dv1+dv2);
				*/

				integrationStep(solid,oldPosition,solid.mVelocity,ZERO_VECTOR3,ZERO_VECTOR3,fdt,dx1,dv1);

				newPosition.set(dx1);
				velocity.set(dv1);

				integrationStep(solid,oldPosition,solid.mVelocity,dx1,dv1,fdt,dx2,dv2);

				add_vv(newPosition,dx2);
				mul_vs(newPosition,hfdt);
				add_vv(newPosition,oldPosition);

				add_vv(velocity,dv2);
				mul_vs(velocity,hfdt);
				add_vv(velocity,solid.mVelocity);
			}
			// Heun's method is 2nd order and so has the same speed as Improved, but it biases towards the 2nd portion of the integration step, and thus may be more stable
			else if(mIntegrator==Integrator.HEUN){
				/*
					integrationStep(solid,oldPosition,solid.mVelocity,ZERO_VECTOR3,ZERO_VECTOR3,fdt,dx1,dv1);
					integrationStep(solid,oldPosition,solid.mVelocity,dx1,dv1,2.0f/3.0f*fdt,dx2,dv2);
					newPosition=oldPosition+0.25f*fdt*(dx1+3.0f*dx2);
					velocity=solid.mVelocity+0.25f*fdt*(dv1+3.0f*dv2);
				*/

				integrationStep(solid,oldPosition,solid.mVelocity,ZERO_VECTOR3,ZERO_VECTOR3,fdt,dx1,dv1);

				newPosition.set(dx1);
				velocity.set(dv1);

				integrationStep(solid,oldPosition,solid.mVelocity,dx1,dv1,ttfdt,dx2,dv2);

				mul_vs(dx2,THREE);
				add_vv(newPosition,dx2);
				mul_vs(newPosition,qfdt);
				add_vv(newPosition,oldPosition);

				mul_vs(dv2,THREE);
				add_vv(velocity,dv2);
				mul_vs(velocity,qfdt);
				add_vv(velocity,solid.mVelocity);
			}
			// Runge Kutta is 4th order, decently fast, and very stable for larger step sizes
			else if(mIntegrator==Integrator.RUNGE_KUTTA){
				/*
					integrationStep(solid,oldPosition,solid.mVelocity,ZERO_VECTOR3,ZERO_VECTOR3,fdt,dx1,dv1);
					integrationStep(solid,oldPosition,solid.mVelocity,dx1,dv1,0.5f*fdt,dx2,dv2);
					integrationStep(solid,oldPosition,solid.mVelocity,dx2,dv2,0.5f*fdt,dx3,dv3);
					integrationStep(solid,oldPosition,solid.mVelocity,dx3,dv3,fdt,dx4,dv4);
					newPosition=oldPosition+1.0f/6.0f*fdt*(dx1+2.0f*(dx2+dx3)+dx4);
					velocity=solid.mVelocity+1.0f/6.0f*fdt*(dv1+2.0f*(dv2+dv3)+dv4);
				*/

				integrationStep(solid,oldPosition,solid.mVelocity,ZERO_VECTOR3,ZERO_VECTOR3,fdt,dx1,dv1);

				newPosition.set(dx1);
				velocity.set(dv1);

				integrationStep(solid,oldPosition,solid.mVelocity,dx1,dv1,hfdt,dx2,dv2);

				mul_vvs(temp,dx2,TWO);
				add_vv(newPosition,temp);
				mul_vvs(temp,dv2,TWO);
				add_vv(velocity,temp);

				integrationStep(solid,oldPosition,solid.mVelocity,dx2,dv2,hfdt,dx1,dv1);

				mul_vvs(temp,dx1,TWO);
				add_vv(newPosition,temp);
				mul_vvs(temp,dv1,TWO);
				add_vv(velocity,temp);

				integrationStep(solid,oldPosition,solid.mVelocity,dx1,dv1,fdt,dx2,dv2);

				add_vv(newPosition,dx2);
				mul_vs(newPosition,sfdt);
				add_vv(newPosition,oldPosition);

				add_vv(velocity,dv2);
				mul_vs(velocity,sfdt);
				add_vv(velocity,solid.mVelocity);
			}

			// Clean up
			capVector3(velocity,mMaxVelocityComponent);
			solid.mVelocity.set(velocity);
			solid.clearForce();

			boolean first=true;
			boolean skip=false;

			if(solid.mDoUpdateCallback && mManager!=null){
				mManager.intraUpdate(solid,dt,fdt);
			}

			// Collect all possible solids in the whole movement area
			if(solid.mCollideWithBits!=0){
				#if defined(TOADLET_USE_CLAMP)
					clampPosition(oldPosition);
					clampPosition(newPosition);
				#endif

				sub_vvv(temp,newPosition,oldPosition);
				if(toSmall(temp)){
					newPosition.set(oldPosition);
					skip=true;
				}
				else{
					if(temp.x<0){temp.x=-temp.x;}
					if(temp.y<0){temp.y=-temp.y;}
					if(temp.z<0){temp.z=-temp.z;}

					scalar m=temp.x;
					if(temp.y>m){m=temp.y;}
					if(temp.z>m){m=temp.z;}
					m+=mEpsilon; // Move it out by epsilon, since we haven't clamped yet

					AABox box=cache_update_box.set(solid.mLocalBound);

					add_av(box,newPosition);
					box.mins.x-=m;
					box.mins.y-=m;
					box.mins.z-=m;
					box.maxs.x+=m;
					box.maxs.y+=m;
					box.maxs.z+=m;

					mNumSpacialCollection=findSolidsInAABox(box,mSpacialCollection,mSpacialCollection.length);
				}
			}

			// Loop to use up available momentum
			loop=0;
			while(skip==false){
				if(first==true){
					first=false;
				}
				else{
					#if defined(TOADLET_USE_CLAMP)
						clampPosition(oldPosition);
						clampPosition(newPosition);
					#endif

					sub_vvv(temp,newPosition,oldPosition);
					if(toSmall(temp)){
						newPosition.set(oldPosition);
						break;
					}
				}

				path.setStartEnd(oldPosition,newPosition);
				traceSolid(c,solid,path);

				if(c.time>=0){
					/// @todo: Improve this Inside logic
					if(c.time==0 && c.collider!=null && c.collider.mMass==Solid.INFINITE_MASS){
						solid.mInside=true;
					}
					else{
						solid.mInside=false;
					}

					// If its a valid collision, and someone is listening, then store it
					if(c.collider!=solid.mTouching &&
						(solid.mCollisionListener!=null || c.collider.mCollisionListener!=null))
					{
						c.collidee=solid;
						sub_vvv(c.velocity,solid.mVelocity,c.collider.mVelocity);

						if(mCollisions.length<numCollisions+1){
							Collision[] collisions=new Collision[numCollisions+1];
							System.arraycopy(mCollisions,0,collisions,0,mCollisions.length);
							mCollisions=collisions;
							mCollisions[numCollisions]=new Collision();
						}

						mCollisions[numCollisions].set(c);
						numCollisions++;
					}
					hitSolid=c.collider;

					// Conservation of momentum
					if(solid.mCoefficientOfRestitutionOverride){
						cor=solid.mCoefficientOfRestitution;
					}
					else{
						cor=(solid.mCoefficientOfRestitution+hitSolid.mCoefficientOfRestitution)/2;
					}

					sub_vvv(temp,hitSolid.mVelocity,solid.mVelocity);

					// Attempt to detect a microcollision
					if(dot(temp,c.normal)<mMicroCollisionThreshold){
						cor=0;
					}

					scalar numerator=mul_ss(ONE+cor,dot(temp,c.normal));

					// Temp stores the velocity change on hitSolid
					temp.reset();

					if(solid.mMass!=0 && hitSolid.mMass!=0){
						oneOverMass=solid.mInvMass;
						oneOverHitMass=hitSolid.mInvMass;

						// Check to make sure its not two infinite mass solids interacting
						if(oneOverMass+oneOverHitMass!=0){
							impulse=div(numerator,oneOverMass+oneOverHitMass);
						}
						else{
							impulse=0;
						}

						if(solid.mMass!=Solid.INFINITE_MASS){
							/*
								solid.mVelocity+=(c.normal*impulse)*oneOverMass;
							*/

							mul_vvs(t,c.normal,impulse);
							mul_vs(t,oneOverMass);
							add_vv(solid.mVelocity,t);
						}
						if(hitSolid.mMass!=Solid.INFINITE_MASS){
							if(solid.mMass==Solid.INFINITE_MASS){
								// Infinite mass trains assume a CoefficientOfRestitution of 1 
								// for the hitSolid calculation to avoid having gravity jam objects against them 
								/*
									temp=c.normal*2.0f*dot(hitSolid.mVelocity-solid.mVelocity,c.normal);
								*/

								sub_vvv(temp,hitSolid.mVelocity,solid.mVelocity);
								mul(temp,c.normal,dot(temp,c.normal)/2);
							}
							else{
								/*
									temp=(c.normal*impulse)*oneOverHitMass;
								*/

								mul_vvs(temp,c.normal,impulse);
								mul_vs(temp,oneOverHitMass);
							}
						}
					}
					else if(hitSolid.mMass==0){
						mul_vvs(temp,c.normal,numerator);
					}
					else if(solid.mMass==0){
						mul_vvs(t,c.normal,numerator);
						add_vv(solid.mVelocity,t);
					}

					// Only affect hitSolid if hitSolid would have collided with solid.
					if(	(hitSolid.mCollideWithBits&solid.mCollisionBits)!=0 &&
						(Math.abs(temp.x)>=mDeactivateSpeed || Math.abs(temp.y)>=mDeactivateSpeed || Math.abs(temp.z)>=mDeactivateSpeed))
					{
						hitSolid.activate();
						sub_vv(hitSolid.mVelocity,temp);
					}

					// Calculate offset vector, and then resulting position
					#if defined(TOADLET_USE_CLAMP)
						clampPosition(c.point);
					#endif

					// Offset our point slightly from the wall
					temp.set(c.normal);
					convertToEpsilonOffset(temp);
					add(temp,c.point);

					// Calculate left over amount
					sub_vvv(leftOver,newPosition,temp);

					// Touching code
					solid.mTouched2=solid.mTouched1;
					solid.mTouched2Normal.set(solid.mTouched1Normal);
					if(solid.mTouched1==c.collider){
						solid.mTouching=c.collider;
						solid.mTouchingNormal.set(c.normal);
					}
					else{
						solid.mTouched1=c.collider;
						solid.mTouched1Normal.set(c.normal);
						solid.mTouching=null;
					}

					if(toSmall(leftOver)){
						newPosition.set(temp);
						break;
					}
					else if(loop>4){
						// We keep hitting something, so just break out
						newPosition.set(temp);
						break;
					}
					else{
						// Calculate new destinations from coefficient of restitution applied to velocity
						if(normalizeCarefully(velocity,solid.mVelocity,mEpsilon)==false){
							newPosition.set(temp);
							break;
						}
						else{
							/*
								oldPosition=temp;
								velocity*=length(leftOver);
								velocity-=c.normal*dot(velocity,c.normal);
								newPosition=oldPosition+velocity;
							*/

							oldPosition.set(temp);
							mul(velocity,length(leftOver));
							mul(temp,c.normal,dot(velocity,c.normal));
							sub_vv(velocity,temp);
							add_vvv(newPosition,oldPosition,velocity);
						}
					}
				}
				else{
					break;
				}

				loop++;
			}

			// Touching code
			if(c.time<0 && loop==0){
				solid.mTouching=null;
				solid.mTouched1=null;
				solid.mTouched2=null;
			}

			if(Math.abs(newPosition.x-solid.mPosition.x)<mDeactivateSpeed && Math.abs(newPosition.y-solid.mPosition.y)<mDeactivateSpeed && Math.abs(newPosition.z-solid.mPosition.z)<mDeactivateSpeed){
				solid.mDeactivateCount++;
				if(solid.mDeactivateCount>mDeactivateCount){
					for(j=solid.mNumConstraints-1;j>=0;--j){
						Constraint constraint=solid.mConstraints[j];
						Solid startSolid=constraint.mStartSolid;
						Solid endSolid=constraint.mEndSolid;
						if(startSolid!=solid){
							if(startSolid.mActive==true && startSolid.mDeactivateCount<=mDeactivateCount){
								break;
							}
						}
						else if(endSolid!=null){
							if(endSolid.mActive==true && endSolid.mDeactivateCount<=mDeactivateCount){
								break;
							}
						}
					}
					if(j<0){
						solid.mActive=false;
						solid.mDeactivateCount=0;
					}
				}
			}
			else{
				solid.mDeactivateCount=0;
			}

			solid.setPositionNoActivate(newPosition);

			if(solid.mDoUpdateCallback && mManager!=null){
				mManager.postUpdate(solid,dt,fdt);
			}
		}

		// Now report all collisions to both solids
		mReportingCollisions=true;
		for(i=0;i<numCollisions;++i){
			Collision col=mCollisions[i];

			// We must do these as 2 separate ifs, because the collision functions may destroy the solids they are hitting
			if(col.collidee!=null){
				CollisionListener listener=col.collidee.mCollisionListener;
				if(listener!=null && col.collider!=null && (col.collidee.mCollideWithBits&col.collider.mCollisionBits)!=0){
					listener.collision(col);
				}
			}
			if(col.collider!=null){
				CollisionListener listener=col.collider.mCollisionListener;
				if(listener!=null && col.collidee!=null && (col.collider.mCollideWithBits&col.collidee.mCollisionBits)!=0){
					Solid s=col.collider;
					col.collider=col.collidee;
					col.collidee=s;

					neg(col.normal);
					neg(col.velocity);

					listener.collision(col);
				}
			}
		}
		mReportingCollisions=false;

		if(mManager!=null){
			mManager.postUpdate(dt,fdt);
		}
	}

	public int findSolidsInAABox(AABox box,Solid solids[],int maxSolids){
		Solid s=null;

		int i,j=0;
		for(i=0;i<mNumSolids;++i){
			s=mSolids[i];
			if(testIntersection_aa(box,s.mWorldBound)){
				if(j<maxSolids){
					solids[j]=s;
				}
				j++;
			}
		}

		return j;
	}

	public int findSolidsInSphere(Sphere sphere,Solid solids[],int maxSolids){
		Solid s=null;

		int i,j=0;
		for(i=0;i<mNumSolids;++i){
			s=mSolids[i];
			if(testIntersection_sa(sphere,s.mWorldBound)){
				if(j<maxSolids){
					solids[j]=s;
				}
				j++;
			}
		}

		return j;
	}

	public void traceSegment(Collision result,Segment segment,int collideWithBits,Solid ignore){
		result.time=-ONE;

		Solid solid2;

		#if defined(TOADLET_USE_CLAMP)
			clampPosition(segment.origin);
			clampPosition(segment.direction);
		#endif

		Vector3 endPoint=cache_traceSegment_endPoint;
		segment.getEndPoint(endPoint);
		AABox total=cache_traceSegment_total.set(segment.origin,segment.origin);
		total.mergeWith(endPoint);
		mNumSpacialCollection=findSolidsInAABox(total,mSpacialCollection,mSpacialCollection.length);

		Collision collision=cache_traceSegment_collision.reset();
		int i;
		for(i=0;i<mNumSpacialCollection;++i){
			solid2=mSpacialCollection[i];
			if(solid2!=ignore && (collideWithBits&solid2.mCollisionBits)!=0){
				collision.time=-ONE;
				testSegment(collision,solid2,segment);
				if(collision.time>=0){
					if(result.time<0 || collision.time<result.time){
						result.set(collision);
					}
					else if(result.time==collision.time){
						add_vv(result.normal,collision.normal);
						boolean b=normalizeCarefully(result.normal,mEpsilon);
						if(b==false){
							result.set(collision);
						}
					}
				}
			}
		}

		if(mManager!=null){
			collision.time=-ONE;
			mManager.traceSegment(collision,segment);
			if(collision.time>=0){
				if(result.time<0 || collision.time<result.time){
					result.set(collision);
				}
				else if(result.time==collision.time){
					add_vv(result.normal,collision.normal);
					boolean b=normalizeCarefully(result.normal,mEpsilon);
					if(b==false){
						result.set(collision);
					}
				}
			}
		}
	}

	public void capVector3(Vector3 vector,scalar value){
		/// @todo: Optimize this, perhaps using some bitwise &?
		if(vector.x>value){vector.x=value;}
		if(vector.x<-value){vector.x=-value;}
		if(vector.y>value){vector.y=value;}
		if(vector.y<-value){vector.y=-value;}
		if(vector.z>value){vector.z=value;}
		if(vector.z<-value){vector.z=-value;}
	}

	public void convertToEpsilonOffset(Vector3 offset){
		if(offset.x>=mQuarterEpsilon)offset.x=mEpsilon;
		else if(offset.x<=-mQuarterEpsilon)offset.x=-mEpsilon;
		else offset.x=0;

		if(offset.y>=mQuarterEpsilon)offset.y=mEpsilon;
		else if(offset.y<=-mQuarterEpsilon)offset.y=-mEpsilon;
		else offset.y=0;

		if(offset.z>=mQuarterEpsilon)offset.z=mEpsilon;
		else if(offset.z<=-mQuarterEpsilon)offset.z=-mEpsilon;
		else offset.z=0;
	}

	public void clampPosition(Vector3 position){
		#if defined(TOADLET_USE_CLAMP)
			#if defined(TOADLET_FIXED_POINT)
				pos.x=(((pos.x + mHalfEpsilon)>>mEpsilonBits) << mEpsilonBits);
				pos.y=(((pos.y + mHalfEpsilon)>>mEpsilonBits) << mEpsilonBits);
				pos.z=(((pos.z + mHalfEpsilon)>>mEpsilonBits) << mEpsilonBits);
			#else
				pos.x=round(pos.x*mOneOverEpsilon)*mEpsilon;
				pos.y=round(pos.y*mOneOverEpsilon)*mEpsilon;
				pos.z=round(pos.z*mOneOverEpsilon)*mEpsilon;
			#endif
		#endif
	}

	public boolean toSmall(Vector3 value){
		return (value.x<mEpsilon && value.x>-mEpsilon && value.y<mEpsilon && value.y>-mEpsilon && value.z<mEpsilon && value.z>-mEpsilon);
	}

	/// Depends upon mSpacialCollection being update, so not public
	protected void traceSolid(Collision result,Solid solid,Segment segment){
		result.time=-ONE;

		if(solid.mCollideWithBits==0){
			return;
		}

		Solid solid2;

		Collision collision=cache_traceSolid_collision.reset();
		int i;
		for(i=0;i<mNumSpacialCollection;++i){
			solid2=mSpacialCollection[i];
			if(solid!=solid2 && (solid.mCollideWithBits&solid2.mCollisionBits)!=0){
				collision.time=-ONE;
				testSolid(collision,solid,solid2,segment);
				if(collision.time>=0){
					if(result.time<0 || collision.time<result.time){
						result.set(collision);
					}
					else if(result.time==collision.time){
						add_vv(result.normal,collision.normal);
						boolean b=normalizeCarefully(result.normal,mEpsilon);
						if(b==false){
							result.set(collision);
						}
					}
				}
			}
		}

		if(mManager!=null){
			collision.time=-ONE;
			mManager.traceSolid(collision,segment,solid);
			if(collision.time>=0){
				if(result.time<0 || collision.time<result.time){
					result.set(collision);
				}
				else if(result.time==collision.time){
					add_vv(result.normal,collision.normal);
					boolean b=normalizeCarefully(result.normal,mEpsilon);
					if(b==false){
						result.set(collision);
					}
				}
			}
		}
	}

	protected void testSolid(Collision result,Solid solid1,Solid solid2,Segment segment){
		Collision collision=cache_testSolid_collision.reset();
		collision.collider=solid2;

		Shape[] shapes1=solid1.mShapes;
		int numShapes1=solid1.mNumShapes;
		Shape[] shapes2=solid2.mShapes;
		int numShapes2=solid2.mNumShapes;
		Shape shape1;
		Shape shape2;

		int i,j;
		for(i=0;i<numShapes1;++i){
			shape1=shapes1[i];

			for(j=0;j<numShapes2;++j){
				shape2=shapes2[j];

				// No need to reset collision or it's time here since the trace functions are gaurenteed to do that

				// AABox collisions
				if(shape1.mType==Shape.Type.AABOX && shape2.mType==Shape.Type.AABOX){
					// Create one big box and then do our tests with a box and a point
					AABox box1=shape1.mAABox;
					AABox box2=shape2.mAABox;

					AABox box=cache_testSolid_box.set(box2);
					add_av(box,solid2.mPosition);
					sub_vv(box.maxs,box1.mins);
					sub_vv(box.mins,box1.maxs);

					traceAABox(collision,segment,box);
				}
				else if(shape1.mType==Shape.Type.AABOX && shape2.mType==Shape.Type.SPHERE){
					// Sphere-Box collisions aren't perfect, since its more of a box-box collision, but oh well
					// Create one big box and then do our tests with a box and a point
					AABox box1=shape1.mAABox;
					AABox box=cache_testSolid_box.set(shape2.mSphere.radius);
					add_av(box,shape2.mSphere.origin);

					add_av(box,solid2.mPosition);
					sub_vv(box.maxs,box1.mins);
					sub_vv(box.mins,box1.maxs);

					traceAABox(collision,segment,box);
				}
				else if(shape1.mType==Shape.Type.AABOX && shape2.mType==Shape.Type.CAPSULE){
					// Capsule-Box collisions aren't perfect, since its more of a box-box collision, but oh well
					// Create one big box and then do our tests with a box and a point
					AABox box1=shape1.mAABox;
					AABox box=cache_testSolid_box;
					shape2.getBound(box);

					add_av(box,solid2.mPosition);
					sub_vv(box.maxs,box1.mins);
					sub_vv(box.mins,box1.maxs);

					traceAABox(collision,segment,box);
				}
				// Sphere collisions
				else if(shape1.mType==Shape.Type.SPHERE && shape2.mType==Shape.Type.AABOX){
					// Sphere-Box collisions aren't perfect, since its more of a box-box collision, but oh well
					// Create one big box and then do our tests with a box and a point
					AABox box1=cache_testSolid_box1.set(shape1.mSphere.radius);
					add_av(box1,shape1.mSphere.origin);

					AABox box=cache_testSolid_box.set(shape2.mAABox);
					add_av(box,solid2.mPosition);
					sub_vv(box.maxs,box1.mins);
					sub_vv(box.mins,box1.maxs);
		
					traceAABox(collision,segment,box);
				}
				else if(shape1.mType==Shape.Type.SPHERE && shape2.mType==Shape.Type.SPHERE){
					// Construct super sphere
					Vector3 origin=cache_testSolid_origin.set(solid2.mPosition);

					sub_vv(origin,shape1.mSphere.origin);
					add_vv(origin,shape2.mSphere.origin);

					Sphere sphere=cache_testSolid_sphere.set(origin,shape2.mSphere.radius+shape1.mSphere.radius);

					traceSphere(collision,segment,sphere);
				}
				else if(shape1.mType==Shape.Type.SPHERE && shape2.mType==Shape.Type.CAPSULE){
					// Construct super capsule
					Vector3 origin=cache_testSolid_origin.set(solid2.mPosition);

					sub_vv(origin,shape1.mSphere.origin);
					add_vv(origin,shape2.mCapsule.origin);

					Capsule capsule=cache_testSolid_capsule.set(origin,shape2.mCapsule.direction,shape2.mCapsule.radius+shape1.mSphere.radius);

					traceCapsule(collision,segment,capsule);
				}
				// Capsule collisions
				else if(shape1.mType==Shape.Type.CAPSULE && shape2.mType==Shape.Type.AABOX){
					// Capsule-Box collisions aren't perfect, since its more of a box-box collision, but oh well
					// Create one big box and then do our tests with a box and a point
					AABox box1=cache_testSolid_box1;
					shape1.getBound(box1);
					AABox box=cache_testSolid_box.set(shape2.mAABox);

					add_av(box,solid2.mPosition);
					sub_vv(box.maxs,box1.mins);
					sub_vv(box.mins,box1.maxs);

					traceAABox(collision,segment,box);
				}
				else if(shape1.mType==Shape.Type.CAPSULE && shape2.mType==Shape.Type.SPHERE){
					// Construct super capsule
					Vector3 origin=cache_testSolid_origin.set(solid2.mPosition);
					sub_vv(origin,shape1.mCapsule.origin);
					add_vv(origin,shape2.mSphere.origin);

					// You must invert the capsule direction when forming the supercapsule at the sphere's location
					Vector3 direction=cache_testSolid_direction.set(shape1.mCapsule.direction);
					neg(direction);
					Capsule capsule=cache_testSolid_capsule.set(origin,direction,shape1.mCapsule.radius+shape2.mSphere.radius);

					traceCapsule(collision,segment,capsule);
				}
				else if(shape1.mType==Shape.Type.CAPSULE && shape2.mType==Shape.Type.CAPSULE){
					// Construct super capsule
					Vector3 origin=cache_testSolid_origin.set(solid2.mPosition);
					sub_vv(origin,shape1.mCapsule.origin);
					add_vv(origin,shape2.mCapsule.origin);

					Capsule capsule=cache_testSolid_capsule.set(origin,shape1.mCapsule.direction,shape1.mCapsule.radius+shape2.mCapsule.radius);

					traceCapsule(collision,segment,capsule);
				}

				if(collision.time>=0){
					if(result.time<0 || collision.time<result.time){
						result.set(collision);
					}
					else if(result.time==collision.time){
						add_vv(result.normal,collision.normal);
						boolean b=normalizeCarefully(result.normal,mEpsilon);
						if(b==false){
							result.set(collision);
						}
					}
				}
			}
		}
	}

	protected void testSegment(Collision result,Solid solid,Segment segment){
		Collision collision=cache_testSegment_collision.reset();
		collision.collider=solid;

		Shape[] shapes=solid.mShapes;
		int numShapes=solid.mNumShapes;

		Shape shape;

		int i;
		for(i=0;i<numShapes;++i){
			// No need to reset collision or it's time here since the trace functions are gaurenteed to do that

			shape=shapes[i];
			switch(shape.mType){
				case AABOX:
					{
						AABox box=cache_testSegment_box.set(shape.mAABox);
						add_av(box,solid.mPosition);
						traceAABox(collision,segment,box);
					}
				break;
				case SPHERE:
					{
						Sphere sphere=cache_testSegment_sphere.set(shape.mSphere);
						add_sv(sphere,solid.mPosition);
						traceSphere(collision,segment,sphere);
					}
				break;
				case CAPSULE:
					{
						Capsule capsule=cache_testSegment_capsule.set(shape.mCapsule);
						add_cv(capsule,solid.mPosition);
						traceCapsule(collision,segment,capsule);
					}
				break;
			}

			if(collision.time>=0){
				if(result.time<0 || collision.time<result.time){
					result.set(collision);
				}
				else if(result.time==collision.time){
					add_vv(result.normal,collision.normal);
					boolean b=normalizeCarefully(result.normal,mEpsilon);
					if(b==false){
						result.set(collision);
					}
				}
			}
		}
	}

	protected void traceAABox(Collision c,Segment segment,AABox box){
		if(testInside(segment.origin,box)){
			scalar x;
			#if defined(TOADLET_FIXED_POINT)
				x=segment.origin.x-box.mins.x;scalar dix=(x^(x>>31))-(x>>31);
				x=segment.origin.y-box.mins.y;scalar diy=(x^(x>>31))-(x>>31);
				x=segment.origin.z-box.mins.z;scalar diz=(x^(x>>31))-(x>>31);
				x=segment.origin.x-box.maxs.x;scalar dax=(x^(x>>31))-(x>>31);
				x=segment.origin.y-box.maxs.y;scalar day=(x^(x>>31))-(x>>31);
				x=segment.origin.z-box.maxs.z;scalar daz=(x^(x>>31))-(x>>31);
			#else
				x=segment.origin.x-box.mins.x;scalar dix=x<0?-x:x;
				x=segment.origin.y-box.mins.y;scalar diy=x<0?-x:x;
				x=segment.origin.z-box.mins.z;scalar diz=x<0?-x:x;
				x=segment.origin.x-box.maxs.x;scalar dax=x<0?-x:x;
				x=segment.origin.y-box.maxs.y;scalar day=x<0?-x:x;
				x=segment.origin.z-box.maxs.z;scalar daz=x<0?-x:x;
			#endif

			if(dix<=diy && dix<=diz && dix<=dax && dix<=day && dix<=daz){
				if(dot(segment.direction,NEG_X_UNIT_VECTOR3)>=0){
					return;
				}
				c.normal.set(NEG_X_UNIT_VECTOR3);
			}
			else if(diy<=diz && diy<=dax && diy<=day && diy<=daz){
				if(dot(segment.direction,NEG_Y_UNIT_VECTOR3)>=0){
					return;
				}
				c.normal.set(NEG_Y_UNIT_VECTOR3);
			}
			else if(diz<=dax && diz<=day && diz<=daz){
				if(dot(segment.direction,NEG_Z_UNIT_VECTOR3)>=0){
					return;
				}
				c.normal.set(NEG_Z_UNIT_VECTOR3);
			}
			else if(dax<=day && dax<=daz){
				if(dot(segment.direction,X_UNIT_VECTOR3)>=0){
					return;
				}
				c.normal.set(X_UNIT_VECTOR3);
			}
			else if(day<=daz){
				if(dot(segment.direction,Y_UNIT_VECTOR3)>=0){
					return;
				}
				c.normal.set(Y_UNIT_VECTOR3);
			}
			else{
				if(dot(segment.direction,Z_UNIT_VECTOR3)>=0){
					return;
				}
				c.normal.set(Z_UNIT_VECTOR3);
			}

			c.time=0;
			c.point.set(segment.origin);
		}
		else{
			c.time=findIntersection(segment,box,c.point,c.normal);
		}
	}

	protected void traceSphere(Collision c,Segment segment,Sphere sphere){
		if(testInside(segment.origin,sphere)){
			// Skip the 'closer in' checks on spheres, since now matter what we do,
			// Apparently we still slide in closer to spheres, very slightly.
			// I'll really need to look at this more some time
			// So we always just push anyone in a sphere out more
			{
				// If we're trying to trace farther into the sphere
				Vector3 n=cache_traceSphere_n.set(segment.origin);

				sub_vv(n,sphere.origin);
				normalizeCarefully(n,mEpsilon);
				// Maybe this calculation is inacurate to test if we might get 'epsilon bumped' in?
				if(dot(n,segment.direction)<=mEpsilon){
					c.time=0;
					c.point.set(segment.origin);
					normalizeCarefully(c.normal,n,mEpsilon);
				}
				else{
					c.time=-ONE;
				}
			}
		}
		else{
			c.time=findIntersection(segment,sphere,c.point,c.normal);
		}
	}

	protected void traceCapsule(Collision c,Segment segment,Capsule capsule){
		Vector3 p1=cache_traceCapsule_p1.reset();
		Vector3 p2=cache_traceCapsule_p2.reset();
		Segment s=cache_traceCapsule_s;
		
		// Capsules are really just segments, so test against a sphere located at the closest point of the capsule segment
		s.origin.set(capsule.origin);
		s.direction.set(capsule.direction);
		getClosestPointsOnSegments(p1,p2,s,segment,mEpsilon);
		
		Sphere sphere=cache_traceCapsule_sphere.set(p1,capsule.radius);
		traceSphere(c,segment,sphere);
	}

	protected void frictionLink(Vector3 result,Solid solid,Vector3 solidVelocity,Solid hitSolid,Vector3 hitSolidNormal,Vector3 appliedForce,scalar fdt){
		result.set(ZERO_VECTOR3);

		// Andrew's friction linking code
		if(solid.mMass>0 && hitSolid.mMass!=0 && (solid.mCoefficientOfStaticFriction>0 || solid.mCoefficientOfDynamicFriction>0)){
			scalar fn=0,lenVr=0;
			Vector3 vr=cache_frictionLink_vr;
			Vector3 ff=cache_frictionLink_ff;
			Vector3 fs=cache_frictionLink_fs;
			Vector3 normVr=cache_frictionLink_normVr;

			fn=mul_ss(dot(solid.mLocalGravity,hitSolidNormal),solid.mMass)+dot(appliedForce,hitSolidNormal);

			// When calculating relative velocity, ignore any components perpendicular to the plane of contact
			// normVr acts as a temp here
			sub_vvv(vr,solidVelocity,hitSolid.mVelocity);
			mul(normVr,hitSolidNormal,dot(vr,hitSolidNormal));
			sub_vv(vr,normVr);
			capVector3(vr,mMaxVelocityComponent);
			lenVr=length(vr);

			if(fn!=0 && lenVr>0){
				div(normVr,vr,lenVr);
				mul_vvs(ff,normVr,fn);
				mul_vvs(result,ff,solid.mCoefficientOfStaticFriction);

				// Multiply fdt in with result to avoid excessively large numbers in the calculation of fs
				mul_vs(result,fdt);

				// This is the static force required to resist relative velocity and any applied forces in this time step * fdt
				// Note friction can only act in the plane of contact, and normVr again acts as a temp variable here
				mul_vvs(fs,vr,-solid.mMass);
				mul(normVr,hitSolidNormal,dot(appliedForce,hitSolidNormal));
				sub_vvv(normVr,appliedForce,normVr);
				mul_vs(normVr,fdt);
				add_vv(fs,normVr);

				capVector3(fs,mMaxForceComponent);

				if(lengthSquared(fs)>lengthSquared(result)){
					mul_vvs(result,ff,solid.mCoefficientOfDynamicFriction);
				}
				else{
					div(result,fs,fdt);
				}
			}
		}
	}

	protected void constraintLink(Vector3 result,Solid solid,Vector3 solidPosition,Vector3 solidVelocity){
		Vector3 tx=cache_constraintLink_tx;
		Vector3 tv=cache_constraintLink_tv;
		Constraint c=null;

		result.set(ZERO_VECTOR3);

		int i;
		for(i=0;i<solid.mNumConstraints;++i){
			c=solid.mConstraints[i];
			if(c.getActive()==false){
				continue;
			}
			else if(solid==c.mStartSolid){
				if(c.mEndSolid!=null){
					sub_vvv(tx,c.mEndSolid.mPosition,solidPosition);
					sub_vvv(tv,c.mEndSolid.mVelocity,solidVelocity);
				}
				else{
					sub_vvv(tx,c.mEndPoint,solidPosition);
					mul_vvs(tv,solidVelocity,-ONE);
				}
			}
			else if(solid==c.mEndSolid){
				sub_vvv(tx,c.mStartSolid.mPosition,solidPosition);
				sub_vvv(tv,c.mStartSolid.mVelocity,solidVelocity);
			}
			else{
				continue;
			}

			mul_vs(tx,c.mSpringConstant);
			mul_vs(tv,c.mDampingConstant);
			add_vv(result,tx);
			add_vv(result,tv);
		}
	}

	protected void updateAcceleration(Vector3 result,Solid solid,Vector3 x,Vector3 v,scalar fdt){
		Vector3 frictionForce=cache_updateAcceleration_frictionForce;
		Vector3 constraintForce=cache_updateAcceleration_constraintForce;
		Vector3 fluidForce=cache_updateAcceleration_fluidForce;

		/*
			result=solid.mLocalGravity;
			if(solid.mMass!=0){
				constraintLink(constraintForce,solid,x,v,fdt);
				for(#NUM_TOUCHED_SOLIDS){
					frictionLink(frictionForce,solid,v,hitSolid,hitSolidNormal,constraintForce+solid.mForce,fdt);
				}
				dragForce=-mCoefficientOfEffectiveDrag*(v-mFluidVelocity);
				result+=1.0f/solid.mMass*(friction+solid.mForce+constraintForce+dragForce);
			}
		*/

		result.set(solid.mLocalGravity);
		if(solid.mMass!=0){
			constraintLink(constraintForce,solid,x,v);
			add_vv(constraintForce,solid.mForce);
			if(solid.mTouched1!=null){
				frictionLink(frictionForce,solid,v,solid.mTouched1,solid.mTouched1Normal,constraintForce,fdt);
				add_vv(constraintForce,frictionForce);
				if(solid.mTouched2!=null && solid.mTouched2!=solid.mTouched1){
					frictionLink(frictionForce,solid,v,solid.mTouched2,solid.mTouched2Normal,constraintForce,fdt);
					add_vv(constraintForce,frictionForce);
				}
			}
			sub_vvv(fluidForce,mFluidVelocity,v);
			mul_vs(fluidForce,solid.mCoefficientOfEffectiveDrag);
			add_vv(constraintForce,fluidForce);
			mul_vs(constraintForce,solid.mInvMass);
			add_vv(result,constraintForce);
		}
	}

	protected void integrationStep(Solid solid,Vector3 x,Vector3 v,Vector3 dx,Vector3 dv,scalar fdt,Vector3 resultX,Vector3 resultV){
		Vector3 tx=cache_integrationStep_tx;
		Vector3 tv=cache_integrationStep_tv;

		/*
			tx=x+dx*fdt;
			tv=v+dv*fdt;
			resultX=tv;
			updateAcceleration(resultV,solid,tx,tv,fdt);
		*/

		mul_vvs(tx,dx,fdt);
		add_vv(tx,x);
		mul_vvs(tv,dv,fdt);
		add_vv(tv,v);
		resultX.set(tv);
		updateAcceleration(resultV,solid,tx,tv,fdt);
	}

	Integrator mIntegrator=Integrator.EULER;

	Vector3 mFluidVelocity=new Vector3();
	Vector3 mGravity=new Vector3();

	#if defined(TOADLET_FIXED_POINT)
		int mEpsilonBits=0;
	#else
		scalar mOneOverEpsilon=0;
	#endif
	scalar mEpsilon=0;
	scalar mHalfEpsilon=0;
	scalar mQuarterEpsilon=0;

	scalar mMaxVelocityComponent=0;
	scalar mMaxForceComponent=0;
	Collision[] mCollisions=new Collision[0];
	int mNumCollision=0;
	Solid[] mSolids=new Solid[0];
	int mNumSolids=0;
	Constraint[] mConstraints=new Constraint[0];
	int mNumConstraints=0;
	Solid[] mSpacialCollection=new Solid[0];
	int mNumSpacialCollection=0;
	boolean mReportingCollisions=false;

	scalar mMicroCollisionThreshold=ONE;

	scalar mDeactivateSpeed=0;
	int mDeactivateCount=0;

	Vector3 cache_update_oldPosition=new Vector3();
	Vector3 cache_update_newPosition=new Vector3();
	Vector3 cache_update_velocity=new Vector3();
	Vector3 cache_update_temp=new Vector3();
	Vector3 cache_update_t=new Vector3();
	Vector3 cache_update_leftOver=new Vector3();
	Vector3 cache_update_dx1=new Vector3();
	Vector3 cache_update_dx2=new Vector3();
	Vector3 cache_update_dv1=new Vector3();
	Vector3 cache_update_dv2=new Vector3();
	Segment cache_update_path=new Segment();
	AABox cache_update_box=new AABox();
	Collision cache_update_c=new Collision();
	Vector3 cache_traceSegment_endPoint=new Vector3();
	AABox cache_traceSegment_total=new AABox();
	Collision cache_traceSegment_collision=new Collision();
	Collision cache_traceSolid_collision=new Collision();
	Collision cache_testSolid_collision=new Collision();
	AABox cache_testSolid_box=new AABox();
	AABox cache_testSolid_box1=new AABox();
	Vector3 cache_testSolid_origin=new Vector3();
	Sphere cache_testSolid_sphere=new Sphere();
	Capsule cache_testSolid_capsule=new Capsule();
	Vector3 cache_testSolid_direction=new Vector3();
	Collision cache_testSegment_collision=new Collision();
	AABox cache_testSegment_box=new AABox();
	Sphere cache_testSegment_sphere=new Sphere();
	Capsule cache_testSegment_capsule=new Capsule();
	Vector3 cache_traceSphere_n=new Vector3();
	Vector3 cache_traceCapsule_p1=new Vector3();
	Vector3 cache_traceCapsule_p2=new Vector3();
	Segment cache_traceCapsule_s=new Segment();
	Sphere cache_traceCapsule_sphere=new Sphere();
	Vector3 cache_frictionLink_vr=new Vector3();
	Vector3 cache_frictionLink_ff=new Vector3();
	Vector3 cache_frictionLink_fs=new Vector3();
	Vector3 cache_frictionLink_normVr=new Vector3();
	Vector3 cache_constraintLink_tx=new Vector3();
	Vector3 cache_constraintLink_tv=new Vector3();
	Vector3 cache_updateAcceleration_frictionForce=new Vector3();
	Vector3 cache_updateAcceleration_constraintForce=new Vector3();
	Vector3 cache_updateAcceleration_fluidForce=new Vector3();
	Vector3 cache_integrationStep_tx=new Vector3();
	Vector3 cache_integrationStep_tv=new Vector3();

	Manager mManager=null;
}
