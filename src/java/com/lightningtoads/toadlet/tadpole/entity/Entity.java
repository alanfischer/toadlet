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

import com.lightningtoads.toadlet.tadpole.Engine;

public class Entity{
	public Entity(){}

	public Entity create(Engine engine){
		if(mCreated==true){
			return this;
		}

		mCreated=true;
		mEngine=engine;

		mEntityDestroyedListener=null;

		mParent=null;

		mIdentityTransform=true;
		mTranslate.reset();
		mRotate.reset();
		mScale.set(Math.ONE,Math.ONE,Math.ONE);
		mScope=-1;
		mBoundingRadius=0;
		mReceiveUpdates=false;

		mModifiedLogicFrame=-1;
		mModifiedVisualFrame=-1;
		mWorldModifiedLogicFrame=-1;
		mWorldModifiedVisualFrame=-1;

		mVisualTransform.reset();
		mVisualWorldBound.reset();
		mVisualWorldTransform.reset();

		return this;
	}

	public void destroy(){
		if(mCreated==false){
			return;
		}

		mCreated=false;

		if(mParent!=null){
			mParent.remove(this);
			mParent=null;
		}

		if(mEntityDestroyedListener!=null){
			mEntityDestroyedListener.entityDestroyed(this);
			setEntityDestroyedListener(null);
		}

		mReceiveUpdates=false;
		
		mEngine.freeEntity(this);
		mEngine=null;
	}
	
	public boolean destroyed(){return !mCreated;}
	public boolean isParent(){return false;}
	public boolean isRenderable(){return false;}
	public boolean isLight(){return false;}
	
	public void setEntityDestroyedListener(EntityDestroyedListener listener){
		mEntityDestroyedListener=listener;
	}

	public EntityDestroyedListener getEntityDestroyedListener(){return mEntityDestroyedListener;}

	public void removeAllEntityDestroyedListeners(){
		setEntityDestroyedListener(null);
	}

	public void parentChanged(ParentEntity newParent){mParent=newParent;}
	public ParentEntity getParent(){return mParent;}

	public void setTranslate(Vector3 translate){
		mTranslate.set(translate);

		setVisualTransformTranslate(mTranslate);

		mIdentityTransform=false;
		modified();
	}

	public void setTranslate(scalar x,scalar y,scalar z){
		mTranslate.set(x,y,z);

		setVisualTransformTranslate(mTranslate);

		mIdentityTransform=false;
		modified();
	}

	public Vector3 getTranslate(){return mTranslate;}

	public void setRotate(Matrix3x3 rotate){
		mRotate.set(rotate);

		setVisualTransformRotateScale(mRotate,mScale);

		mIdentityTransform=false;
		modified();
	}

	public void setRotate(scalar x,scalar y,scalar z,scalar angle){
		Math.setMatrix3x3FromAxisAngle(mRotate,cache_setRotate_vector.set(x,y,z),angle);

		setVisualTransformRotateScale(mRotate,mScale);

		mIdentityTransform=false;
		modified();
	}

	public Matrix3x3 getRotate(){return mRotate;}

	public void setScale(Vector3 scale){
		mScale.set(scale);

		setVisualTransformRotateScale(mRotate,mScale);

		mIdentityTransform=false;
		modified();
	}

	public void setScale(scalar x,scalar y,scalar z){
		mScale.set(x,y,z);

		setVisualTransformRotateScale(mRotate,mScale);

		mIdentityTransform=false;
		modified();
	}

	public Vector3 getScale(){return mScale;}

	public boolean isIdentityTransform(){return mIdentityTransform;}

	public void setScope(int scope){
		mScope=scope;
	}

	public int getScope(){return mScope;}

	public void setBoundingRadius(scalar boundingRadius){
		mBoundingRadius=boundingRadius;
	}

	public scalar getBoundingRadius(){return mBoundingRadius;}

	public void setReceiveUpdates(boolean receiveUpdates){
		mReceiveUpdates=receiveUpdates;
	}

	public boolean getReceiveUpdates(){return mReceiveUpdates;}

	/// Only called if the Entity registers itself with the Scene in registerUpdateEntity.
	/// Dont forget to call unregisterUpdateEntity in its destroy.
	public void logicUpdate(int dt){}
	public void visualUpdate(int dt){}

	public void modified(){
		if(mEngine!=null){
			mModifiedLogicFrame=mEngine.getScene().getLogicFrame();
			mModifiedVisualFrame=mEngine.getScene().getVisualFrame();
		}
	}

	public boolean modifiedSinceLastLogicFrame(){
		return mWorldModifiedLogicFrame+1>=mEngine.getScene().getLogicFrame();
	}

	public boolean modifiedSinceLastVisualFrame(){
		return mWorldModifiedVisualFrame+1>=mEngine.getScene().getVisualFrame();
	}

	public Engine getEngine(){return mEngine;}

	public Matrix4x4 getVisualTransform(){return mVisualTransform;}
	public Matrix4x4 getVisualWorldTransform(){return mVisualWorldTransform;}

	protected void setVisualTransformTranslate( Vector3 translate){
		Math.setMatrix4x4FromTranslate(mVisualTransform,translate);
	}
	
	public void setVisualTransformRotateScale(Matrix3x3 rotate,Vector3 scale){
		Math.setMatrix4x4FromRotateScale(mVisualTransform,rotate,scale);
	}

	public void internal_setManaged(boolean managed){mManaged=managed;}
	public boolean internal_getManaged(){return mManaged;}

	// Allocation items
	protected boolean mManaged=false;

	// Engine items
	protected boolean mCreated=false;
	protected Engine mEngine=null;

	// Entity items
	protected EntityDestroyedListener mEntityDestroyedListener;

	protected ParentEntity mParent;

	protected boolean mIdentityTransform;
	protected Vector3 mTranslate=new Vector3();
	protected Matrix3x3 mRotate=new Matrix3x3();
	protected Vector3 mScale=new Vector3();
	protected int mScope;
	protected scalar mBoundingRadius;
	protected boolean mReceiveUpdates;

	protected int mModifiedLogicFrame;
	protected int mModifiedVisualFrame;
	protected int mWorldModifiedLogicFrame;
	protected int mWorldModifiedVisualFrame;

	protected Matrix4x4 mVisualTransform=new Matrix4x4();
	protected Sphere mVisualWorldBound=new Sphere();
	protected Matrix4x4 mVisualWorldTransform=new Matrix4x4();

	protected Vector3 cache_setRotate_vector=new Vector3();
}
