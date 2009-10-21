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
import javax.microedition.m3g.*;

public class CameraEntity extends Entity{
	// ProjectionTypes
	public static final byte PT_FOVX=0;
	public static final byte PT_FOVY=1;

	public CameraEntity(Engine engine){
		super(engine);
		mType|=TYPE_CAMERA;
		
		mCamera=new Camera();
		mNode=mCamera;

		mEngine.getSceneManager().mWorld.setActiveCamera(mCamera);
	}
	
	public void setProjectionFovX(int fovx,int aspect,int near,int far){
		mProjectionType=PT_FOVX;
		mFov=fovx;
		mAspect=aspect;
		mNear=near;
		mFar=far;

		float fh=Math.fixedToFloat(Math.tan(fovx>>1));
		float faspect=Math.fixedToFloat(aspect);
		float fnear=Math.fixedToFloat(near);
		float ffar=Math.fixedToFloat(far);

		mProjection.set(new float[]{
			1.0f/fh,0,0,0,
			0,1.0f/(fh*faspect),0,0,
			0,0,(ffar+fnear)/(fnear-ffar),(2*ffar*fnear)/(fnear-ffar),
			0,0,-1.0f,0});
			
		mCamera.setGeneric(mProjection);
	}

	public void setProjectionFovY(int fovy,int aspect,int near,int far){
		mProjectionType=PT_FOVY;
		mFov=fovy;
		mAspect=aspect;
		mNear=near;
		mFar=far;

		float fh=Math.fixedToFloat(Math.tan(fovy>>1));
		float faspect=Math.fixedToFloat(aspect);
		float fnear=Math.fixedToFloat(near);
		float ffar=Math.fixedToFloat(far);

		mProjection.set(new float[]{
			1.0f/(fh*faspect),0,0,0,
			0,1.0f/fh,0,0,
			0,0,(ffar+fnear)/(fnear-ffar),(2*ffar*fnear)/(fnear-ffar),
			0,0,-1.0f,0});
			
		mCamera.setGeneric(mProjection);
	}

	public void setLookAt(Vector3 pos,Vector3 dir,Vector3 up){
		Math.normalizeCarefully(mCacheDir,dir,0);
		Math.normalizeCarefully(mCacheUp,up,0);
		Math.cross(mCacheRight,mCacheDir,mCacheUp);
		Math.normalizeCarefully(mCacheRight,0);
		Math.cross(mCacheUp,mCacheRight,mCacheDir);

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

		if(mTransform==null){
			mTransform=new Transform();
			mTransformData=new float[16];
			mTransformData[15]=1.0f;
		}

		mTransformData[0]=Math.fixedToFloat(mCacheRight.x);
		mTransformData[4]=Math.fixedToFloat(mCacheRight.y);
		mTransformData[8]=Math.fixedToFloat(mCacheRight.z);
		mTransformData[1]=Math.fixedToFloat(mCacheUp.x);
		mTransformData[5]=Math.fixedToFloat(mCacheUp.y);
		mTransformData[9]=Math.fixedToFloat(mCacheUp.z);
		mTransformData[2]=-Math.fixedToFloat(mCacheDir.x);
		mTransformData[6]=-Math.fixedToFloat(mCacheDir.y);
		mTransformData[10]=-Math.fixedToFloat(mCacheDir.z);

		mTransformData[3]=Math.fixedToFloat(pos.x);
		mTransformData[7]=Math.fixedToFloat(pos.y);
		mTransformData[11]=Math.fixedToFloat(pos.z);
			
		mTransform.set(mTransformData);
		mNode.setTransform(mTransform);
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

	Transform mProjection=new Transform();
	Camera mCamera;
	byte mProjectionType=PT_FOVX;
	int mFov,mAspect,mNear,mFar;
	boolean mViewportSet=false;
	int mViewportX,mViewportY,mViewportWidth,mViewportHeight;
	Vector3 mCacheRight=new Vector3();
	Vector3 mCacheDir=new Vector3();
	Vector3 mCacheUp=new Vector3();
}
