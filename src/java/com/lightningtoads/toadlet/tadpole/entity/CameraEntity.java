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

package com.lightningtoads.toadlet.tadpole.entity;

#include <com/lightningtoads/toadlet/tadpole/Types.h>

import com.lightningtoads.toadlet.peeper.*;
import com.lightningtoads.toadlet.tadpole.*;

public class CameraEntity extends Entity{
	enum ProjectionType{
		FOVX,
		FOVY,
		ORTHO,
		FRUSTUM,
		MATRIX
	}

	public CameraEntity(){super();}

	public Entity create(Engine engine){
		super.create(engine);

		setProjectionFovX(Math.HALF_PI,Math.fromInt(1),Math.fromInt(1),Math.fromInt(100));
		mViewportSet=false;
		mViewport.reset();
		mClearFlags=Renderer.ClearFlag.COLOR|Renderer.ClearFlag.DEPTH;
		mClearColor.reset();
		mSkipFirstClear=false;

		updateViewTransform();

		mNumCulledEntities=0;

		mFPSLastTime=0;
		mFPSFrameCount=0;
		mFPS=0;

		return this;
	}
	
	public void setProjectionFovX(scalar fovx,scalar aspect,scalar nearDist,scalar farDist){
		mProjectionType=ProjectionType.FOVX;
		mFov=fovx;mAspect=aspect;
		mNearDist=nearDist;mFarDist=farDist;

		Math.setMatrix4x4FromPerspectiveX(mProjectionTransform,fovx,aspect,nearDist,farDist);

		mBoundingRadius=mFarDist/2;
	}

	public void setProjectionFovY(scalar fovy,scalar aspect,scalar nearDist,scalar farDist){
		mProjectionType=ProjectionType.FOVY;
		mFov=fovy;mAspect=aspect;
		mNearDist=nearDist;mFarDist=farDist;

		Math.setMatrix4x4FromPerspectiveY(mProjectionTransform,fovy,aspect,nearDist,farDist);

		mBoundingRadius=mFarDist/2;
	}

	public void setProjectionOrtho(scalar leftDist,scalar rightDist,scalar bottomDist,scalar topDist,scalar nearDist,scalar farDist){
		mProjectionType=ProjectionType.ORTHO;
		mLeftDist=leftDist;mRightDist=rightDist;
		mBottomDist=bottomDist;mTopDist=topDist;
		mNearDist=nearDist;mFarDist=farDist;

		Math.setMatrix4x4FromOrtho(mProjectionTransform,leftDist,rightDist,bottomDist,topDist,nearDist,farDist);

		mBoundingRadius=mFarDist/2;
	}

	public void setProjectionFrustum(scalar leftDist,scalar rightDist,scalar bottomDist,scalar topDist,scalar nearDist,scalar farDist){
		mProjectionType=ProjectionType.FRUSTUM;
		mLeftDist=leftDist;mRightDist=rightDist;
		mBottomDist=bottomDist;mTopDist=topDist;
		mNearDist=nearDist;mFarDist=farDist;

		Math.setMatrix4x4FromFrustum(mProjectionTransform,leftDist,rightDist,bottomDist,topDist,nearDist,farDist);

		mBoundingRadius=mFarDist/2;
	}
	
	public void setProjectionTransform(Matrix4x4 transform){
		mProjectionType=ProjectionType.MATRIX;

		mProjectionTransform.set(transform);

		/// @todo  need to set mBoundingRadius
	}
	
	public void setProjectionRotation(scalar rotate){
		Matrix4x4 projection=cache_setProjectionRotation_projection.reset();

		scalar x=mViewport.x+mViewport.width/2;
		scalar y=mViewport.y+mViewport.height/2;

		Math.setMatrix4x4FromTranslate(projection,x,y,0);
		Math.preMul(projection,mProjectionTransform);

		projection.reset();
		Math.setMatrix4x4FromZ(projection,rotate);
		Math.postMul(mProjectionTransform,projection);

		projection.reset();
		Math.setMatrix4x4FromTranslate(projection,-x,-y,0);
	}

	public Matrix4x4 getProjectionTransform(){return mProjectionTransform;}

