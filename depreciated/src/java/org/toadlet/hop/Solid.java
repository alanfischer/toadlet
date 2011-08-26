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

public final class Solid{
	public final static scalar INFINITE_MASS=-Math.ONE;

	public Solid(){
		reset();
	}

	public void reset(){
		destroy();

		mCollisionBits=1;
		mCollideWithBits=1;
		mMass=Math.ONE;
		mInvMass=Math.ONE;
		mPosition.reset();
		mVelocity.reset();
		mForce.reset();
		mLocalGravity.reset();
		mLocalGravityOverride=false;
		mCoefficientOfRestitution=Math.HALF;
		mCoefficientOfRestitutionOverride=false;
		mCoefficientOfStaticFriction=Math.HALF;
		mCoefficientOfDynamicFriction=Math.HALF;
		mCoefficientOfEffectiveDrag=0;

		mNumShapes=0;
		mLocalBound.reset();
		mWorldBound.reset();

		mNumConstraints=0;
	
		mCollisionListener=null;
		mUserData=null;

		mActive=true;
		mDeactivateCount=0;	

		mTouched1=null;
		mTouched1Normal.reset();
		mTouched2=null;
		mTouched2Normal.reset();
		mTouching=null;
		mTouchingNormal.reset();
		mInside=false;
		
		mDoUpdateCallback=false;
		
		mSimulator=null;
	}

	public void destroy(){
		while(mNumConstraints>0){
			Constraint constraint=mConstraints[0];
			if(constraint.mStartSolid==this){
				constraint.destroy();
			}
			else{
				constraint.setEndSolid(null);
				internal_removeConstraint(constraint);
			}
		}

		mNumShapes=0;
	}
	
	public void setCollisionBits(int bits){mCollisionBits=bits;}
	public int getCollisionBits(){return mCollisionBits;}

	public void setCollideWithBits(int bits){mCollideWithBits=bits;}
	public int getCollideWithBits(){return mCollideWithBits;}

	public void setMass(scalar mass){
		mMass=mass;
		if(mMass>0){
			mInvMass=Math.div(Math.ONE,mMass);
		}
		else{
			mInvMass=0;
		}
	}
	public scalar getMass(){return mMass;}

	public void setInfiniteMass(){
		mMass=INFINITE_MASS;
		mInvMass=0;
	}
	public boolean hasInfiniteMass(){return mMass==INFINITE_MASS;}

	public void setPosition(Vector3 position){
		mActive=true;
		mDeactivateCount=0;
		mPosition.set(position);
		Math.add(mWorldBound,mLocalBound,mPosition);

		// Reset touching information
		mTouched1=null;
		mTouched2=null;
		mTouching=null;
		mInside=false;
	}

	public Vector3 getPosition(){return mPosition;}
	
	public void setVelocity(Vector3 velocity){
		mActive=true;
		mDeactivateCount=0;
		mVelocity.set(velocity);
	}

	public Vector3 getVelocity(){return mVelocity;}

	public void addForce(Vector3 force){
		mActive=true;
		mDeactivateCount=0;
		Math.add(mForce,force);
	}

	public Vector3 getForce(){return mForce;}
	public void clearForce(){mForce.reset();}

	public void setLocalGravity(Vector3 gravity){
		mActive=true;
		mDeactivateCount=0;
		mLocalGravity.set(gravity);
		mLocalGravityOverride=true;
	}

	public Vector3 getLocalGravity(){return mLocalGravity;}

	public void setWorldGravity(){
		mActive=true;
		mDeactivateCount=0;
		mLocalGravityOverride=false;
	}

	public boolean hasLocalGravity(){return mLocalGravityOverride;}

	public void setCoefficientOfRestitution(scalar coeff){mCoefficientOfRestitution=coeff;}
	public scalar getCoefficientOfRestitution(){return mCoefficientOfRestitution;}

	public void setCoefficientOfRestitutionOverride(boolean over){mCoefficientOfRestitutionOverride=over;}
	public boolean getCoefficientOfRestitutionOverride(){return mCoefficientOfRestitutionOverride;}

	public void setCoefficientOfStaticFriction(scalar coeff){mCoefficientOfStaticFriction=coeff;}
	public scalar getCoefficientOfStaticFriction(){return mCoefficientOfStaticFriction;}

	public void setCoefficientOfDynamicFriction(scalar coeff){mCoefficientOfDynamicFriction=coeff;}
	public scalar getCoefficientOfDynamicFriction(){return mCoefficientOfDynamicFriction;}

	public void setCoefficientOfEffectiveDrag(scalar coeff){mCoefficientOfEffectiveDrag=coeff;}
	public scalar getCoefficientOfEffectiveDrag(){return mCoefficientOfEffectiveDrag;}

	public void addShape(Shape shape){
		mActive=true;
		mDeactivateCount=0;

		if(mShapes==null || mShapes.length<mNumShapes+1){
			Shape[] shapes=new Shape[mNumShapes+1];
			if(mShapes!=null){
				System.arraycopy(mShapes,0,shapes,0,mShapes.length);
			}
			mShapes=shapes;
		}
		mShapes[mNumShapes++]=shape;

		shape.mSolid=this;
		updateLocalBound();
	}

