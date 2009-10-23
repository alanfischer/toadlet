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

package com.lightningtoads.toadlet.tadpole.microhop;

import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.egg.mathfixed.Math;
import com.lightningtoads.toadlet.tadpole.*;
import com.lightningtoads.toadlet.tadpole.sg.*;

public class HopEntity extends NodeEntity{
	public HopEntity(Engine engine){
		super(engine);

		solidConstruct();

		try{
			mSceneManager=(HopSceneManager)mEngine.getSceneManager();
		}
		catch(ClassCastException e){
			mSceneManager=null;
		}

		if(mSceneManager==null){
			throw new RuntimeException("HopEntity: Invalid scene manager");
		}

		mSceneManager.registerHopEntity(this);
	}

	public void destroy(){
		mSceneManager.unregisterHopEntity(this);

		super.destroy();
	}

	public void parentChanged(Entity entity,NodeEntity parent){
		if(entity==this){
			if(parent==mSceneManager){
				mSceneManager.addSolid(this);

				mSceneManager.clampPosition(mLocalBound.mins);
				mSceneManager.clampPosition(mLocalBound.maxs);
			}
			else{
				mSceneManager.removeSolid(this);
			}
		}

		super.parentChanged(entity,parent);
	}

	public void setTranslate(Vector3 translate){
		super.setTranslate(translate);
		setPosition(translate);
		mLastPosition.set(translate);
	}

	public void setTranslate(int x,int y,int z){
		super.setTranslate(x,y,z);
		setPosition(getTranslate());
		mLastPosition.set(getTranslate());
	}

	// This function is used to access the FixedDT interpolated velocity, since toadlet::hop & toadlet::tadpole::hop are merged,
	// the regular getVelocity function returns the real velocity
	public Vector3 getInterpolatedVelocity(){
		return mInterpolatedVelocity;
	}

	public void think(){}

	public final void setNextThink(int nextThink){
		mNextThink=nextThink;
	}

	public final int getNextThink(){
		return mNextThink;
	}

	//#ifdef TOADLET_PLATFORM_JSR184
		public void setCastsShadow(boolean shadow){
			mCastsShadow=shadow;
		}

		public boolean getCastsShadow(){
			return mCastsShadow;
		}
	//#endif

	public final HopSceneManager getSceneManager(){return mSceneManager;}

	final void prePhysicsNoFixedDT(){
		mActivePrevious=getActive();
	}

	final void postPhysicsNoFixedDT(){
		if(getActive()==false && mActivePrevious==false){
			return;
		}

		super.setTranslate(getPosition());
		mInterpolatedVelocity.set(getVelocity());
	}

	final void prePhysicsFixedDT(){
		mLastPosition.set(getPosition());
		mLastVelocity.set(getVelocity());
		mActivePrevious=getActive();
	}

	final void interpolatePosition(int f){
		if((getActive()==false && mActivePrevious==false) || mSceneManager==null){
			return;
		}

		// If we are deactivating, then make sure we are at our rest point
		if(getActive()==false && mActivePrevious){
			f=Math.ONE;
		}

		Math.mul(mSceneManager.mInterpolationCache1,mLastPosition,Math.ONE-f);
		Math.mul(mSceneManager.mInterpolationCache2,getPosition(),f);
		Math.add(mSceneManager.mInterpolationCache1,mSceneManager.mInterpolationCache2);
		super.setTranslate(mSceneManager.mInterpolationCache1);

		Math.mul(mSceneManager.mInterpolationCache1,mLastVelocity,Math.ONE-f);
		Math.mul(mSceneManager.mInterpolationCache2,getVelocity(),f);
		Math.add(mSceneManager.mInterpolationCache1,mSceneManager.mInterpolationCache2);
		mInterpolatedVelocity.set(mSceneManager.mInterpolationCache1);
	}

	final void updateThink(int dt){
		if(mNextThink>0){
			mNextThink-=dt;
			if(mNextThink<=0){
				mNextThink=0;
				think();
			}
		}
	}

	boolean mActivePrevious=true;
	


	public final static byte INTERACTION_NORMAL=0;
	public final static byte INTERACTION_NONE=1;
	public final static byte INTERACTION_GHOST=2;

	public final static int INFINITE_MASS=
		//#exec fpcomp.bat "(-1)"
		;

	public void solidConstruct(){
		mMass=
			//#exec fpcomp.bat "(1)"
			;
		mInteraction=INTERACTION_NORMAL;
		mCoefficientOfRestitution=
			//#exec fpcomp.bat "(1/2)"
			;
		mCoefficientOfRestitutionOverride=false;
		mCoefficientOfStaticFriction=
			//#exec fpcomp.bat "(1/2)"
			;
		mCoefficientOfDynamicFriction=
			//#exec fpcomp.bat "(1/2)"
			;
		mCoefficientOfGravity=
			//#exec fpcomp.bat "(1)"
			;
		mCoefficientOfEffectiveDrag=0;
		mLastTouched=null;
		mTouching=null;
		mCollisionListener=null;
		mActive=true;
		mDeactivateCount=0;
	}

