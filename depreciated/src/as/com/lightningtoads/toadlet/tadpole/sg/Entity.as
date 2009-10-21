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

package com.lightningtoads.toadlet.tadpole.sg{

import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.egg.mathfixed.tMath;
import com.lightningtoads.toadlet.tadpole.*;

public class Entity{
	public static var TYPE_NODE:int=1<<0;
	public static var TYPE_RENDERABLE:int=1<<1;
	public static var TYPE_MESH:int=1<<2;
	public static var TYPE_SPRITE:int=1<<3;
	public static var TYPE_EVENT:int=1<<4;
	public static var TYPE_CAMERA:int=1<<5;
	public static var TYPE_LIGHT:int=1<<6;
	public static var TYPE_BEAM:int=1<<7;

	public function Entity(engine:Engine){
		if(mEngine==null){
			mEngine=engine;
		}
		mScope=-1;
	}

	public function instanceOf(type:int):Boolean{
		return (mType&type)>0;
	}

	public function getType():int{
		return mType;
	}

	public function destroy():void{
		if(mParent!=null){
			mParent.remove(this);
		}
		mDestroyed=true;
	}

	public function destroyed():Boolean{
		return mDestroyed;
	}

	public function parentChanged(entity:Entity,parent:NodeEntity):void{
		if(entity==this){
			mParent=parent;
		}
	}

	public function getParent():NodeEntity{
		return mParent;
	}

	public function setTranslateV(translate:Vector3):void{
		mTranslate.setV(translate);

		var bits:int=mEngine.mRenderShiftBits;

		mTransform.a03=mTranslate.x>>bits;
		mTransform.a13=mTranslate.y>>bits;
		mTransform.a23=mTranslate.z>>bits;

		mIdentityTransform=false;
	}

	public function setTranslateXYZ(x:int,y:int,z:int):void{
		mTranslate.setXYZ(x,y,z);

		var bits:int=mEngine.mRenderShiftBits;

		mTransform.a03=mTranslate.x>>bits;
		mTransform.a13=mTranslate.y>>bits;
		mTransform.a23=mTranslate.z>>bits;

		mIdentityTransform=false;
	}

	public function getTranslate():Vector3{
		return mTranslate;
	}

	public function setRotateM(rotate:Matrix3x3):void{ 
		mRotate.setM(rotate);

		mTransform.a00=((mRotate.a00>>2)*(mScale.x>>6))>>8;
		mTransform.a10=((mRotate.a10>>2)*(mScale.x>>6))>>8;
		mTransform.a20=((mRotate.a20>>2)*(mScale.x>>6))>>8;
		mTransform.a01=((mRotate.a01>>2)*(mScale.y>>6))>>8;
		mTransform.a11=((mRotate.a11>>2)*(mScale.y>>6))>>8;
		mTransform.a21=((mRotate.a21>>2)*(mScale.y>>6))>>8;
		mTransform.a02=((mRotate.a02>>2)*(mScale.z>>6))>>8;
		mTransform.a12=((mRotate.a12>>2)*(mScale.z>>6))>>8;
		mTransform.a22=((mRotate.a22>>2)*(mScale.z>>6))>>8;

		mIdentityTransform=false;
	}

	public function setScaleV(scale:Vector3):void{
		mScale.setV(scale);

		mTransform.a00=((mRotate.a00>>2)*(mScale.x>>6))>>8;
		mTransform.a10=((mRotate.a10>>2)*(mScale.x>>6))>>8;
		mTransform.a20=((mRotate.a20>>2)*(mScale.x>>6))>>8;
		mTransform.a01=((mRotate.a01>>2)*(mScale.y>>6))>>8;
		mTransform.a11=((mRotate.a11>>2)*(mScale.y>>6))>>8;
		mTransform.a21=((mRotate.a21>>2)*(mScale.y>>6))>>8;
		mTransform.a02=((mRotate.a02>>2)*(mScale.z>>6))>>8;
		mTransform.a12=((mRotate.a12>>2)*(mScale.z>>6))>>8;
		mTransform.a22=((mRotate.a22>>2)*(mScale.z>>6))>>8;

		mIdentityTransform=false;
	}

	public function setScaleXYZ(x:int,y:int,z:int):void{
		mScale.setXYZ(x,y,z);

		mTransform.a00=((mRotate.a00>>2)*(mScale.x>>6))>>8;
		mTransform.a10=((mRotate.a10>>2)*(mScale.x>>6))>>8;
		mTransform.a20=((mRotate.a20>>2)*(mScale.x>>6))>>8;
		mTransform.a01=((mRotate.a01>>2)*(mScale.y>>6))>>8;
		mTransform.a11=((mRotate.a11>>2)*(mScale.y>>6))>>8;
		mTransform.a21=((mRotate.a21>>2)*(mScale.y>>6))>>8;
		mTransform.a02=((mRotate.a02>>2)*(mScale.z>>6))>>8;
		mTransform.a12=((mRotate.a12>>2)*(mScale.z>>6))>>8;
		mTransform.a22=((mRotate.a22>>2)*(mScale.z>>6))>>8;

		mIdentityTransform=false;
	}

	public function getScale():Vector3{
		return mScale;
	}

	public function getRotate():Matrix3x3{
		return mRotate;
	}

	public function setScope(scope:int):void{
		mScope=scope;
	}

	public function getScope():int{
		return mScope;
	}

	public function setBoundingRadius(bound:int):void{
		mBoundingRadius=bound;
	}

	public function getBoundingRadius():int{
		return mBoundingRadius;
	}

	public function getEngine():Engine{
		return mEngine;
	}

	protected var mDestroyed:Boolean;
	protected var mEngine:Engine;
	protected var mType:int;

	protected var mParent:NodeEntity;

	public var mIdentityTransform:Boolean=true;
	public var mTranslate:Vector3=new Vector3();
	public var mRotate:Matrix3x3=new Matrix3x3();
	public var mScale:Vector3=Vector3.Vector3XYZ(tMath.ONE,tMath.ONE,tMath.ONE);
	public var mScope:int=-1;
	public var mBoundingRadius:int=-tMath.ONE;

	public var mTransform:Matrix4x3=new Matrix4x3();
	public var mWorldTransform:Matrix4x3=new Matrix4x3();
}

}
