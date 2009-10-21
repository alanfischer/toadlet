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

public class CameraEntity extends Entity{
	// ProjectionTypes
	public static final byte PT_FOVX=0;
	public static final byte PT_FOVY=1;

	// Buffers
	public static final int BUFFER_COLOR=1<<0;
	public static final int BUFFER_DEPTH=1<<1;

	public CameraEntity(Engine engine){
		super(engine);
		mType|=TYPE_CAMERA;
	}

	public void setProjectionFovX(int fovx,int aspect,int nearDistance,int farDistance){
		mProjectionType=PT_FOVX;
		mFov=fovx;
		mAspect=aspect;
		mNear=nearDistance;
		mFar=farDistance;

		int h=Math.tan(fovx>>1);

		mProjectionTransform.set(
			Math.div(Math.ONE,h),0,0,0,
			0,Math.div(Math.ONE,Math.mul(h,aspect)),0,0,
			0,0,Math.div(farDistance+nearDistance,nearDistance-farDistance),Math.div(Math.mul(farDistance,nearDistance)<<1,nearDistance-farDistance),
			0,0,-Math.ONE,0);

		mBoundingRadius=mFar>>1;
	}

	public void setProjectionFovY(int fovy,int aspect,int nearDistance,int farDistance){
		mProjectionType=PT_FOVY;
		mFov=fovy;
		mAspect=aspect;
		mNear=nearDistance;
		mFar=farDistance;

		int h=Math.tan(fovy>>1);

		mProjectionTransform.set(
			Math.div(Math.ONE,Math.mul(h,aspect)),0,0,0,
			0,Math.div(Math.ONE,h),0,0,
			0,0,Math.div(farDistance+nearDistance,nearDistance-farDistance),Math.div(Math.mul(farDistance,nearDistance)<<1,nearDistance-farDistance),
			0,0,-Math.ONE,0);

		mBoundingRadius=mFar>>1;
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

		mTransform.setAt(0,0,mCacheRight.x);
		mTransform.setAt(1,0,mCacheRight.y);
		mTransform.setAt(2,0,mCacheRight.z);
		mTransform.setAt(0,1,mCacheUp.x);
		mTransform.setAt(1,1,mCacheUp.y);
		mTransform.setAt(2,1,mCacheUp.z);
		mTransform.setAt(0,2,-mCacheDir.x);
		mTransform.setAt(1,2,-mCacheDir.y);
		mTransform.setAt(2,2,-mCacheDir.z);

		mTransform.setAt(0,3,pos.x);
		mTransform.setAt(1,3,pos.y);
		mTransform.setAt(2,3,pos.z);
	}

	public void setViewport(int x,int y,int width,int height){
		mViewportSet=true;
		mViewportX=x;
		mViewportY=y;
		mViewportWidth=width;
		mViewportHeight=height;
	}

	public void setClearBuffers(int buffers){mClearBuffers=buffers;}
	public void setClearColor(int color){mClearColor=color;}
	public void setSkipFirstBufferClear(boolean skip){mSkipFirstBufferClear=skip;}

	public byte getProjectionType(){return mProjectionType;}
	public int getFov(){return mFov;}
	public int getAspect(){return mAspect;}
	public int getNear(){return mNear;}
	public int getFar(){return mFar;}

	public boolean getViewportSet(){return mViewportSet;}
	public int getViewportX(){return mViewportX;}
	public int getViewportY(){return mViewportY;}
	public int getViewportWidth(){return mViewportWidth;}
	public int getViewportHeight(){return mViewportHeight;}

	public int getClearBuffers(){return mClearBuffers;}
	public int getClearColor(){return mClearColor;}
	public boolean setSkipFirstBufferClear(){return mSkipFirstBufferClear;}

	public Matrix4x4 getViewTransform(){return mViewTransform;}
	public Matrix4x4 getProjectionTransform(){return mProjectionTransform;}

	protected void updateViewTransform(){
		int wt00=mWorldTransform.at(0,0),wt01=mWorldTransform.at(0,1),wt02=mWorldTransform.at(0,2);
		int wt10=mWorldTransform.at(1,0),wt11=mWorldTransform.at(1,1),wt12=mWorldTransform.at(1,2);
		int wt20=mWorldTransform.at(2,0),wt21=mWorldTransform.at(2,1),wt22=mWorldTransform.at(2,2);
		int wtx=mWorldTransform.at(0,3),wty=mWorldTransform.at(1,3),wtz=mWorldTransform.at(2,3);

		mBoundingOrigin.set(
			//#exec fpcomp.bat "wtx-wt02*mBoundingRadius"
			,
			//#exec fpcomp.bat "wty-wt12*mBoundingRadius"
			,
			//#exec fpcomp.bat "wtz-wt22*mBoundingRadius"
		);

		mViewTransform.setAt(0,0,wt00);
		mViewTransform.setAt(0,1,wt10);
		mViewTransform.setAt(0,2,wt20);
		mViewTransform.setAt(1,0,wt01);
		mViewTransform.setAt(1,1,wt11);
		mViewTransform.setAt(1,2,wt21);
		mViewTransform.setAt(2,0,wt02);
		mViewTransform.setAt(2,1,wt12);
		mViewTransform.setAt(2,2,wt22);

		mViewTransform.setAt(0,3,-
			//#exec fpcomp.bat "(wt00*wtx + wt10*wty + wt20*wtz)"
		);
		mViewTransform.setAt(1,3,-
			//#exec fpcomp.bat "(wt01*wtx + wt11*wty + wt21*wtz)"
		);
		mViewTransform.setAt(2,3,-
			//#exec fpcomp.bat "(wt02*wtx + wt12*wty + wt22*wtz)"
		);
	}

	Vector3 mBoundingOrigin=new Vector3();
	byte mProjectionType=PT_FOVX;
	int mFov,mAspect,mNear,mFar;
	boolean mViewportSet;
	int mViewportX,mViewportY,mViewportWidth,mViewportHeight;
	int mClearBuffers;
	int mClearColor;
	boolean mSkipFirstBufferClear;

	Matrix4x4 mProjectionTransform=new Matrix4x4();
	Matrix4x4 mViewTransform=new Matrix4x4();

	Vector3 mCacheRight=new Vector3();
	Vector3 mCacheDir=new Vector3();
	Vector3 mCacheUp=new Vector3();
}
