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

public class CameraEntity extends Entity{
	// ProjectionTypes
	public static var PT_FOVX:int=0;
	public static var PT_FOVY:int=1;

	public function CameraEntity(engine:Engine){
		super(engine);
		mType|=TYPE_CAMERA;
	}

	public function setProjectionFovX(fovx:int,aspect:int,nearDistance:int,farDistance:int):void{
		mProjectionType=PT_FOVX;
		mFov=fovx;
		mAspect=aspect;
		mNear=nearDistance;
		mFar=farDistance;

		var h:int=tMath.tan(fovx>>1);
		mInvHX=tMath.div(tMath.ONE,h);
		mInvHY=tMath.div(tMath.ONE,tMath.mul(h,aspect));

		mBoundingRadius=mFar>>1;
	}

	public function setProjectionFovY(fovy:int,aspect:int,nearDistance:int,farDistance:int):void{
		mProjectionType=PT_FOVY;
		mFov=fovy;
		mAspect=aspect;
		mNear=nearDistance;
		mFar=farDistance;

		var h:int=tMath.tan(fovy>>1);
		mInvHX=tMath.div(tMath.ONE,tMath.mul(h,aspect));
		mInvHY=tMath.div(tMath.ONE,h);

		mBoundingRadius=mFar>>1;
	}

	public function setLookAt(pos:Vector3,dir:Vector3,up:Vector3):void{
		tMath.normalizeCarefullyVV(mCacheDir,dir,0);
		tMath.normalizeCarefullyVV(mCacheUp,up,0);
		tMath.crossVVV(mCacheRight,mCacheDir,mCacheUp);
		tMath.normalizeCarefullyV(mCacheRight,0);
		tMath.crossVVV(mCacheUp,mCacheRight,mCacheDir);

		mIdentityTransform=false;

		mTranslate.setV(pos);

		mRotate.a00=mCacheRight.x;
		mRotate.a10=mCacheRight.y;
		mRotate.a20=mCacheRight.z;
		mRotate.a01=mCacheUp.x;
		mRotate.a11=mCacheUp.y;
		mRotate.a21=mCacheUp.z;
		mRotate.a02=-mCacheDir.x;
		mRotate.a12=-mCacheDir.y;
		mRotate.a22=-mCacheDir.z;

		var bits:int=mEngine.mRenderShiftBits;

		mTransform.a00=mCacheRight.x;
		mTransform.a10=mCacheRight.y;
		mTransform.a20=mCacheRight.z;
		mTransform.a01=mCacheUp.x;
		mTransform.a11=mCacheUp.y;
		mTransform.a21=mCacheUp.z;
		mTransform.a02=-mCacheDir.x;
		mTransform.a12=-mCacheDir.y;
		mTransform.a22=-mCacheDir.z;

		mTransform.a03=pos.x>>bits;
		mTransform.a13=pos.y>>bits;
		mTransform.a23=pos.z>>bits;
	}

	public function setViewport(x:int,y:int,width:int,height:int):void{
		mViewportSet=true;
		mViewportX=x;
		mViewportY=y;
		mViewportWidth=width;
		mViewportHeight=height;
	}

	public function setClearBuffers(buffers:int):void{
		mClearBuffers=buffers;
	}

	public function setClearColor(color:int):void{
		mClearColor=color;
	}

	public function getProjectionType():int{
		return mProjectionType;
	}

	public function getFov():int{
		return mFov;
	}

	public function getAspect():int{
		return mAspect;
	}
	
	public function getNear():int{
		return mNear;
	}

	public function getFar():int{
		return mFar;
	}

	public function getViewportX():int{
		return mViewportX;
	}

	public function getViewportY():int{
		return mViewportY;
	}

	public function getViewportWidth():int{
		return mViewportWidth;
	}

	public function getViewportHeight():int{
		return mViewportHeight;
	}

	public function getClearBuffers():int{
		return mClearBuffers;
	}

	public function getClearColor():int{
		return mClearColor;
	}

	internal function updateViewTransform():void{
		var bits:int=mEngine.mRenderShiftBits;

		var wt00:int=mWorldTransform.a00,wt01:int=mWorldTransform.a01,wt02:int=mWorldTransform.a02;
		var wt10:int=mWorldTransform.a10,wt11:int=mWorldTransform.a11,wt12:int=mWorldTransform.a12;
		var wt20:int=mWorldTransform.a20,wt21:int=mWorldTransform.a21,wt22:int=mWorldTransform.a22;
		var wtx:int=mWorldTransform.a03<<bits,wty:int=mWorldTransform.a13<<bits,wtz:int=mWorldTransform.a23<<bits;

		mBoundingOrigin.setXYZ(
			wtx-(((wt02>>2)*(mBoundingRadius>>6))>>8),
			wty-(((wt12>>2)*(mBoundingRadius>>6))>>8),
			wtz-(((wt22>>2)*(mBoundingRadius>>6))>>8)
		);

		mViewTransform.a00=wt00;
		mViewTransform.a01=wt10;
		mViewTransform.a02=wt20;
		mViewTransform.a10=wt01;
		mViewTransform.a11=wt11;
		mViewTransform.a12=wt21;
		mViewTransform.a20=wt02;
		mViewTransform.a21=wt12;
		mViewTransform.a22=wt22;

		mViewTransform.a03=-
			((((wt00>>2)*(wtx>>6))>>8) + (((wt10>>2)*(wty>>6))>>8) + (((wt20>>2)*(wtz>>6))>>8))
			>>bits
		;
		mViewTransform.a13=-
			((((wt01>>2)*(wtx>>6))>>8) + (((wt11>>2)*(wty>>6))>>8) + (((wt21>>2)*(wtz>>6))>>8))
			>>bits
		;
		mViewTransform.a23=-
			((((wt02>>2)*(wtx>>6))>>8) + (((wt12>>2)*(wty>>6))>>8) + (((wt22>>2)*(wtz>>6))>>8))
			>>bits
		;
	}

	public var mBoundingOrigin:Vector3=new Vector3();
	public var mProjectionType:int=PT_FOVX;
	public var mFov:int,mAspect:int,mNear:int,mFar:int;
	public var mViewportSet:Boolean;
	public var mViewportX:int,mViewportY:int,mViewportWidth:int,mViewportHeight:int;
	public var mClearBuffers:int;
	public var mClearColor:int;

	public var mViewTransform:Matrix4x3=new Matrix4x3();
	public var mInvHX:int,mInvHY:int;

	protected var mCacheRight:Vector3=new Vector3();
	protected var mCacheDir:Vector3=new Vector3();
	protected var mCacheUp:Vector3=new Vector3();
}

}