	public void setMass(int mass){mMass=mass;}
	public int getMass(){return mMass;}
	
	public void setInfiniteMass(){mMass=INFINITE_MASS;}
	public boolean hasInfiniteMass(){return mMass==INFINITE_MASS;}

	public void setInteraction(byte i){mInteraction=i;}
	public byte getInteraction(){return mInteraction;}

	public void setPosition(Vector3 position){
		mPosition.set(position);
		Math.add(mWorldBound,mLocalBound,mPosition);
		mActive=true;
		mDeactivateCount=0;
	}
	public Vector3 getPosition(){return mPosition;}
	
	public void setVelocity(Vector3 velocity){
		mVelocity.set(velocity);
		mActive=true;
		mDeactivateCount=0;

		// Set velocity interpolation parameters here, since we combined toadlet.hop + toadlet.tadpole.hop
		mLastVelocity.set(velocity);
		mInterpolatedVelocity.set(velocity);
	}
	public Vector3 getVelocity(){return mVelocity;}
	
	public void addForce(Vector3 force){
		Math.add(mForce,mForce,force);
		mActive=true;
		mDeactivateCount=0;
	}
	public Vector3 getForce(){return mForce;}
	public void clearForce(){mForce.set(Math.ZERO_VECTOR3);}

	public void setCoefficientOfRestitution(int coeff){mCoefficientOfRestitution=coeff;}
	public int getCoefficientOfRestitution(){return mCoefficientOfRestitution;}

	public void setCoefficientOfRestitutionOverride(boolean b){mCoefficientOfRestitutionOverride=b;}
	public boolean getCoefficientOfRestitutionOverride(){return mCoefficientOfRestitutionOverride;}

	public void setCoefficientOfStaticFriction(int coeff){mCoefficientOfStaticFriction=coeff;}
	public int getCoefficientOfStaticFriction(){return mCoefficientOfStaticFriction;}
	
	public void setCoefficientOfDynamicFriction(int coeff){mCoefficientOfDynamicFriction=coeff;}
	public int getCoefficientOfDynamicFriction(){return mCoefficientOfDynamicFriction;}
	
	public void setCoefficientOfGravity(int coeff){
		mCoefficientOfGravity=coeff;
		mActive=true;
		mDeactivateCount=0;
	}
	public int getCoefficientOfGravity(){return mCoefficientOfGravity;}
	
	public void setCoefficientOfEffectiveDrag(int coeff){mCoefficientOfEffectiveDrag=coeff;}
	public int getCoefficientOfEffectiveDrag(){return mCoefficientOfEffectiveDrag;}

	public void setAABox(AABox box){
		mLocalBound.set(box);
		internal_updateLocalBound();
	}

	public AABox getAABox(){return mLocalBound;}

	public AABox getLocalBound(){return mLocalBound;}
	public AABox getWorldBound(){return mWorldBound;}
	
	public HopEntity getTouching(){return mTouching;}
	public Vector3 getTouchingNormal(){return mTouchingNormal;}
	
	public void setCollisionListener(CollisionListener listener){mCollisionListener=listener;}
	public CollisionListener getCollisionListener(){return mCollisionListener;}

	public boolean getActive(){return (mActive && mParent==mSceneManager);}

	public boolean getInside(){return mInside;}

	void setPositionNoActivate(Vector3 position){
		mPosition.set(position);
		Math.add(mWorldBound,mLocalBound,mPosition);
	}

	void internal_updateLocalBound(){
		// Update world here to make sure everything is current
		Math.add(mWorldBound,mLocalBound,mPosition);
	}

	int mMass;
	byte mInteraction;
	Vector3 mPosition=new Vector3();
	Vector3 mVelocity=new Vector3();
	Vector3 mForce=new Vector3();
	int mCoefficientOfRestitution;
	boolean mCoefficientOfRestitutionOverride;
	int mCoefficientOfStaticFriction;
	int mCoefficientOfDynamicFriction;
	int mCoefficientOfGravity;
	int mCoefficientOfEffectiveDrag;

	int mNumShapes;
	AABox mLocalBound=new AABox();
	AABox mWorldBound=new AABox();

	CollisionListener mCollisionListener;

	boolean mActive;
	int mDeactivateCount;
	
	HopEntity mLastTouched;
	HopEntity mTouching;
	Vector3 mTouchingNormal=new Vector3();
	boolean mInside;

	int mNextThink;
	HopSceneManager mSceneManager;
	Vector3 mLastPosition=new Vector3();
	Vector3 mLastVelocity=new Vector3();
	Vector3 mInterpolatedVelocity=new Vector3();
	//#ifdef TOADLET_PLATFORM_JSR184
		javax.microedition.m3g.Mesh mShadowMesh=null;
		boolean mCastsShadow=false;
	//#endif
}

