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

public final class Shape{
	public static var TYPE_AABOX:int=0;
	public static var TYPE_SPHERE:int=1;
	public static var TYPE_CAPSULE:int=2;

	public function Shape(){
		mAABox=new AABox();
		mType=TYPE_AABOX;
	}

	public static function ShapeA(box:AABox):Shape{
		var s:Shape=new Shape();
		s.mAABox=AABox.AABoxA(box);
		s.mType=TYPE_AABOX;
		return s;
	}

	public static function ShapeS(sphere:Sphere):Shape{
		var s:Shape=new Shape();
		s.mSphere=Sphere.SphereS(sphere);
		s.mType=TYPE_SPHERE;
		return s;
	}

	public static function ShapeC(capsule:Capsule):Shape{
		var s:Shape=new Shape();
		s.mCapsule=Capsule.CapsuleC(capsule);
		s.mType=TYPE_CAPSULE;
		return s;
	}

	public function setAABox(box:AABox):void{
		mSphere=null;
		mCapsule=null;
		if(mAABox==null){
			mAABox=AABox.AABoxA(box);
		}
		else{
			mAABox.setA(box);
		}
		mType=TYPE_AABOX;
		if(mSolid!=null){
			internal_clamp();
			mSolid.updateLocalBound();
		}
	}

	public function getAABox():AABox{
		return mAABox;
	}

	public function setSphere(sphere:Sphere):void{
		mAABox=null;
		mCapsule=null;
		if(mSphere==null){
			mSphere=Sphere.SphereS(sphere);
		}
		else{
			mSphere.setS(sphere);
		}
		mType=TYPE_SPHERE;
		if(mSolid!=null){
			mSolid.updateLocalBound();
		}
	}

	public function getSphere():Sphere{
		return mSphere;
	}

	public function setCapsuleC(capsule:Capsule):void{
		mAABox=null;
		mSphere=null;
		if(mCapsule==null){
			mCapsule=Capsule.CapsuleC(capsule);
		}
		else{
			mCapsule.setC(capsule);
		}
		mType=TYPE_CAPSULE;
		if(mSolid!=null){
			mSolid.updateLocalBound();
		}
	}

	public function getCapsule():Capsule{
		return mCapsule;
	}

	public function getType():int{
		return mType;
	}

	public function getBound(box:AABox):void{
		var r:int;

		switch(mType){
			case(TYPE_AABOX):
				box.setA(mAABox);
			break;
			case(TYPE_SPHERE):
			{
				r=mSphere.radius;
				box.mins.x=-r;
				box.mins.y=-r;
				box.mins.z=-r;
				box.maxs.x=r;
				box.maxs.y=r;
				box.maxs.z=r;
				tMath.addAV(box,mSphere.origin);
			}
			break;
			case(TYPE_CAPSULE):
			{
				r=mCapsule.radius;
				var d:Vector3=mCapsule.direction;

				if(d.x<0){
					box.mins.x=d.x-r;	
					box.maxs.x=r;
				}
				else{
					box.mins.x=-r;	
					box.maxs.x=d.x+r;
				}
				if(d.y<0){
					box.mins.y=d.y-r;	
					box.maxs.y=r;
				}
				else{
					box.mins.y=-r;	
					box.maxs.y=d.y+r;
				}
				if(d.z<0){
					box.mins.z=d.z-r;	
					box.maxs.z=r;
				}
				else{
					box.mins.z=-r;	
					box.maxs.z=d.z+r;
				}

				tMath.addAV(box,mCapsule.origin);
			}
			break;
		}
	}

	public function internal_clamp():void{
		if(mSolid!=null && mSolid.mSceneManager!=null){
			switch(mType){
				case(TYPE_AABOX):
					mSolid.mSceneManager.clampPosition(mAABox.mins);
					mSolid.mSceneManager.clampPosition(mAABox.maxs);
				break;
				case(TYPE_SPHERE):
					// Do Nothing
				break;
				case(TYPE_CAPSULE):
					// Do Nothing
				break;
				default:
					// Do Nothing
				break;
			}
		}
	}

	internal var mAABox:AABox=new AABox();
	internal var mSphere:Sphere;
	internal var mCapsule:Capsule;
	
	internal var mType:int;
	internal var mSolid:HopEntity;
}

}
