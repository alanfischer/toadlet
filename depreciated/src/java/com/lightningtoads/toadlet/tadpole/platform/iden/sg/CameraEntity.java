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
import com.motorola.iden.micro3d.*;

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

	public void setTransform(AffineTransform transform){
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

		mTransform.set(AffineTransform.M00,mCacheRight.x>>4);
		mTransform.set(AffineTransform.M10,mCacheRight.y>>4);
		mTransform.set(AffineTransform.M20,mCacheRight.z>>4);
		mTransform.set(AffineTransform.M01,mCacheUp.x>>4);
		mTransform.set(AffineTransform.M11,mCacheUp.y>>4);
		mTransform.set(AffineTransform.M21,mCacheUp.z>>4);
		mTransform.set(AffineTransform.M02,-mCacheDir.x>>4);
		mTransform.set(AffineTransform.M12,-mCacheDir.y>>4);
		mTransform.set(AffineTransform.M22,-mCacheDir.z>>4);

		mTransform.set(AffineTransform.M03,pos.x>>bits);
		mTransform.set(AffineTransform.M13,pos.y>>bits);
		mTransform.set(AffineTransform.M23,pos.z>>bits);
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

		int wt00=mWorldTransform.get(AffineTransform.M00)<<4,wt01=mWorldTransform.get(AffineTransform.M01)<<4,wt02=mWorldTransform.get(AffineTransform.M02)<<4;
		int wt10=mWorldTransform.get(AffineTransform.M10)<<4,wt11=mWorldTransform.get(AffineTransform.M11)<<4,wt12=mWorldTransform.get(AffineTransform.M12)<<4;
		int wt20=mWorldTransform.get(AffineTransform.M20)<<4,wt21=mWorldTransform.get(AffineTransform.M21)<<4,wt22=mWorldTransform.get(AffineTransform.M22)<<4;
		int wtx=mWorldTransform.get(AffineTransform.M03)<<bits,wty=mWorldTransform.get(AffineTransform.M13)<<bits,wtz=mWorldTransform.get(AffineTransform.M23)<<bits;

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

		mViewTransform.set(AffineTransform.M00,wt00>>4);
		mViewTransform.set(AffineTransform.M01,wt10>>4);
		mViewTransform.set(AffineTransform.M02,wt20>>4);
		mViewTransform.set(AffineTransform.M10,wt01>>4);
		mViewTransform.set(AffineTransform.M11,wt11>>4);
		mViewTransform.set(AffineTransform.M12,wt21>>4);
		mViewTransform.set(AffineTransform.M20,wt02>>4);
		mViewTransform.set(AffineTransform.M21,wt12>>4);
		mViewTransform.set(AffineTransform.M22,wt22>>4);

		mViewTransform.set(AffineTransform.M03,-
			//#exec fpcomp.bat "(wt00*wtx + wt10*wty + wt20*wtz)"
			>>bits
		);
		mViewTransform.set(AffineTransform.M13,-
			//#exec fpcomp.bat "(wt01*wtx + wt11*wty + wt21*wtz)"
			>>bits
		);
		mViewTransform.set(AffineTransform.M23,-
			//#exec fpcomp.bat "(wt02*wtx + wt12*wty + wt22*wtz)"
			>>bits
		);
	}

	Vector3 mBoundingOrigin=new Vector3();
	byte mProjectionType=PT_FOVX;
	int mFov,mAspect,mNear,mFar;
	boolean mViewportSet=false;
	int mViewportX,mViewportY,mViewportWidth,mViewportHeight;

	AffineTransform mViewTransform=new AffineTransform();
	
	Vector3 mCacheRight=new Vector3();
	Vector3 mCacheDir=new Vector3();
	Vector3 mCacheUp=new Vector3();
}
