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

		float fx=Math.fixedToFloat(mTranslate.x),fy=Math.fixedToFloat(mTranslate.y),fz=Math.fixedToFloat(mTranslate.z);
		mNode.setTranslation(fx,fy,fz);
	}

	public void setTranslate(int x,int y,int z){
		mTranslate.set(x,y,z);

		float fx=Math.fixedToFloat(mTranslate.x),fy=Math.fixedToFloat(mTranslate.y),fz=Math.fixedToFloat(mTranslate.z);
		mNode.setTranslation(fx,fy,fz);
	}

	public final Vector3 getTranslate(){
		return mTranslate;
	}

	public void setRotate(Matrix3x3 rotate){ 
		mRotate.set(rotate);

		if(mTransform==null){
			mTransform=new Transform();
			mTransformData=new float[16];
			mTransformData[15]=1.0f;
		}

		mTransformData[0]=Math.fixedToFloat(mRotate.at(0,0));
		mTransformData[4]=Math.fixedToFloat(mRotate.at(1,0));
		mTransformData[8]=Math.fixedToFloat(mRotate.at(2,0));
		mTransformData[1]=Math.fixedToFloat(mRotate.at(0,1));
		mTransformData[5]=Math.fixedToFloat(mRotate.at(1,1));
		mTransformData[9]=Math.fixedToFloat(mRotate.at(2,1));
		mTransformData[2]=Math.fixedToFloat(mRotate.at(0,2));
		mTransformData[6]=Math.fixedToFloat(mRotate.at(1,2));
		mTransformData[10]=Math.fixedToFloat(mRotate.at(2,2));

		mTransform.set(mTransformData);
		mNode.setTransform(mTransform);
	}

	public void setScale(Vector3 scale){
		mScale.set(scale);

		float fx=Math.fixedToFloat(mScale.x),fy=Math.fixedToFloat(mScale.y),fz=Math.fixedToFloat(mScale.z);
		mNode.setScale(fx,fy,fz);
	}

	public void setScale(int x,int y,int z){
		mScale.set(x,y,z);

		float fx=Math.fixedToFloat(mScale.x),fy=Math.fixedToFloat(mScale.y),fz=Math.fixedToFloat(mScale.z);
		mNode.setScale(fx,fy,fz);
	}

	public final Vector3 getScale(){
		return mScale;
	}

	public final Matrix3x3 getRotate(){
		return mRotate;
	}

	public void setScope(int scope){
		mScope=scope;
		mNode.setScope(mScope);
	}

	public int getScope(){
		return mScope;
	}

	// Unused, JSR-184 does it's own culling
	public final void setBoundingRadius(int bound){
		mBoundingRadius=bound;
	}

	public final int getBoundingRadius(){
		return mBoundingRadius;
	}

	public final Engine getEngine(){
		return mEngine;
	}

	public final void setNode(Node node){
		mNode=node;
	}

	public final Node getNode(){
		return mNode;
	}

	protected boolean mDestroyed;
	protected Engine mEngine;
	protected int mType;

	protected NodeEntity mParent;

	Vector3 mTranslate=new Vector3();
	Matrix3x3 mRotate=new Matrix3x3();
	Vector3 mScale=new Vector3(Math.ONE,Math.ONE,Math.ONE);
	int mScope=-1;
	int mBoundingRadius=-Math.ONE;

	Node mNode;
	Transform mTransform;
	float[] mTransformData;
}
