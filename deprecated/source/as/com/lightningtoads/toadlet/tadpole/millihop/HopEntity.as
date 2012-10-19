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
import com.lightningtoads.toadlet.tadpole.*;
import com.lightningtoads.toadlet.tadpole.sg.*;

public class HopEntity extends NodeEntity{
	public function HopEntity(engine:Engine){
		super(engine);

		solidConstruct();

		try{
			mSceneManager=HopSceneManager(mEngine.getSceneManager());
		}
		catch(e:TypeError){
			mSceneManager=null;
		}

		if(mSceneManager==null){
			throw new Error("HopEntity: Invalid scene manager");
		}

		mSceneManager.registerHopEntity(this);
	}

	public override function destroy():void{
		mSceneManager.unregisterHopEntity(this);

		super.destroy();
	}

	public override function parentChanged(entity:Entity,parent:NodeEntity):void{
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

	public override function setTranslateV(translate:Vector3):void{
		super.setTranslateV(translate);
		setPosition(translate);
		mLastPosition.setV(translate);
	}

	public override function setTranslateXYZ(x:int,y:int,z:int):void{
		super.setTranslateXYZ(x,y,z);
		setPosition(getTranslate());
		mLastPosition.setV(getTranslate());
	}

	// This function is used to access the FixedDT interpolated velocity, since toadlet::hop & toadlet::tadpole::hop are merged,
	// the regular getVelocity function returns the real velocity
	public function getInterpolatedVelocity():Vector3{
		return mInterpolatedVelocity;
	}

	public function think():void{}

	public function setNextThink(nextThink:int):void{
		mNextThink=nextThink;
	}

	public function getNextThink():int{
		return mNextThink;
	}

	public function getSceneManager():HopSceneManager{return mSceneManager;}

	internal function prePhysicsNoFixedDT():void{
		mActivePrevious=getActive();
	}

	internal function postPhysicsNoFixedDT():void{
		if(getActive()==false && mActivePrevious==false){
			return;
		}

		super.setTranslateV(getPosition());
		mInterpolatedVelocity.setV(getVelocity());
	}

	internal function prePhysicsFixedDT():void{
		mLastPosition.setV(getPosition());
		mLastVelocity.setV(getVelocity());
		mActivePrevious=getActive();
	}

	internal function interpolatePhysicalParameters(f:int):void{
		if((getActive()==false && mActivePrevious==false) || mSceneManager==null){
			return;
		}

		// If we are deactivating, then make sure we are at our rest point
		if(getActive()==false && mActivePrevious){
			f=tMath.ONE;
		}

		tMath.mulVVS(mSceneManager.mInterpolationCache1,mLastPosition,tMath.ONE-f);
		tMath.mulVVS(mSceneManager.mInterpolationCache2,getPosition(),f);
		tMath.addVV(mSceneManager.mInterpolationCache1,mSceneManager.mInterpolationCache2);
		super.setTranslateV(mSceneManager.mInterpolationCache1);

		tMath.mulVVS(mSceneManager.mInterpolationCache1,mLastVelocity,tMath.ONE-f);
		tMath.mulVVS(mSceneManager.mInterpolationCache2,getVelocity(),f);
		tMath.addVV(mSceneManager.mInterpolationCache1,mSceneManager.mInterpolationCache2);
		mInterpolatedVelocity.setV(mSceneManager.mInterpolationCache1);
	}

	internal function updateThink(dt:int):void{
		if(mNextThink>0){
			mNextThink-=dt;
			if(mNextThink<=0){
				mNextThink=0;
				think();
			}
		}
	}

	internal var mActivePrevious:Boolean=true;



	public static var INTERACTION_NORMAL:int=0;
	public static var INTERACTION_NONE:int=1;
	public static var INTERACTION_GHOST:int=2; // Ghost solids are not pushed by other solids, they only collide into other solids

	public static var INFINITE_MASS:int=-tMath.ONE;

	public function solidConstruct():void{
		mMass=tMath.ONE;
		mInteraction=INTERACTION_NORMAL;
		mCoefficientOfRestitution=tMath.HALF;
		mCoefficientOfRestitutionOverride=false;
		mCoefficientOfStaticFriction=tMath.HALF;
		mCoefficientOfDynamicFriction=tMath.HALF;
		mCoefficientOfGravity=tMath.ONE;
		mCoefficientOfEffectiveDrag=0;
		mLastTouched=null;
		mTouching=null;
		mCollisionListener=null;
		mActive=true;
		mDeactivateCount=0;
		mDoUpdateCallback=false;
	}

	public function setMass(mass:int):void{mMass=mass;}
	public function getMass():int{return mMass;}

	public function setInfiniteMass():void{mMass=INFINITE_MASS;}
	public function hasInfiniteMass():Boolean{return mMass==INFINITE_MASS;}

	public function setInteraction(i:int):void{mInteraction=i;}
	public function getInteraction():int{return mInteraction;}

	public function setPosition(position:Vector3):void{
		mPosition.setV(position);
		tMath.addAAV(mWorldBound,mLocalBound,mPosition);
		mActive=true;
		mDeactivateCount=0;

		// Reset touching information
		mLastTouched=null;
		mTouching=null;
		mTouchingNormal.reset();
		mInside=false;
	}
	public function getPosition():Vector3{return mPosition;}

	public function setVelocity(velocity:Vector3):void{
		mVelocity.setV(velocity);
		mActive=true;
		mDeactivateCount=0;

		// Set velocity interpolation parameters here, since we combined toadlet.hop + toadlet.tadpole.hop
		mLastVelocity.setV(velocity);
		mInterpolatedVelocity.setV(velocity);
	}
	public function getVelocity():Vector3{return mVelocity;}

	public function addForce(force:Vector3):void{
		tMath.addVVV(mForce,mForce,force);
		mActive=true;
		mDeactivateCount=0;
	}
	public function getForce():Vector3{return mForce;}
	public function clearForce():void{mForce.setV(tMath.ZERO_VECTOR3);}

	public function setCoefficientOfRestitution(coeff:int):void{mCoefficientOfRestitution=coeff;}
	public function getCoefficientOfRestitution():int{return mCoefficientOfRestitution;}

	public function setCoefficientOfRestitutionOverride(b:Boolean):void{mCoefficientOfRestitutionOverride=b;}
	public function getCoefficientOfRestitutionOverride():Boolean{return mCoefficientOfRestitutionOverride;}

	public function setCoefficientOfStaticFriction(coeff:int):void{mCoefficientOfStaticFriction=coeff;}
	public function getCoefficientOfStaticFriction():int{return mCoefficientOfStaticFriction;}

	public function setCoefficientOfDynamicFriction(coeff:int):void{mCoefficientOfDynamicFriction=coeff;}
	public function getCoefficientOfDynamicFriction():int{return mCoefficientOfDynamicFriction;}

	public function setCoefficientOfGravity(coeff:int):void{
		mCoefficientOfGravity=coeff;
		mActive=true;
		mDeactivateCount=0;
	}
	public function getCoefficientOfGravity():int{return mCoefficientOfGravity;}

	public function setCoefficientOfEffectiveDrag(coeff:int):void{mCoefficientOfEffectiveDrag=coeff;}
	public function getCoefficientOfEffectiveDrag():int{return mCoefficientOfEffectiveDrag;}

	public function addShape(shape:Shape):void{
		mActive=true;
		mDeactivateCount=0;

		mShapes[mShapes.length]=shape;

		shape.mSolid=this;
		updateLocalBound();
	}

	public function removeShape(shape:Shape):void{
		mActive=true;
		mDeactivateCount=0;

		var i:int;
		for(i=0;i<mShapes.length;++i){
			if(mShapes[i]==shape)break;
		}
		if(i!=mShapes.length){
			mShapes.splice(i,1);
		}

		shape.mSolid=null;
		updateLocalBound();
	}

	public function getShape(i:int):Shape{return mShapes[i];}
	public function getNumShapes():int{return mShapes.length;}

	public function getLocalBound():AABox{return mLocalBound;}
	public function getWorldBound():AABox{return mWorldBound;}
	
	public function getTouching():HopEntity{return mTouching;}
	public function getTouchingNormal():Vector3{return mTouchingNormal;}
	
	public function setCollisionListener(listener:CollisionListener):void{mCollisionListener=listener;}
	public function getCollisionListener():CollisionListener{return mCollisionListener;}

	public function activate():void{
		mActive=true;
		mDeactivateCount=0;
	}
	public function getActive():Boolean{return (mActive && mParent==mSceneManager);}

	public function getInside():Boolean{return mInside;}

	public function setDoUpdateCallback(callback:Boolean):void{mDoUpdateCallback=callback;}
	
	public function setPositionNoActivate(position:Vector3):void{
		mPosition.setV(position);
		tMath.addAAV(mWorldBound,mLocalBound,mPosition);
	}

	public function updateLocalBound():void{
		if(mShapes.length==0){
			mLocalBound.setA(tMath.ZERO_AABOX);
		}
		else{
			mShapes[0].getBound(mLocalBound);
			var i:int;
			for(i=1;i<mShapes.length;++i){
				mShapes[i].getBound(mAABoxCache1);
				mLocalBound.mergeWithA(mAABoxCache1);
			}
		}

		// Update world here to make sure everything is current
		tMath.addAAV(mWorldBound,mLocalBound,mPosition);
	}

	internal var mMass:int;
	internal var mInteraction:int;
	internal var mPosition:Vector3=new Vector3();
	internal var mVelocity:Vector3=new Vector3();
	internal var mForce:Vector3=new Vector3();
	internal var mCoefficientOfRestitution:int;
	internal var mCoefficientOfRestitutionOverride:Boolean;
	internal var mCoefficientOfStaticFriction:int;
	internal var mCoefficientOfDynamicFriction:int;
	internal var mCoefficientOfGravity:int;
	internal var mCoefficientOfEffectiveDrag:int;

	internal var mShapes:Array=new Array(0);
	internal var mLocalBound:AABox=new AABox();
	internal var mWorldBound:AABox=new AABox();
	internal var mAABoxCache1:AABox=new AABox();

	internal var mCollisionListener:CollisionListener;

	internal var mActive:Boolean;
	internal var mDeactivateCount:int;

	internal var mLastTouched:HopEntity;
	internal var mTouching:HopEntity;
	internal var mTouchingNormal:Vector3=new Vector3();
	internal var mInside:Boolean;

	internal var mDoUpdateCallback:Boolean;

	internal var mNextThink:int;
	internal var mSceneManager:HopSceneManager;
	internal var mLastPosition:Vector3=new Vector3();
	internal var mLastVelocity:Vector3=new Vector3();
	internal var mInterpolatedVelocity:Vector3=new Vector3();
}

}