	public void setLookAt(Vector3 eye,Vector3 point,Vector3 up){
		Math.setMatrix4x4FromLookAt(mVisualTransform,eye,point,up,true);

		Math.setVector3FromMatrix4x4(mTranslate,mVisualTransform);
		Math.setMatrix3x3FromMatrix4x4(mRotate,mVisualTransform);

		mIdentityTransform=false;
		modified();
	}
	
	public void setLookDir(Vector3 eye,Vector3 dir,Vector3 up){
		Math.setMatrix4x4FromLookDir(mVisualTransform,eye,dir,up,true);

		Math.setVector3FromMatrix4x4(mTranslate,mVisualTransform);
		Math.setMatrix3x3FromMatrix4x4(mRotate,mVisualTransform);

		mIdentityTransform=false;
		modified();
	}

	public void setViewport(Viewport viewport){
		mViewportSet=true;
		mViewport.set(viewport);
	}
	
	public void setViewport(int x,int y,int width,int height){
		mViewportSet=true;
		mViewport.set(x,y,width,height);
	}

	public void setClearFlags(int clearFlags){
		mClearFlags=clearFlags;
	}

	public void setClearColor(Color clearColor){
		mClearColor.set(clearColor);
	}
	
	public void setSkipFirstClear(boolean skip){
		mSkipFirstClear=skip;
	}

	public ProjectionType getProjectionType(){return mProjectionType;}
	public scalar getFov(){return mFov;}
	public scalar getAspect(){return mAspect;}
	public scalar getLeftDist(){return mLeftDist;}
	public scalar getRightDist(){return mRightDist;}
	public scalar getBottomDist(){return mBottomDist;}
	public scalar getTopDist(){return mTopDist;}
	public scalar getNearDist(){return mNearDist;}
	public scalar getFarDist(){return mFarDist;}

	public boolean getViewportSet(){return mViewportSet;}
	public Viewport getViewport(){return mViewport;}
	public int getViewportX(){return mViewport.x;}
	public int getViewportY(){return mViewport.y;}
	public int getViewportWidth(){return mViewport.width;}
	public int getViewportHeight(){return mViewport.height;}

	public int getClearFlags(){return mClearFlags;}
	public Color getClearColor(){return mClearColor;}
	public boolean getSkipFirstClear(){return mSkipFirstClear;}

	public void updateViewTransform(){
		scalar wt00=mVisualWorldTransform.at(0,0),wt01=mVisualWorldTransform.at(0,1),wt02=mVisualWorldTransform.at(0,2);
		scalar wt10=mVisualWorldTransform.at(1,0),wt11=mVisualWorldTransform.at(1,1),wt12=mVisualWorldTransform.at(1,2);
		scalar wt20=mVisualWorldTransform.at(2,0),wt21=mVisualWorldTransform.at(2,1),wt22=mVisualWorldTransform.at(2,2);
		mVisualWorldTranslate.x=mVisualWorldTransform.at(0,3);mVisualWorldTranslate.y=mVisualWorldTransform.at(1,3);mVisualWorldTranslate.z=mVisualWorldTransform.at(2,3);

		mVisualBoundingOrigin.set(mVisualWorldTranslate.x-Math.mul(wt02,mBoundingRadius),mVisualWorldTranslate.y-Math.mul(wt12,mBoundingRadius),mVisualWorldTranslate.z-Math.mul(wt22,mBoundingRadius));

		mViewTransform.setAt(0,0,wt00);
		mViewTransform.setAt(0,1,wt10);
		mViewTransform.setAt(0,2,wt20);
		mViewTransform.setAt(1,0,wt01);
		mViewTransform.setAt(1,1,wt11);
		mViewTransform.setAt(1,2,wt21);
		mViewTransform.setAt(2,0,wt02);
		mViewTransform.setAt(2,1,wt12);
		mViewTransform.setAt(2,2,wt22);

		mViewTransform.setAt(0,3,-(Math.mul(wt00,mVisualWorldTranslate.x) + Math.mul(wt10,mVisualWorldTranslate.y) + Math.mul(wt20,mVisualWorldTranslate.z)));
		mViewTransform.setAt(1,3,-(Math.mul(wt01,mVisualWorldTranslate.x) + Math.mul(wt11,mVisualWorldTranslate.y) + Math.mul(wt21,mVisualWorldTranslate.z)));
		mViewTransform.setAt(2,3,-(Math.mul(wt02,mVisualWorldTranslate.x) + Math.mul(wt12,mVisualWorldTranslate.y) + Math.mul(wt22,mVisualWorldTranslate.z)));

		// Update frustum planes
		Math.mul(mViewProjectionTransform,mProjectionTransform,mViewTransform);
		scalar[] vpt=mViewProjectionTransform.data;
		// Right clipping plane.
		Math.normalize(mClipPlanes[0].set(vpt[3]-vpt[0], vpt[7]-vpt[4], vpt[11]-vpt[8], vpt[15]-vpt[12]));
		// Left clipping plane.
		Math.normalize(mClipPlanes[1].set(vpt[3]+vpt[0], vpt[7]+vpt[4], vpt[11]+vpt[8], vpt[15]+vpt[12]));
		// Bottom clipping plane.
		Math.normalize(mClipPlanes[2].set(vpt[3]+vpt[1], vpt[7]+vpt[5], vpt[11]+vpt[9], vpt[15]+vpt[13]));
		// Top clipping plane.
		Math.normalize(mClipPlanes[3].set(vpt[3]-vpt[1], vpt[7]-vpt[5], vpt[11]-vpt[9], vpt[15]-vpt[13]));
		// Far clipping plane.
		Math.normalize(mClipPlanes[4].set(vpt[3]-vpt[2], vpt[7]-vpt[6], vpt[11]-vpt[10], vpt[15]-vpt[14]));
		// Near clipping plane.
		Math.normalize(mClipPlanes[5].set(vpt[3]+vpt[2], vpt[7]+vpt[6], vpt[11]+vpt[10], vpt[15]+vpt[14]));
	}

