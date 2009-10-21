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

package com.lightningtoads.toadlet.tadpole.millihop;

import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.egg.mathfixed.Math;

public final class Shape{
	public final static byte TYPE_AABOX=0;
	public final static byte TYPE_SPHERE=1;
	public final static byte TYPE_CAPSULE=2;

	public Shape(){
		mAABox=new AABox();
		mType=TYPE_AABOX;
	}

	public Shape(AABox box){
		mAABox=new AABox(box);
		mType=TYPE_AABOX;
	}

	public Shape(Sphere sphere){
		mSphere=new Sphere(sphere);
		mType=TYPE_SPHERE;
	}

	public Shape(Capsule capsule){
		mCapsule=new Capsule(capsule);
		mType=TYPE_CAPSULE;
	}

	public void setAABox(AABox box){
		mSphere=null;
		mCapsule=null;
		if(mAABox==null){
			mAABox=new AABox(box);
		}
		else{
			mAABox.set(box);
		}
		mType=TYPE_AABOX;
		if(mSolid!=null){
			internal_clamp();
			mSolid.updateLocalBound();
		}
	}

	public AABox getAABox(){
		return mAABox;
	}

	public void setSphere(Sphere sphere){
		mAABox=null;
		mCapsule=null;
		if(mSphere==null){
			mSphere=new Sphere(sphere);
		}
		else{
			mSphere.set(sphere);
		}
		mType=TYPE_SPHERE;
		if(mSolid!=null){
			mSolid.updateLocalBound();
		}
	}

	public Sphere getSphere(){
		return mSphere;
	}

	public void setCapsule(Capsule capsule){
		mAABox=null;
		mSphere=null;
		if(mCapsule==null){
			mCapsule=new Capsule(capsule);
		}
		else{
			mCapsule.set(capsule);
		}
		mType=TYPE_CAPSULE;
		if(mSolid!=null){
			mSolid.updateLocalBound();
		}
	}

	public Capsule getCapsule(){
		return mCapsule;
	}

	public int getType(){
		return mType;
	}

	public void getBound(AABox box){
		switch(mType){
			case(TYPE_AABOX):
				box.set(mAABox);
			break;
			case(TYPE_SPHERE):
			{
				int r=mSphere.radius;
				box.mins.x=-r;
				box.mins.y=-r;
				box.mins.z=-r;
				box.maxs.x=r;
				box.maxs.y=r;
				box.maxs.z=r;
				Math.add(box,mSphere.origin);
			}
			break;
			case(TYPE_CAPSULE):
			{
				int r=mCapsule.radius;
				Vector3 d=mCapsule.direction;

				if(d.x<0){
					box.mins.x=d.x-r;	
					box.maxs.x=r;
				}
				else{
					box.mins.x=-r;	
					box.maxs.x=d.x+r;
				}
				if(d.y<0){
					box.mins.y=d.y-r;	
					box.maxs.y=r;
				}
				else{
					box.mins.y=-r;	
					box.maxs.y=d.y+r;
				}
				if(d.z<0){
					box.mins.z=d.z-r;	
					box.maxs.z=r;
				}
				else{
					box.mins.z=-r;	
					box.maxs.z=d.z+r;
				}

				Math.add(box,mCapsule.origin);
			}
			break;
		}
	}

	public void internal_clamp(){
		if(mSolid!=null && mSolid.mSceneManager!=null){
			switch(mType){
				case(TYPE_AABOX):
					mSolid.mSceneManager.clampPosition(mAABox.mins);
					mSolid.mSceneManager.clampPosition(mAABox.maxs);
				break;
				case(TYPE_SPHERE):
					// Do Nothing
				break;
				case(TYPE_CAPSULE):
					// Do Nothing
				break;
				default:
					// Do Nothing
				break;
			}
		}
	}

	AABox mAABox;
	Sphere mSphere;
	Capsule mCapsule;
	
	byte mType;
	HopEntity mSolid;
}

