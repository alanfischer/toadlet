/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright (C) 2006, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer
 *
 * All source code for the Toadlet Engine, including
 * this file, is the sole property of Lightning Toads
 * Productions, LLC. It has been developed on our own
 * time, with our own tools, on our own hardware. None
 * of this code may be observed, used or changed
 * without our explicit permission.
 *
 *  - Lightning Toads Productions, LLC
 *
 ********** Copyright header - do not remove **********/

package com.lightningtoads.toadlet.tadpole.millihop{

import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.egg.mathfixed.tMath;
import com.lightningtoads.toadlet.tadpole.*;
import com.lightningtoads.toadlet.tadpole.sg.*;
#ifdef TOADLET_DEBUG
	import com.lightningtoads.toadlet.egg.*;
#endif

#define TOADLET_NO_USE_CLAMP

public final class HopSceneManager extends SceneManager{
	public function HopSceneManager(engine:Engine){
		super(engine);

		mFixedDT=50; // Fixed DT of 20 fps
		mAccumulatedDT=0;

		simulatorConstruct();
	}

	public override function update(dt:int):void{
		super.update(dt);

		var entity:HopEntity;
		
		var i:int;
		if(mFixedDT==0){
			for(i=mHopEntities.length-1;i>=0;--i){
				entity=mHopEntities[i];
				entity.prePhysicsNoFixedDT();
			}

			simulatorUpdate(dt);

			for(i=mHopEntities.length-1;i>=0;--i){
				entity=mHopEntities[i];
				entity.postPhysicsNoFixedDT();
				entity.updateThink(dt);
			}
		}
		else{
			mAccumulatedDT+=dt;

			if(mAccumulatedDT>=mFixedDT){
				for(i=mHopEntities.length-1;i>=0;--i){
					entity=mHopEntities[i];
					entity.prePhysicsFixedDT();
				}

				while(mAccumulatedDT>=mFixedDT){
					mAccumulatedDT-=mFixedDT;
					simulatorUpdate(mFixedDT);

					for(i=mHopEntities.length-1;i>=0;--i){
						entity=mHopEntities[i];
						entity.updateThink(mFixedDT);
					}
				}
			}

			var f:int=tMath.div(tMath.intToFixed(mAccumulatedDT),tMath.intToFixed(mFixedDT));

			for(i=mHopEntities.length-1;i>=0;--i){
				entity=mHopEntities[i];
				entity.interpolatePhysicalParameters(f);
			}
		}
	}

	internal function registerHopEntity(entity:HopEntity):void{
		var i:int;
		for(i=0;i<mHopEntities.length;++i){
			if(mHopEntities[i]==entity)break;
		}

		if(i==mHopEntities.length){
			mHopEntities[mHopEntities.length]=entity;
		}
	}

	internal function unregisterHopEntity(entity:HopEntity):void{
		var i:int;
		for(i=0;i<mHopEntities.length;++i){
			if(mHopEntities[i]==entity)break;
		}
		if(i!=mHopEntities.length){
			mHopEntities.splice(i,1);
		}
	}

	// A fixed DT of 0 disabled fixed DTs
	public function setFixedDT(dt:int):void{
		mFixedDT=dt;
	}

	public function getFixedDT():int{
		return mFixedDT;
	}

	internal var mHopEntities:Array=new Array();
	internal var mFixedDT:int;
	internal var mAccumulatedDT:int;
	internal var mInterpolationCache1:Vector3=new Vector3();
	internal var mInterpolationCache2:Vector3=new Vector3();





	public static var IT_EULER:int=0;
	public static var IT_IMPROVED:int=1;
	public static var IT_RUNGE_KUTTA:int=2;

	public function simulatorConstruct():void{
		setEpsilonBits(4);
		setMaxVelocityComponent(tMath.intToFixed(180)); // Above 180 we can overflow in square
		setGravity(Vector3.Vector3XYZ(0,0,-tMath.milliToFixed(9810)));
		setIntegratorType(IT_EULER);
		setDeactivateSpeed(1<<8); // Some default
		setDeactivateCount(4); // 4 frames
	}

	public function setEpsilonBits(epsilonBits:int):void{
		mEpsilonBits=epsilonBits;
		mEpsilon=1<<mEpsilonBits;
		mHalfEpsilon=mEpsilon>>1;
		mQuarterEpsilon=mEpsilon>>2;
	}
	public function getEpsilonBits():int{return mEpsilonBits;}

	public function setIntegratorType(integratorType:int):void{mIntegratorType=integratorType;}
	public function getIntegratorType():int{return mIntegratorType;}
	
	public function setMaxVelocityComponent(maxVelocityComponent:int):void{mMaxVelocityComponent=maxVelocityComponent;}
	public function getMaxVelocityComponent():int{return mMaxVelocityComponent;}
	
	public function setGravity(gravity:Vector3):void{
		mGravity.setV(gravity);
		var i:int;
		for(i=0;i<mSolids.length;++i){
			mSolids[i].mActive=true;
			mSolids[i].mDeactivateCount=0;
		}
	}
	public function getGravity():Vector3{return mGravity;}

	public function setFluidVelocity(fluidVelocity:Vector3):void{mFluidVelocity.setV(fluidVelocity);}
	public function getFluidVelocity():Vector3{return mFluidVelocity;}

	public function setManager(manager:Manager):void{mManager=manager;}
	public function getManager():Manager{return mManager;}

	public function setDeactivateSpeed(speed:int):void{mDeactivateSpeed=speed;}
	public function setDeactivateCount(count:int):void{mDeactivateCount=count;}

	public function addSolid(solid:HopEntity):void{
		var i:int;
		for(i=0;i<mSolids.length;++i){
			if(mSolids[i]==solid)break;
		}

		if(i==mSolids.length){
			mSolids[mSolids.length]=solid;
			mSpacialCollection[mSolids.length]=null;

			solid.mActive=true;
			solid.mDeactivateCount=0;
		}
	}

	public function removeSolid(solid:HopEntity):void{
		// We can't touch anything if we're not in the simulation
		// This will help in some logic code
		solid.mTouching=null;
		solid.mLastTouched=null;

		var i:int;
		for(i=0;i<mSolids.length;++i){
			var s:HopEntity=mSolids[i];
			if(s.mLastTouched==solid){
				s.mLastTouched=null;
			}
			if(s.mTouching==solid){
				s.mTouching=null;
			}
		}

		for(i=0;i<mSolids.length;++i){
			if(mSolids[i]==solid)break;
		}
		if(i!=mSolids.length){
			mSolids.splice(i,1);
		}
	}

	public function getNumSolids():int{return mSolids.length;}
	public function getSolid(i:int):HopEntity{return mSolids[i];}
	
	public function simulatorUpdate(dt:int):void{
		var oldPosition:Vector3=mCacheOldPosition;oldPosition.reset();
		var newPosition:Vector3=mCacheNewPosition;newPosition.reset();
		var offset:Vector3=mCacheOffset;offset.reset();
		var velocity:Vector3=mCacheVelocity;velocity.reset();
		var temp:Vector3=mCacheTemp;temp.reset();
		var t:Vector3=mCacheT;t.reset();
		var leftOver:Vector3=mCacheLeftOver;leftOver.reset();
		var frictionForce:Vector3=mCacheFrictionForce;frictionForce.reset();
		var k1:Vector3=mCacheK1;k1.reset();
		var k2:Vector3=mCacheK2;k2.reset();
		var k3:Vector3=mCacheK3;k3.reset();
		var k4:Vector3=mCacheK4;k4.reset();
		var cor:int=0,j:int=0,oneOverMass:int=0,oneOverHitMass:int=0;
		var loop:int=0;
		var solid:HopEntity=null,hitSolid:HopEntity=null;
		var c:Collision=mCacheUpdateCollision;c.reset();
		var numCollisions:int=0;
		var i:int;

		// Convert millisecond dt to fixed point dt
		var fdt:int=tMath.milliToFixed(dt);

		if(mManager!=null){
			mManager.preUpdate(dt,fdt);
		}

		for(i=0;i<mSolids.length;i++){
			solid=mSolids[i];

			if(solid.mActive==false){
				continue;
			}

			if(solid.mDoUpdateCallback && mManager!=null){
				mManager.preUpdateS(solid,dt,fdt);
			}

			hitSolid=solid.mTouching;
			frictionLink(solid,solid.mVelocity,hitSolid,fdt,frictionForce);

			if(solid.mMass>0){
				oneOverMass=tMath.div(tMath.ONE,solid.mMass);
			}
			else{
				oneOverMass=0;
			}

			// Integrate to obtain velocity
			if(mIntegratorType==IT_EULER){
				tMath.mulVVS(t,mGravity,tMath.mul(solid.mCoefficientOfGravity,fdt));
				tMath.addVVV(velocity,solid.mVelocity,t);
				if(solid.mMass!=0){
					tMath.subVVV(t,solid.mVelocity,mFluidVelocity);
					tMath.mulVS(t,solid.mCoefficientOfEffectiveDrag);
					tMath.subVVV(t,solid.mForce,t);
					tMath.addVV(t,frictionForce);
					tMath.mulVS(t,tMath.mul(fdt,oneOverMass));
					tMath.addVV(velocity,t);
				}
			}
			else if(mIntegratorType==IT_IMPROVED){
				tMath.mulVVS(k1,mGravity,tMath.mul(solid.mCoefficientOfGravity,fdt));
				k2.setV(k1);
				if(solid.mMass!=0){
					// Calculate relative velocity and store in leftOver
					tMath.subVVV(leftOver,solid.mVelocity,mFluidVelocity);

					tMath.mulVVS(t,leftOver,solid.mCoefficientOfEffectiveDrag);
					tMath.subVVV(t,solid.mForce,t);
					tMath.addVV(t,frictionForce);
					tMath.mulVS(t,tMath.mul(fdt,oneOverMass));
					tMath.addVV(k1,t);
					
					temp.setV(frictionForce);
					
					tMath.addVVV(t,solid.mVelocity,k1);
					frictionLink(solid,t,hitSolid,fdt,frictionForce);
					
					tMath.addVVV(t,leftOver,k1);
					tMath.mulVS(t,solid.mCoefficientOfEffectiveDrag);
					tMath.subVVV(t,solid.mForce,t);
					tMath.addVV(t,frictionForce);
					tMath.mulVS(t,tMath.mul(fdt,oneOverMass));
					tMath.addVV(k1,t);

					tMath.addVV(frictionForce,temp);
					tMath.mulVS(frictionForce,tMath.HALF);
				}
				tMath.addVVV(t,k1,k2);
				tMath.mulVS(t,tMath.HALF);
				tMath.addVVV(velocity,solid.mVelocity,t);
			}
			else if(mIntegratorType==IT_RUNGE_KUTTA){
				throw new Error("IT_RUNGE_KUTTA not implemented");
			}

			if(solid.mInteraction==HopEntity.INTERACTION_NORMAL && hitSolid!=null && hitSolid.hasInfiniteMass()==false){
				tMath.mulVVS(temp,frictionForce,-tMath.ONE);
				hitSolid.addForce(temp);
			}

			// Cap velocity
			if(velocity.x>mMaxVelocityComponent){velocity.x=mMaxVelocityComponent;}
			if(velocity.x<-mMaxVelocityComponent){velocity.x=-mMaxVelocityComponent;}
			if(velocity.y>mMaxVelocityComponent){velocity.y=mMaxVelocityComponent;}
			if(velocity.y<-mMaxVelocityComponent){velocity.y=-mMaxVelocityComponent;}
			if(velocity.z>mMaxVelocityComponent){velocity.z=mMaxVelocityComponent;}
			if(velocity.z<-mMaxVelocityComponent){velocity.z=-mMaxVelocityComponent;}

			// Integrate to obtain positions
			oldPosition.setV(solid.mPosition);
			if(mIntegratorType==IT_EULER){
				tMath.mulVVS(temp,velocity,fdt);
				tMath.addVVV(newPosition,oldPosition,temp);
			}
			else if(mIntegratorType==IT_IMPROVED || mIntegratorType==IT_RUNGE_KUTTA){
				// The Runge Kutta denegerates to the Improved for position
				tMath.mulVVS(k1,solid.mVelocity,fdt);
				tMath.mulVVS(k2,velocity,fdt);
				tMath.addVVV(temp,k1,k2);
				tMath.mulVS(temp,tMath.HALF);
				tMath.addVVV(newPosition,oldPosition,temp);
			}

			// Clean up
			solid.mVelocity.setV(velocity);
			solid.clearForce();

			var first:Boolean=true;
			var skip:Boolean=false;

			if(solid.mDoUpdateCallback && mManager!=null){
				mManager.intraUpdateS(solid,dt,fdt);
			}

			// Collect all possible solids in the whole movement area
			if(solid.mInteraction!=HopEntity.INTERACTION_NONE){
				#ifdef TOADLET_USE_CLAMP
					clampPosition(oldPosition);
					clampPosition(newPosition);
				#endif

				tMath.subVVV(temp,newPosition,oldPosition);
				if(toSmall(temp)){
					newPosition.setV(oldPosition);
					skip=true;
				}
				else{
					if(temp.x<0){temp.x=-temp.x;}
					if(temp.y<0){temp.y=-temp.y;}
					if(temp.z<0){temp.z=-temp.z;}

					var m:int=temp.x;
					if(temp.y>m){m=temp.y;}
					if(temp.z>m){m=temp.z;}
					m+=mEpsilon; // Move it out by epsilon, since we haven't clamped yet

					//AABox startBox=new AABox(solid.mLocalBound);
					var box:AABox=mCacheBox1;
					box.setA(solid.mLocalBound);

					tMath.addAV(box,newPosition);
					box.mins.x-=m;
					box.mins.y-=m;
					box.mins.z-=m;
					box.maxs.x+=m;
					box.maxs.y+=m;
					box.maxs.z+=m;

					mNumSpacialCollection=getSolidsInAABox(box,mSpacialCollection);
				}
			}

			// Loop to use up available momentum
			loop=0;
			while(skip==false){
				if(first==true){
					first=false;
				}
				else{
					#ifdef TOADLET_USE_CLAMP
						clampPosition(oldPosition);
						clampPosition(newPosition);
					#endif

					tMath.subVVV(temp,newPosition,oldPosition);
					if(toSmall(temp)){
						newPosition.setV(oldPosition);
						break;
					}
				}
				traceSolid(solid,oldPosition,newPosition,c);

				if(c.time>=0){
					// TODO: Improve this Inside logic
					if(c.time==0 && c.solid!=null && c.solid.mMass==HopEntity.INFINITE_MASS){
						solid.mInside=true;
					}
					else{
						solid.mInside=false;
					}

					// If its a valid collision, and someone is listening, then store it
					if(c.solid!=solid.mTouching &&
						(solid.mCollisionListener!=null || c.solid.mCollisionListener!=null)){

						c.collider=solid;
						tMath.subVVV(c.velocity,solid.mVelocity,c.solid.mVelocity);

						if(mCollisions.length<=numCollisions){
							mCollisions[mCollisions.length]=new Collision();
						}
						
						mCollisions[numCollisions].setC(c);
						numCollisions++;
					}
					hitSolid=c.solid;
					
					// Conservation of momentum
					if(solid.mCoefficientOfRestitutionOverride){
						cor=solid.mCoefficientOfRestitution;
					}
					else{
						cor=(solid.mCoefficientOfRestitution+hitSolid.mCoefficientOfRestitution)>>1;
					}

					tMath.subVVV(temp,hitSolid.mVelocity,solid.mVelocity);
					var numerator:int=tMath.mul((tMath.ONE+cor),tMath.dotVV(temp,c.normal));

					// Temp stores the velocity change on hitSolid
					temp.reset();

					if(solid.mMass!=0 && hitSolid.mMass!=0){
						if(solid.hasInfiniteMass()){
							oneOverMass=0;
						}
						else{
							oneOverMass=tMath.div(tMath.ONE,solid.mMass);
						}
					
						if(hitSolid.hasInfiniteMass()){
							oneOverHitMass=0;
						}
						else{
							oneOverHitMass=tMath.div(tMath.ONE,hitSolid.mMass);
						}

						if(oneOverMass+oneOverHitMass!=0){
							j=tMath.div(numerator,oneOverMass+oneOverHitMass);
						}
						else{
							j=0;
						}

						if(solid.hasInfiniteMass()==false){
							tMath.mulVVS(t,c.normal,j);
							tMath.mulVS(t,oneOverMass);
							tMath.addVV(solid.mVelocity,t);
						}
						if(hitSolid.hasInfiniteMass()==false && solid.mInteraction==HopEntity.INTERACTION_NORMAL){
							if(solid.hasInfiniteMass()){
								// HACK: For now, infinite mass trains assume a CoefficientOfRestitution of 1 
								// for the hitSolid calculation to avoid having gravity jam objects against them 
								tMath.subVVV(temp,hitSolid.mVelocity,solid.mVelocity);
								tMath.mulVVS(temp,c.normal,tMath.dotVV(temp,c.normal)<<1);
							}
							else{
								tMath.mulVVS(temp,c.normal,j);
								tMath.mulVS(temp,oneOverHitMass);
							}
						}
					}
					else if(hitSolid.mMass==0 && solid.mInteraction==HopEntity.INTERACTION_NORMAL){
						tMath.mulVVS(temp,c.normal,numerator);
					}
					else if(solid.mMass==0){
						tMath.mulVVS(t,c.normal,numerator);
						tMath.addVV(solid.mVelocity,t);
					}

					if(Math.abs(temp.x)>=mDeactivateSpeed || Math.abs(temp.y)>=mDeactivateSpeed || Math.abs(temp.z)>=mDeactivateSpeed){
						hitSolid.mActive=true;
						hitSolid.mDeactivateCount=0;
						tMath.subVV(hitSolid.mVelocity,temp);
					}

					// Calculate offset vector, and then resulting position
					#if defined(TOADLET_USE_CLAMP)
						clampPosition(c.point);
					#endif

					// Different offset options
					#define TOADLET_OFFSET_1
					#if defined(TOADLET_OFFSET_1)
						temp.setV(c.normal);
						convertToEpsilonOffset(temp);
						tMath.addVV(temp,c.point);
					#elif defined(TOADLET_OFFSET_2)
						tMath.subVVV(offset,oldPosition,c.point);
						convertToEpsilonOffset(offset);
						temp.setV(offset);
						tMath.addVV(temp,c.point);
					#elif defined(TOADLET_OFFSET_3)
						tMath.subVVV(offset,oldPosition,c.point);
						convertToEpsilonOffset(offset);
						temp.setV(c.normal);
						convertToEpsilonOffset(temp);
						tMath.addVV(temp,c.point);
						tMath.addVV(temp,offset);
					#else
						temp.setV(c.point);
					#endif

					// Calculate left over amount
					tMath.subVVV(leftOver,newPosition,temp);

					if(solid.mInteraction!=HopEntity.INTERACTION_NONE && c.solid.mInteraction==HopEntity.INTERACTION_NORMAL){
						// Touching code
						if(solid.mLastTouched==c.solid){
							solid.mTouching=c.solid;
							solid.mTouchingNormal.setV(c.normal);
						}
						else{
							solid.mLastTouched=c.solid;
							solid.mTouching=null;
						}
					}

					if(toSmall(leftOver)){
						newPosition.setV(temp);
						break;
					}
					else if(loop>4){
						newPosition.setV(temp);
						break;
					}
					else{
						// Calculate new destinations from coefficient of restitution applied to velocity
						if(tMath.normalizeCarefullyVV(velocity,solid.mVelocity,mEpsilon)==false){
							newPosition.setV(temp);
							break;
						}
						else{
							oldPosition.setV(temp);
							tMath.mulVS(velocity,tMath.lengthV(leftOver));
							tMath.mulVVS(temp,c.normal,tMath.dotVV(velocity,c.normal));
							tMath.subVV(velocity,temp);
							tMath.addVVV(newPosition,oldPosition,velocity);
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
				if(solid.mTouching!=null && solid.mInteraction!=HopEntity.INTERACTION_NONE && solid.mTouching.mInteraction==HopEntity.INTERACTION_NORMAL){
					solid.mLastTouched=solid.mTouching;
					solid.mTouching=null;
				}
				else{
					solid.mLastTouched=null;
				}
			}

			if(Math.abs(newPosition.x-solid.mPosition.x)<mDeactivateSpeed && Math.abs(newPosition.y-solid.mPosition.y)<mDeactivateSpeed && Math.abs(newPosition.z-solid.mPosition.z)<mDeactivateSpeed){
				solid.mDeactivateCount++;
				if(solid.mDeactivateCount>mDeactivateCount){
					solid.mActive=false;
					solid.mDeactivateCount=0;
				}
			}
			else{
				solid.mDeactivateCount=0;
			}

			solid.setPositionNoActivate(newPosition);

			if(solid.mDoUpdateCallback && mManager!=null){
				mManager.postUpdateS(solid,dt,fdt);
			}
		}

		// Now report all collisions to both solids
		for(i=0;i<numCollisions;++i){
			var col:Collision=mCollisions[i];

			var listener:CollisionListener=col.collider.mCollisionListener;
			if(listener!=null){
				listener.collision(col);
			}

			var s:HopEntity=col.solid;
			col.solid=col.collider;
			col.collider=s;

			listener=col.collider.mCollisionListener;
			if(col.collider.mInteraction==HopEntity.INTERACTION_NORMAL && listener!=null){
				tMath.negV(col.normal);
				tMath.negV(col.velocity);
				listener.collision(col);
			}
		}

		if(mManager!=null){
			mManager.postUpdate(dt,fdt);
		}
	}

	public function getSolidsInAABox(box:AABox,solids:Array):int{
		var amount:int=0;

		var i:int;
		for(i=0;i<mSolids.length;++i){
			if(tMath.testIntersectionAA(box,mSolids[i].mWorldBound)){
				solids[amount]=mSolids[i];
				amount++;
			}
		}

		return amount;
	}

	public function getHopEntitiesInAABox(box:AABox,solids:Array):void{
		var amount:int=getSolidsInAABox(box,mSpacialCollection);
		solids.splice(0,solids.length);

		var i:int;
		for(i=0;i<amount;++i){
			solids[i]=mSpacialCollection[i];
		}
	}
	
	public function convertToEpsilonOffset(offset:Vector3):void{
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
	
	public function convertToGenerousOffset(offset:Vector3):void{
		if(offset.x>=0)offset.x=mEpsilon;
		else if(offset.x<=0)offset.x=-mEpsilon;
		else offset.x=0;

		if(offset.y>=0)offset.y=mEpsilon;
		else if(offset.y<=-0)offset.y=-mEpsilon;
		else offset.y=0;

		if(offset.z>=0)offset.z=mEpsilon;
		else if(offset.z<=-0)offset.z=-mEpsilon;
		else offset.z=0;
	}

	public function clampPosition(pos:Vector3):void{
		#ifdef TOADLET_USE_CLAMP
			pos.x = ((pos.x + mHalfEpsilon)>>mEpsilonBits) << mEpsilonBits;
			pos.y = ((pos.y + mHalfEpsilon)>>mEpsilonBits) << mEpsilonBits;
			pos.z = ((pos.z + mHalfEpsilon)>>mEpsilonBits) << mEpsilonBits;
		#endif
	}
	
	public function toSmall(value:Vector3):Boolean{
		return (value.x<mEpsilon && value.x>-mEpsilon && value.y<mEpsilon && value.y>-mEpsilon && value.z<mEpsilon && value.z>-mEpsilon);
	}

	public function traceLine(start:Vector3,end:Vector3,ignore:HopEntity,closest:Collision):void{
		var b:Boolean;

		closest.time=-tMath.ONE;

		var s:Segment=mCacheSegment1;s.setVV(start,end);

		var solid2:HopEntity;

		#ifdef TOADLET_USE_CLAMP
			clampPosition(s.origin);
			clampPosition(s.direction);
		#endif

		var total:AABox=mCacheBox3;total.setVV(start,start);

		total.mergeWithV(end);
		mNumSpacialCollection=getSolidsInAABox(total,mSpacialCollection);
		
		var i:int;
		for(i=0;i<mNumSpacialCollection;++i){
			solid2=mSpacialCollection[i];
			if(ignore!=solid2 && solid2.mInteraction==HopEntity.INTERACTION_NORMAL){
				mCacheCollision1.time=-tMath.ONE;
				testLine(solid2,s,mCacheCollision1);
				if(mCacheCollision1.time>=0){
					if(closest.time<0 || mCacheCollision1.time<closest.time){
						closest.setC(mCacheCollision1);
					}
					else if(closest.time==mCacheCollision1.time){
						tMath.addVV(closest.normal,mCacheCollision1.normal);
						b=tMath.normalizeCarefullyV(closest.normal,mEpsilon);
						if(b==false){
							closest.setC(mCacheCollision1);
						}
					}
				}

			}
		}

		if(mManager!=null){
			mCacheCollision1.time=-tMath.ONE;
			mManager.traceLine(s,mCacheCollision1);
			if(mCacheCollision1.time>=0){
				if(closest.time<0 || mCacheCollision1.time<closest.time){
					closest.setC(mCacheCollision1);
				}
				else if(closest.time==mCacheCollision1.time){
					tMath.addVV(closest.normal,mCacheCollision1.normal);
					b=tMath.normalizeCarefullyV(closest.normal,mEpsilon);
					if(b==false){
						closest.setC(mCacheCollision1);
					}
				}
			}
		}
	}

	// Depends upon mSpacialCollection being updated, so not public
	internal function traceSolid(solid:HopEntity,start:Vector3,end:Vector3,closest:Collision):void{
		var b:Boolean;

		closest.time=-tMath.ONE;

		if(solid.mInteraction==HopEntity.INTERACTION_NONE){
			return;
		}

		var s:Segment=mCacheSegment1;s.setVV(start,end);

		var solid2:HopEntity;
	
		// Box tracing moved out to the main loop

		var i:int;
		for(i=0;i<mNumSpacialCollection;++i){
			solid2=mSpacialCollection[i];
			if(solid!=solid2 && solid2.mInteraction==HopEntity.INTERACTION_NORMAL){
				mCacheCollision1.time=-tMath.ONE;
				testSolid(solid,solid2,s,mCacheCollision1);
				if(mCacheCollision1.time>=0){
					if(closest.time<0 || mCacheCollision1.time<closest.time){
						closest.setC(mCacheCollision1);
					}
					else if(closest.time==mCacheCollision1.time){
						tMath.addVV(closest.normal,mCacheCollision1.normal);
						b=tMath.normalizeCarefullyV(closest.normal,mEpsilon);
						if(b==false){
							closest.setC(mCacheCollision1);
						}
					}
				}

			}
		}

		if(mManager!=null){
			mCacheCollision1.time=-tMath.ONE;
			mManager.traceSolid(s,solid,mCacheCollision1);
			if(mCacheCollision1.time>=0){
				if(closest.time<0 || mCacheCollision1.time<closest.time){
					closest.setC(mCacheCollision1);
				}
				else if(closest.time==mCacheCollision1.time){
					tMath.addVV(closest.normal,mCacheCollision1.normal);
					b=tMath.normalizeCarefullyV(closest.normal,mEpsilon);
					if(b==false){
						closest.setC(mCacheCollision1);
					}
				}
			}
		}
	}

	internal function testSolid(solid1:HopEntity,solid2:HopEntity,segment:Segment,closest:Collision):void{
		mCacheCollision2.reset();
		mCacheCollision2.solid=solid2;

		var shapes1:Array=solid1.mShapes;
		var numShapes1:int=shapes1.length;
		var shapes2:Array=solid2.mShapes;
		var numShapes2:int=shapes2.length;
		var shape1:Shape;
		var shape2:Shape;

		var box:AABox;
		var box1:AABox;
		var box2:AABox;
		var origin:Vector3;
		var sphere:Sphere;
		var capsule:Capsule;

		var i:int,j:int;
		for(i=0;i<numShapes1;++i){
			shape1=shapes1[i];

			for(j=0;j<numShapes2;++j){
				shape2=shapes2[j];

				// No need to reset mCacheCollision2 or it's time here since the trace functions are gaurenteed to do that

				// AABox collisions
				if(shape1.mType==Shape.TYPE_AABOX && shape2.mType==Shape.TYPE_AABOX){
					// Create one big box and then do our tests with a box and a point
					box1=shape1.mAABox;
					box2=shape2.mAABox;

					box=mCacheBox4;box.setA(box2);
					tMath.addAV(box,solid2.mPosition);
					tMath.subVV(box.maxs,box1.mins);
					tMath.subVV(box.mins,box1.maxs);

					traceAABox(segment,box,mCacheCollision2);
				}
				else if(shape1.mType==Shape.TYPE_AABOX && shape2.mType==Shape.TYPE_SPHERE){
					// Sphere-Box collisions aren't perfect, since its more of a box-box collision, but oh well
					// Create one big box and then do our tests with a box and a point
					box1=shape1.mAABox;
					box=mCacheBox4;box.setR(shape2.mSphere.radius);
					tMath.addAV(box,shape2.mSphere.origin);

					tMath.addAV(box,solid2.mPosition);
					tMath.subVV(box.maxs,box1.mins);
					tMath.subVV(box.mins,box1.maxs);

					traceAABox(segment,box,mCacheCollision2);
				}
				else if(shape1.mType==Shape.TYPE_AABOX && shape2.mType==Shape.TYPE_CAPSULE){
					// Capsule-Box collisions aren't perfect, since its more of a box-box collision, but oh well
					// Create one big box and then do our tests with a box and a point
					box1=shape1.mAABox;
					box=mCacheBox4;
					shape2.getBound(box);

					tMath.addAV(box,solid2.mPosition);
					tMath.subVV(box.maxs,box1.mins);
					tMath.subVV(box.mins,box1.maxs);

					traceAABox(segment,box,mCacheCollision2);
				}
				// Sphere collisions
				else if(shape1.mType==Shape.TYPE_SPHERE && shape2.mType==Shape.TYPE_AABOX){
					// Sphere-Box collisions aren't perfect, since its more of a box-box collision, but oh well
					// Create one big box and then do our tests with a box and a point
					box1=mCacheBox4;box1.setR(shape1.mSphere.radius);
					tMath.addAV(box1,shape1.mSphere.origin);

					box=mCacheBox5;box.setA(shape2.mAABox);
					tMath.addAV(box,solid2.mPosition);
					tMath.subVV(box.maxs,box1.mins);
					tMath.subVV(box.mins,box1.maxs);
		
					traceAABox(segment,box,mCacheCollision2);
				}
				else if(shape1.mType==Shape.TYPE_SPHERE && shape2.mType==Shape.TYPE_SPHERE){
					// Construct super sphere
					origin=mCacheVector1;origin.setV(solid2.mPosition);

					tMath.subVV(origin,shape1.mSphere.origin);
					tMath.addVV(origin,shape2.mSphere.origin);

					sphere=mCacheSphere1;sphere.setVR(origin,shape2.mSphere.radius+shape1.mSphere.radius);

					traceSphere(segment,sphere,mCacheCollision2);
				}
				else if(shape1.mType==Shape.TYPE_SPHERE && shape2.mType==Shape.TYPE_CAPSULE){
					// Construct super capsule
					origin=mCacheVector1;origin.setV(solid2.mPosition);

					tMath.subVV(origin,shape1.mSphere.origin);
					tMath.addVV(origin,shape2.mCapsule.origin);

					capsule=mCacheCapsule1;capsule.setVVR(origin,shape2.mCapsule.direction,shape2.mCapsule.radius+shape1.mSphere.radius);

					traceCapsule(segment,capsule,mCacheCollision2);
				}
				// Capsule collisions
				else if(shape1.mType==Shape.TYPE_CAPSULE && shape2.mType==Shape.TYPE_AABOX){
					// Capsule-Box collisions aren't perfect, since its more of a box-box collision, but oh well
					// Create one big box and then do our tests with a box and a point
					box1=mCacheBox4;
					shape1.getBound(box1);
					box=mCacheBox5;box.setA(shape2.mAABox);

					tMath.addAV(box,solid2.mPosition);
					tMath.subVV(box.maxs,box1.mins);
					tMath.subVV(box.mins,box1.maxs);

					traceAABox(segment,box,mCacheCollision2);
				}
				else if(shape1.mType==Shape.TYPE_CAPSULE && shape2.mType==Shape.TYPE_SPHERE){
					// Construct super capsule
					origin=mCacheVector1;origin.setV(solid2.mPosition);
					tMath.subVV(origin,shape1.mCapsule.origin);
					tMath.addVV(origin,shape2.mSphere.origin);

					// You must invert the capsule direction when forming the supercapsule at the sphere's location
					var direction:Vector3=mCacheDirection;direction.setV(shape1.mCapsule.direction);
					tMath.negV(direction);
					capsule=mCacheCapsule1;capsule.setVVR(origin,direction,shape1.mCapsule.radius+shape2.mSphere.radius);

					traceCapsule(segment,capsule,mCacheCollision2);
				}
				else if(shape1.mType==Shape.TYPE_CAPSULE && shape2.mType==Shape.TYPE_CAPSULE){
#if 0
					// The origin of the 2nd capsule wrt the 1st shape's position
					Vector3 origin=(solid2->mPosition)-(shape1->mSphere.origin)+(shape2->mSphere.origin);

					// Find the closest point on the 1st capsule to the 2nd's origin
					Segment s1;
					s1.origin=solid1->mPosition;
					s1.direction=shape1->mCapsule.direction;
					Vector3 o;
					getClosestPointOnSegment(o,s1,origin);
					
					// Construct super capsule
					Capsule capsule(origin+o,shape2->mCapsule.direction,shape1->mCapsule.radius+shape2->mCapsule.radius);
					traceCapsule(segment,capsule,mCacheCollision1);
#endif
					// Construct super capsule
					origin=mCacheVector1;origin.setV(solid2.mPosition);
					tMath.subVV(origin,shape1.mCapsule.origin);
					tMath.addVV(origin,shape2.mCapsule.origin);

					capsule=mCacheCapsule1;capsule.setVVR(origin,shape1.mCapsule.direction,shape1.mCapsule.radius+shape2.mCapsule.radius);

					traceCapsule(segment,capsule,mCacheCollision2);
				}

				if(mCacheCollision2.time>=0){
					if(closest.time<0 || mCacheCollision2.time<closest.time){
						closest.setC(mCacheCollision2);
					}
					else if(closest.time==mCacheCollision2.time){
						tMath.addVV(closest.normal,mCacheCollision2.normal);
						var b:Boolean=tMath.normalizeCarefullyV(closest.normal,mEpsilon);
						if(b==false){
							closest.setC(mCacheCollision2);
						}
					}
				}
			}
		}
	}
	
	internal function testLine(solid:HopEntity,segment:Segment,closest:Collision):void{
		mCacheCollision2.reset();
		mCacheCollision2.solid=solid;

		var shapes:Array=solid.mShapes;
		var numShapes:int=shapes.length;

		var shape:Shape;

		var i:int;
		for(i=0;i<numShapes;++i){
			// No need to reset mCacheCollision2 or it's time here since the trace functions are gaurenteed to do that

			shape=shapes[i];
			switch(shape.mType){
				case(Shape.TYPE_AABOX):
					{
						var box:AABox=mCacheBox4;box.setA(shape.mAABox);

						tMath.addAV(box,solid.mPosition);
						traceAABox(segment,box,mCacheCollision2);
					}
				break;
				case(Shape.TYPE_SPHERE):
					{
						var sphere:Sphere=mCacheSphere1;sphere.setS(shape.mSphere);

						tMath.addSV(sphere,solid.mPosition);
						traceSphere(segment,sphere,mCacheCollision2);
					}
				break;
				case(Shape.TYPE_CAPSULE):
					{
						var capsule:Capsule=mCacheCapsule1;capsule.setC(shape.mCapsule);

						tMath.addCV(capsule,solid.mPosition);
						traceCapsule(segment,capsule,mCacheCollision2);
					}
				break;
			}

			if(mCacheCollision2.time>=0){
				if(closest.time<0 || mCacheCollision2.time<closest.time){
					closest.setC(mCacheCollision2);
				}
				else if(closest.time==mCacheCollision2.time){
					tMath.addVV(closest.normal,mCacheCollision2.normal);
					var b:Boolean=tMath.normalizeCarefullyV(closest.normal,mEpsilon);
					if(b==false){
						closest.setC(mCacheCollision2);
					}
				}
			}
		}
	}

	internal function traceAABox(segment:Segment,box:AABox,c:Collision):void{
		// We offset boxes to ensure that they stay in the 'half epsilon' borders
		// This lets us not worry about shifting box planes to detect the closest outside,
		// like we have to do with the sphere

		// Use QuarterEpsilon instead of Half, since a quarter will ensure where we get rounded to
		//AABox b=new AABox(box);
		var b:AABox=mCacheBox6;
		b.setA(box);

		// I dont think this is necessary anymore
		//b.maxs.x-=mQuarterEpsilon;
		//b.maxs.y-=mQuarterEpsilon;
		//b.maxs.z-=mQuarterEpsilon;
		//b.mins.x+=mQuarterEpsilon;
		//b.mins.y+=mQuarterEpsilon;
		//b.mins.z+=mQuarterEpsilon;

		if(tMath.testInsideVA(segment.origin,b)){
			var dix:int=tMath.abs(segment.origin.x-b.mins.x);
			var diy:int=tMath.abs(segment.origin.y-b.mins.y);
			var diz:int=tMath.abs(segment.origin.z-b.mins.z);
			var dax:int=tMath.abs(segment.origin.x-b.maxs.x);
			var day:int=tMath.abs(segment.origin.y-b.maxs.y);
			var daz:int=tMath.abs(segment.origin.z-b.maxs.z);

			if(dix<=diy && dix<=diz && dix<=dax && dix<=day && dix<=daz){
				if(tMath.dotVV(segment.direction,tMath.NEG_X_UNIT_VECTOR3)>=0){
					return;
				}
				c.normal.setV(tMath.NEG_X_UNIT_VECTOR3);
			}
			else if(diy<=diz && diy<=dax && diy<=day && diy<=daz){
				if(tMath.dotVV(segment.direction,tMath.NEG_Y_UNIT_VECTOR3)>=0){
					return;
				}
				c.normal.setV(tMath.NEG_Y_UNIT_VECTOR3);
			}
			else if(diz<=dax && diz<=day && diz<=daz){
				if(tMath.dotVV(segment.direction,tMath.NEG_Z_UNIT_VECTOR3)>=0){
					return;
				}
				c.normal.setV(tMath.NEG_Z_UNIT_VECTOR3);
			}
			else if(dax<=day && dax<=daz){
				if(tMath.dotVV(segment.direction,tMath.X_UNIT_VECTOR3)>=0){
					return;
				}
				c.normal.setV(tMath.X_UNIT_VECTOR3);
			}
			else if(day<=daz){
				if(tMath.dotVV(segment.direction,tMath.Y_UNIT_VECTOR3)>=0){
					return;
				}
				c.normal.setV(tMath.Y_UNIT_VECTOR3);
			}
			else{
				if(tMath.dotVV(segment.direction,tMath.Z_UNIT_VECTOR3)>=0){
					return;
				}
				c.normal.setV(tMath.Z_UNIT_VECTOR3);
			}

			c.time=0;
			c.point.setV(segment.origin);
		}
		else{
			c.time=tMath.findIntersectionSA(segment,b,c.point,c.normal);
		}
	}

	internal function traceSphere(segment:Segment,sphere:Sphere,c:Collision):void{
		// Check to see if we're inside
		if(tMath.testInsideVS(segment.origin,sphere)){
			// Skip the 'closer in' checks on spheres, since now matter what we do,
			// Apparently we still slide in closer to spheres, very slightly.
			// I'll really need to look at this more some time
			// So we always just push anyone in a sphere out more
			{
				// If we're trying to trace farther into the sphere
				var n:Vector3=mCacheVector4;n.setV(segment.origin);

				tMath.subVV(n,sphere.origin);
				tMath.normalizeCarefullyV(n,mEpsilon);
				// Maybe this calculation is inacurate to test if we might get 'epsilon bumped' in?
				if(tMath.dotVV(n,segment.direction)<=mEpsilon){
					c.time=0;
					c.point.setV(segment.origin);
					tMath.normalizeCarefullyVV(c.normal,n,mEpsilon);
				}
				else{
					c.time=-tMath.ONE;
				}
			}
		}
		else{
			c.time=tMath.findIntersectionSS(segment,sphere,c.point,c.normal);
		}
	}

	internal function traceCapsule(segment:Segment,capsule:Capsule,c:Collision):void{
		var p1:Vector3=mCacheVector2;
		var p2:Vector3=mCacheVector3;
		var s:Segment=mCacheSegment2;

		// Capsules are really just segments, so test against a sphere located at the closest point of the capsule segment
		s.origin.setV(capsule.origin);
		s.direction.setV(capsule.direction);
		tMath.getClosestPointsOnSegments(p1,p2,s,segment,mEpsilon);

		var sphere:Sphere=mCacheSphere1;sphere.setVR(p1,capsule.radius);
		traceSphere(segment,sphere,c);
	}

	internal function frictionLink(solid:HopEntity,solidVelocity:Vector3,hitSolid:HopEntity,fdt:int,result:Vector3):void{
		result.setV(tMath.ZERO_VECTOR3);

		// Andrew's friction linking code
		if(hitSolid!=null && solid.mMass>0 && hitSolid.mMass!=0 && (solid.mCoefficientOfStaticFriction>0 || solid.mCoefficientOfDynamicFriction>0)){
			var fn:int=0,lenVr:int=0;
			//Vector3 vr=new Vector3();
			var vr:Vector3=mCachevr;
			vr.setV(tMath.ZERO_VECTOR3);
			//Vector3 ff=new Vector3();
			var ff:Vector3=mCacheff;
			ff.setV(tMath.ZERO_VECTOR3);
			//Vector3 fs=new Vector3();
			var fs:Vector3=mCachefs;
			fs.setV(tMath.ZERO_VECTOR3);
			//Vector3 normVr=new Vector3();
			var normVr:Vector3=mCachenormVr;
			normVr.setV(tMath.ZERO_VECTOR3);

			fn=tMath.mul(tMath.mul(tMath.dotVV(mGravity,solid.mTouchingNormal),solid.mCoefficientOfGravity),solid.mMass) + tMath.dotVV(solid.mForce,solid.mTouchingNormal);
			tMath.subVVV(vr,solidVelocity,hitSolid.mVelocity);
			lenVr=tMath.lengthV(vr);
			tMath.divVVS(normVr,vr,lenVr);

			if(fn!=0 && lenVr>0){
				tMath.mulVVS(ff,normVr,fn);
				tMath.mulVVS(result,ff,solid.mCoefficientOfStaticFriction);

				// Multiply fdt in here to avoid exessively large numbers when calculating fs
				tMath.mulVS(result,fdt);
			
				// This is the static force required to resist relative velocity and any applied forces in this time step
				tMath.mulVVS(fs,vr,-solid.mMass);
				tMath.mulVS(normVr,tMath.dotVV(solid.mForce,normVr));
				tMath.mulVS(normVr,fdt);
				tMath.addVV(fs,normVr);

				if(tMath.lengthSquaredV(fs)>tMath.lengthSquaredV(result)){
					tMath.mulVVS(result,ff,solid.mCoefficientOfDynamicFriction);
				}
				else{
					tMath.divVVS(result,fs,fdt);
				}
			}
		}
	}

	internal var mIntegratorType:int;

	internal var mGravity:Vector3=new Vector3();
	internal var mFluidVelocity:Vector3=new Vector3();

	internal var mEpsilonBits:int;
	internal var mEpsilon:int;
	internal var mHalfEpsilon:int;
	internal var mQuarterEpsilon:int;

	internal var mMaxVelocityComponent:int;
	internal var mCollisions:Array=new Array();
	internal var mSolids:Array=new Array()
	internal var mSpacialCollection:Array=new Array();
	internal var mNumSpacialCollection:int=0;

	internal var mDeactivateSpeed:int;
	internal var mDeactivateCount:int;

	internal var mManager:Manager;

	internal var mCacheOldPosition:Vector3=new Vector3();
	internal var mCacheNewPosition:Vector3=new Vector3();
	internal var mCacheOffset:Vector3=new Vector3();
	internal var mCacheVelocity:Vector3=new Vector3();
	internal var mCacheTemp:Vector3=new Vector3();
	internal var mCacheT:Vector3=new Vector3();
	internal var mCacheLeftOver:Vector3=new Vector3();
	internal var mCacheFrictionForce:Vector3=new Vector3();
	internal var mCacheK1:Vector3=new Vector3();
	internal var mCacheK2:Vector3=new Vector3();
	internal var mCacheK3:Vector3=new Vector3();
	internal var mCacheK4:Vector3=new Vector3();
	internal var mCacheUpdateCollision:Collision=new Collision();

	internal var mCacheCollision1:Collision=new Collision();
	internal var mCacheCollision2:Collision=new Collision();

	internal var mCacheVector1:Vector3=new Vector3();
	internal var mCacheVector2:Vector3=new Vector3();
	internal var mCacheVector3:Vector3=new Vector3();
	internal var mCacheVector4:Vector3=new Vector3();
	internal var mCacheDirection:Vector3=new Vector3();
	internal var mCacheSegment1:Segment=new Segment();
	internal var mCacheSegment2:Segment=new Segment();
	internal var mCacheBox1:AABox=new AABox();
	internal var mCacheBox2:AABox=new AABox();
	internal var mCacheBox3:AABox=new AABox();
	internal var mCacheBox4:AABox=new AABox();
	internal var mCacheBox5:AABox=new AABox();
	internal var mCacheBox6:AABox=new AABox();

	internal var mCacheSphere1:Sphere=new Sphere();
	internal var mCacheCapsule1:Capsule=new Capsule();

	internal var mCachevr:Vector3=new Vector3();
	internal var mCacheff:Vector3=new Vector3();
	internal var mCachefs:Vector3=new Vector3();
	internal var mCachenormVr:Vector3=new Vector3();
}

}