	public Matrix4x4 getViewTransform(){return mViewTransform;}  /// Only valid during rendering
	public Vector3 getVisualWorldTranslate(){return mVisualWorldTranslate;}  /// Only valid during rendering

	public boolean culled(Sphere sphere){
		scalar distance=0;
		int i;
		for(i=0;i<6;++i){
			distance=Math.dot(mClipPlanes[i].normal,sphere.origin)+mClipPlanes[i].d;
			if(distance<-sphere.radius){
				return true;
			}
		}
		return false;
	}

	public boolean culled(AABox box){
		Vector3 vertex=cache_culled_vertex;
		int i;
		for(i=0;i<6;i++){
			box.findPVertex(vertex,mClipPlanes[i].normal);
			if(Math.dot(mClipPlanes[i].normal,vertex)+mClipPlanes[i].d<0){
				return true;
			}
		}
		return false;
	}

	public int getNumCulledEntities(){return mNumCulledEntities;}

	public void updateFramesPerSecond(){
		mFPSFrameCount++;
		int fpsTime=mEngine.getScene().getVisualTime();
		if(mFPSLastTime==0 || fpsTime-mFPSLastTime>5000){
			scalar fps=0;
			if(mFPSLastTime>0){
				fps=Math.div(Math.fromInt(mFPSFrameCount),Math.fromMilli(fpsTime-mFPSLastTime));
			}
			mFPSLastTime=fpsTime;
			mFPSFrameCount=0;
			mFPS=fps;
		}
	}

	public scalar getFramesPerSecond(){return mFPS;}

	ProjectionType mProjectionType=ProjectionType.FOVX;
	scalar mFov=0,mAspect=0;
	scalar mLeftDist=0,mRightDist=0;
	scalar mBottomDist=0,mTopDist=0;
	scalar mNearDist=0,mFarDist=0;
	Matrix4x4 mProjectionTransform=new Matrix4x4();
	boolean mViewportSet=false;
	Viewport mViewport=new Viewport();
	int mClearFlags=0;
	Color mClearColor=new Color();
	boolean mSkipFirstClear=false;

	Vector3 mVisualBoundingOrigin=new Vector3();
	Vector3 mVisualWorldTranslate=new Vector3();
	Matrix4x4 mViewTransform=new Matrix4x4();
	Matrix4x4 mViewProjectionTransform=new Matrix4x4();
	Plane[] mClipPlanes=new Plane[6];{for(int i=0;i<mClipPlanes.length;++i)mClipPlanes[i]=new Plane();}

	public int mNumCulledEntities=0;

	public int mFPSLastTime=0;
	public int mFPSFrameCount=0;
	public scalar mFPS=0;

	Matrix4x4 cache_setProjectionRotation_projection=new Matrix4x4();
	Vector3 cache_culled_vertex=new Vector3();
}
