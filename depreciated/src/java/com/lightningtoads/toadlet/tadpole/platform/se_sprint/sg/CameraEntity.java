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

package com.lightningtoads.toadlet.tadpole.sg;

import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.egg.mathfixed.Math;
import com.lightningtoads.toadlet.tadpole.*;
import com.mascotcapsule.micro3d.v3.*;

public class CameraEntity extends Entity{
	// ProjectionTypes
	public static final byte PT_FOVX=0;
	public static final byte PT_FOVY=1;

	public CameraEntity(Engine engine){
		super(engine);
		mType|=TYPE_CAMERA;
	}

	public void setProjectionFovX(int fovx,int aspect,int near,int far){
		mProjectionType=PT_FOVX;
		mFov=fovx;
		mAspect=aspect;
		mNear=near;
		mFar=far;
		mBoundingRadius=mFar>>1;
	}
	
	public void setProjectionFovY(int fovy,int aspect,int near,int far){
		mProjectionType=PT_FOVY;
		mFov=fovy;
		mAspect=aspect;
		mNear=near;
		mFar=far;
		mBoundingRadius=mFar>>1;
	}

	public void setTransform(AffineTrans transform){
		mTransform=transform;
	}

	public void setLookAt(Vector3 pos,Vector3 dir,Vector3 up){
		Math.normalizeCarefully(mCacheDir,dir,0);
		Math.normalizeCarefully(mCacheUp,up,0);
		Math.cross(mCacheRight,mCacheDir,mCacheUp);
		Math.normalizeCarefully(mCacheRight,0);
		Math.cross(mCacheUp,mCacheRight,mCacheDir);

		mIdentityTransform=false;

		mTranslate.set(pos);

		mRotate.setAt(0,0,mCacheRight.x);
		mRotate.setAt(1,0,mCacheRight.y);
		mRotate.setAt(2,0,mCacheRight.z);
		mRotate.setAt(0,1,mCacheUp.x);
		mRotate.setAt(1,1,mCacheUp.y);
		mRotate.setAt(2,1,mCacheUp.z);
		mRotate.setAt(0,2,-mCacheDir.x);
		mRotate.setAt(1,2,-mCacheDir.y);
		mRotate.setAt(2,2,-mCacheDir.z);

		int bits=mEngine.getSceneManager().mRenderShiftBits;

		mTransform.m00=mCacheRight.x>>4;
		mTransform.m10=mCacheRight.y>>4;
		mTransform.m20=mCacheRight.z>>4;
		mTransform.m01=mCacheUp.x>>4;
		mTransform.m11=mCacheUp.y>>4;
		mTransform.m21=mCacheUp.z>>4;
		mTransform.m02=-mCacheDir.x>>4;
		mTransform.m12=-mCacheDir.y>>4;
		mTransform.m22=-mCacheDir.z>>4;

		mTransform.m03=pos.x>>bits;
		mTransform.m13=pos.y>>bits;
		mTransform.m23=pos.z>>bits;
	}

	public void setViewport(int x,int y,int width,int height){
		mViewportSet=true;
		mViewportX=x;
		mViewportY=y;
		mViewportWidth=width;
		mViewportHeight=height;
	}

	public byte getProjectionType(){
		return mProjectionType;
	}

	public int getFov(){
		return mFov;
	}

	public int getAspect(){
		return mAspect;
	}

	public int getNear(){
		return mNear;
	}

	public int getFar(){
		return mFar;
	}

	public int getViewportX(){
		return mViewportX;
	}

	public int getViewportY(){
		return mViewportY;
	}

	public int getViewportWidth(){
		return mViewportWidth;
	}

	public int getViewportHeight(){
		return mViewportHeight;
	}


	void updateViewTransform(){
		int bits=mEngine.getSceneManager().mRenderShiftBits;

		int wt00=mWorldTransform.m00<<4,wt01=mWorldTransform.m01<<4,wt02=mWorldTransform.m02<<4;
		int wt10=mWorldTransform.m10<<4,wt11=mWorldTransform.m11<<4,wt12=mWorldTransform.m12<<4;
		int wt20=mWorldTransform.m20<<4,wt21=mWorldTransform.m21<<4,wt22=mWorldTransform.m22<<4;
		int wtx=mWorldTransform.m03<<bits,wty=mWorldTransform.m13<<bits,wtz=mWorldTransform.m23<<bits;

		mBoundingOrigin.set(
			//#exec fpcomp.bat "wtx-wt02*mBoundingRadius"
			,
			//#exec fpcomp.bat "wty-wt12*mBoundingRadius"
			,
			//#exec fpcomp.bat "wtz-wt22*mBoundingRadius"
		);

		// For reasons yet unknown to me, mascot capsule requires that we invert the up & forward vectors from what we calculate
		wt01=-wt01;
		wt11=-wt11;
		wt21=-wt21;
		wt02=-wt02;
		wt12=-wt12;
		wt22=-wt22;

		mViewTransform.m00=wt00>>4;
		mViewTransform.m01=wt10>>4;
		mViewTransform.m02=wt20>>4;
		mViewTransform.m10=wt01>>4;
		mViewTransform.m11=wt11>>4;
		mViewTransform.m12=wt21>>4;
		mViewTransform.m20=wt02>>4;
		mViewTransform.m21=wt12>>4;
		mViewTransform.m22=wt22>>4;

		mViewTransform.m03=-
			//#exec fpcomp.bat "(wt00*wtx + wt10*wty + wt20*wtz)"
			>>bits
		;
		mViewTransform.m13=-
			//#exec fpcomp.bat "(wt01*wtx + wt11*wty + wt21*wtz)"
			>>bits
		;
		mViewTransform.m23=-
			//#exec fpcomp.bat "(wt02*wtx + wt12*wty + wt22*wtz)"
			>>bits
		;
	}

	Vector3 mBoundingOrigin=new Vector3();
	byte mProjectionType=PT_FOVX;
	boolean mViewportSet=false;
	int mFov,mAspect,mNear,mFar;
	int mViewportX,mViewportY,mViewportWidth,mViewportHeight;

	AffineTrans mViewTransform=new AffineTrans();

	Vector3 mCacheRight=new Vector3();
	Vector3 mCacheDir=new Vector3();
	Vector3 mCacheUp=new Vector3();
}