	public void removeShape(Shape shape){
		mActive=true;
		mDeactivateCount=0;

		int i;
		for(i=0;i<mNumShapes;++i){
			if(mShapes[i]==shape)break;
		}
		if(i!=mNumShapes){
			System.arraycopy(mShapes,i+1,mShapes,i,mNumShapes-(i+1));
			mShapes[--mNumShapes]=null;
		}

		shape.mSolid=null;
		updateLocalBound();
	}

	public void removeAllShapes(){
		mActive=true;
		mDeactivateCount=0;
		mNumShapes=0;
		updateLocalBound();
	}

	public Shape getShape(int i){return mShapes[i];}
	public int getNumShapes(){return mNumShapes;}

	public AABox getLocalBound(){return mLocalBound;}
	public AABox getWorldBound(){return mWorldBound;}

	public Solid getTouching(){return mTouching;}
	public Vector3 getTouchingNormal(){return mTouchingNormal;}

	public void setCollisionListener(CollisionListener listener){mCollisionListener=listener;}
	public CollisionListener getCollisionListener(){return mCollisionListener;}

	public void setUserData(Object data){mUserData=data;}
	public Object getUserData(){return mUserData;}

	public void activate(){
		mDeactivateCount=0;
		if(mActive==false){
			mActive=true;
			int i;
			for(i=0;i<mNumConstraints;++i){
				Constraint c=mConstraints[i];
				if(c.mStartSolid!=this && c.mStartSolid!=null){
					c.mStartSolid.activate();
				}
				else if(c.mEndSolid!=this && c.mEndSolid!=null){
					c.mEndSolid.activate();
				}
			}
		}
	}

	public boolean getActive(){return (mActive && mSimulator!=null);}

	public boolean getInside(){return mInside;}

	public void setDoUpdateCallback(boolean callback){mDoUpdateCallback=callback;}

	public Simulator getSimulator(){return mSimulator;}

	public void setPositionNoActivate(Vector3 position){
		mPosition.set(position);
		Math.add(mWorldBound,mLocalBound,mPosition);
	}

	public void updateLocalBound(){
		if(mNumShapes==0){
			mLocalBound.reset();
		}
		else{
			mShapes[0].getBound(mLocalBound);
			int i;
			for(i=1;i<mNumShapes;++i){
				mShapes[i].getBound(cacheAABox);
				mLocalBound.mergeWith(cacheAABox);
			}
		}

		// Update world here to make sure everything is current
		Math.add(mWorldBound,mLocalBound,mPosition);
	}

	void internal_setSimulator(Simulator s){
		mSimulator=s;
		int i;
		for(i=0;i<mNumShapes;++i){
			mShapes[i].internal_clamp();
		}
	}

	void internal_addConstraint(Constraint constraint){
		if(mConstraints==null || mConstraints.length<mNumConstraints+1){
			Constraint[] constraints=new Constraint[mNumConstraints+1];
			if(mConstraints!=null){
				System.arraycopy(mConstraints,0,constraints,0,mConstraints.length);
			}
			mConstraints=constraints;
		}
		mConstraints[mNumConstraints++]=constraint;
	}

	void internal_removeConstraint(Constraint constraint){
		int i;
		for(i=0;i<mNumConstraints;++i){
			if(mConstraints[i]==constraint)break;
		}
		if(i!=mNumConstraints){
			mConstraints[i]=null;
			System.arraycopy(mConstraints,i+1,mConstraints,i,mNumConstraints-(i+1));
			mConstraints[--mNumConstraints]=null;
		}
	}

	AABox cacheAABox=new AABox();

	int mCollisionBits=0;
	int mCollideWithBits=0;
	scalar mMass=0;
	scalar mInvMass=0;
	Vector3 mPosition=new Vector3();
	Vector3 mVelocity=new Vector3();
	Vector3 mForce=new Vector3();
	Vector3 mLocalGravity=new Vector3();
	boolean mLocalGravityOverride=false;
	scalar mCoefficientOfRestitution=0;
	boolean mCoefficientOfRestitutionOverride=false;
	scalar mCoefficientOfStaticFriction=0;
	scalar mCoefficientOfDynamicFriction=0;
	scalar mCoefficientOfEffectiveDrag=0;

	Shape[] mShapes;
	int mNumShapes=0;
	AABox mLocalBound=new AABox();
	AABox mWorldBound=new AABox();

	Constraint[] mConstraints=new Constraint[0];
	int mNumConstraints=0;
	
	CollisionListener mCollisionListener=null;
	Object mUserData=null;

	boolean mActive=false;
	int mDeactivateCount=0;	

	Solid mTouched1=null;
	Vector3 mTouched1Normal=new Vector3();
	Solid mTouched2=null;
	Vector3 mTouched2Normal=new Vector3();
	Solid mTouching=null;
	Vector3 mTouchingNormal=new Vector3();
	boolean mInside;

	boolean mDoUpdateCallback=false;

	Simulator mSimulator=null;
}
