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

public class Entity{
	public final static int TYPE_NODE=1<<0;
	public final static int TYPE_RENDERABLE=1<<1;
	public final static int TYPE_MESH=1<<2;
	public final static int TYPE_SPRITE=1<<3;
	public final static int TYPE_EVENT=1<<4;
	public final static int TYPE_CAMERA=1<<5;
	public final static int TYPE_LIGHT=1<<6;
	public final static int TYPE_BEAM=1<<7;

	public Entity(Engine engine){
		if(mEngine==null){
			mEngine=engine;
		}

		mTransform.setIdentity();
	}

	public final boolean instanceOf(int type){
		return (mType&type)>0;
	}

	public final int getType(){
		return mType;
	}

	public void destroy(){
		if(mParent!=null){
			mParent.remove(this);
		}
		mDestroyed=true;
	}

	public final boolean destroyed(){
		return mDestroyed;
	}

	public void parentChanged(Entity entity,NodeEntity parent){
		if(entity==this){
			mParent=parent;
		}
	}

	public final NodeEntity getParent(){
		return mParent;
	}

	public void setTranslate(Vector3 translate){
		mTranslate.set(translate);

		int bits=mEngine.getSceneManager().mRenderShiftBits;

		mTransform.set(AffineTransform.M03,mTranslate.x>>bits);
		mTransform.set(AffineTransform.M13,mTranslate.y>>bits);
		mTransform.set(AffineTransform.M23,mTranslate.z>>bits);

		mIdentityTransform=false;
	}

	public void setTranslate(int x,int y,int z){
		mTranslate.set(x,y,z);

		int bits=mEngine.getSceneManager().mRenderShiftBits;

		mTransform.set(AffineTransform.M03,mTranslate.x>>bits);
		mTransform.set(AffineTransform.M13,mTranslate.y>>bits);
		mTransform.set(AffineTransform.M23,mTranslate.z>>bits);

		mIdentityTransform=false;
	}

	public final Vector3 getTranslate(){
		return mTranslate;
	}

	public void setRotate(Matrix3x3 rotate){ 
		mRotate.set(rotate);

		mTransform.set(AffineTransform.M00,Math.mul(mRotate.at(0,0),mScale.x)>>4);
		mTransform.set(AffineTransform.M10,Math.mul(mRotate.at(1,0),mScale.x)>>4);
		mTransform.set(AffineTransform.M20,Math.mul(mRotate.at(2,0),mScale.x)>>4);
		mTransform.set(AffineTransform.M01,Math.mul(mRotate.at(0,1),mScale.y)>>4);
		mTransform.set(AffineTransform.M11,Math.mul(mRotate.at(1,1),mScale.y)>>4);
		mTransform.set(AffineTransform.M21,Math.mul(mRotate.at(2,1),mScale.y)>>4);
		mTransform.set(AffineTransform.M02,Math.mul(mRotate.at(0,2),mScale.z)>>4);
		mTransform.set(AffineTransform.M12,Math.mul(mRotate.at(1,2),mScale.z)>>4);
		mTransform.set(AffineTransform.M22,Math.mul(mRotate.at(2,2),mScale.z)>>4);

		mIdentityTransform=false;
	}

	public final Matrix3x3 getRotate(){
		return mRotate;
	}

	public void setScale(Vector3 scale){
		mScale.set(scale);

		mTransform.set(AffineTransform.M00,Math.mul(mRotate.at(0,0),mScale.x)>>4);
		mTransform.set(AffineTransform.M10,Math.mul(mRotate.at(1,0),mScale.x)>>4);
		mTransform.set(AffineTransform.M20,Math.mul(mRotate.at(2,0),mScale.x)>>4);
		mTransform.set(AffineTransform.M01,Math.mul(mRotate.at(0,1),mScale.y)>>4);
		mTransform.set(AffineTransform.M11,Math.mul(mRotate.at(1,1),mScale.y)>>4);
		mTransform.set(AffineTransform.M21,Math.mul(mRotate.at(2,1),mScale.y)>>4);
		mTransform.set(AffineTransform.M02,Math.mul(mRotate.at(0,2),mScale.z)>>4);
		mTransform.set(AffineTransform.M12,Math.mul(mRotate.at(1,2),mScale.z)>>4);
		mTransform.set(AffineTransform.M22,Math.mul(mRotate.at(2,2),mScale.z)>>4);

		mIdentityTransform=false;
	}	

	public void setScale(int x,int y,int z){
		mScale.set(x,y,z);

		mTransform.set(AffineTransform.M00,Math.mul(mRotate.at(0,0),mScale.x)>>4);
		mTransform.set(AffineTransform.M10,Math.mul(mRotate.at(1,0),mScale.x)>>4);
		mTransform.set(AffineTransform.M20,Math.mul(mRotate.at(2,0),mScale.x)>>4);
		mTransform.set(AffineTransform.M01,Math.mul(mRotate.at(0,1),mScale.y)>>4);
		mTransform.set(AffineTransform.M11,Math.mul(mRotate.at(1,1),mScale.y)>>4);
		mTransform.set(AffineTransform.M21,Math.mul(mRotate.at(2,1),mScale.y)>>4);
		mTransform.set(AffineTransform.M02,Math.mul(mRotate.at(0,2),mScale.z)>>4);
		mTransform.set(AffineTransform.M12,Math.mul(mRotate.at(1,2),mScale.z)>>4);
		mTransform.set(AffineTransform.M22,Math.mul(mRotate.at(2,2),mScale.z)>>4);

		mIdentityTransform=false;
	}

	public final Vector3 getScale(){
		return mScale;
	}

	public final void setScope(int scope){
		mScope=scope;
	}

	public final int getScope(){
		return mScope;
	}

	public final void setBoundingRadius(int bound){
		mBoundingRadius=bound;
	}

	public final int getBoundingRadius(){
		return mBoundingRadius;
	}

	public final Engine getEngine(){
		return mEngine;
	}

	protected boolean mDestroyed;
	protected Engine mEngine;
	protected int mType;

	protected NodeEntity mParent;

	boolean mIdentityTransform=true;
	Vector3 mTranslate=new Vector3();
	Matrix3x3 mRotate=new Matrix3x3();
	Vector3 mScale=new Vector3(Math.ONE,Math.ONE,Math.ONE);
	int mScope=-1;
	int mBoundingRadius=-Math.ONE;

	AffineTransform mTransform=new AffineTransform();
	AffineTransform mWorldTransform=new AffineTransform();
}
