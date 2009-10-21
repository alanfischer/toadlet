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

package com.lightningtoads.toadlet.hop;

#include <com/lightningtoads/toadlet/hop/Types.h>

public final class Shape{
	public enum Type{
		AABOX,
		SPHERE,
		CAPSULE,
	};

	public Shape(){
		mAABox=new AABox();
		mType=Type.AABOX;
	}

	public Shape(AABox box){
		mAABox=new AABox(box);
		mType=Type.AABOX;
	}

	public Shape(Sphere sphere){
		mSphere=new Sphere(sphere);
		mType=Type.SPHERE;
	}

	public Shape(Capsule capsule){
		mCapsule=new Capsule(capsule);
		mType=Type.CAPSULE;
	}

	public void reset(){
		if(mSolid!=null){
			mSolid.removeShape(this);
			mSolid=null;
		}
	
		if(mAABox!=null){
			mAABox.reset();
		}
		else{
			mAABox=new AABox();
		}
		mType=Type.AABOX;

		if(mSphere!=null){
			mSphere.reset();
		}		
		if(mCapsule!=null){
			mCapsule.reset();
		}
	}
	
	public void setAABox(AABox box){
		mSphere=null;
		mCapsule=null;

		mType=Type.AABOX;
		if(mAABox==null){
			mAABox=new AABox(box);
		}
		else{
			mAABox.set(box);
		}

		if(mSolid!=null){
			internal_clamp();
			mSolid.updateLocalBound();
		}
	}

	AABox getAABox(){return mAABox;}

	public void setSphere(Sphere sphere){
		mAABox=null;
		mCapsule=null;

		mType=Type.SPHERE;
		if(mSphere==null){
			mSphere=new Sphere(sphere);
		}
		else{
			mSphere.set(sphere);
		}

		if(mSolid!=null){
			mSolid.updateLocalBound();
		}
	}

	public Sphere getSphere(){return mSphere;}

	public void setCapsule(Capsule capsule){
		mAABox=null;
		mSphere=null;

		mType=Type.CAPSULE;
		if(mCapsule==null){
			mCapsule=new Capsule(capsule);
		}
		else{
			mCapsule.set(capsule);
		}

		if(mSolid!=null){
			mSolid.updateLocalBound();
		}
	}

	public Capsule getCapsule(){return mCapsule;}

	public Type getType(){return mType;}

	public void getBound(AABox box){
		switch(mType){
			case AABOX:
				box.set(mAABox);
			break;
			case SPHERE:
			{
				scalar r=mSphere.radius;
				box.mins.x=-r;
				box.mins.y=-r;
				box.mins.z=-r;
				box.maxs.x=r;
				box.maxs.y=r;
				box.maxs.z=r;
				Math.add(box,mSphere.origin);
			}
			break;
			case CAPSULE:
			{
				scalar r=mCapsule.radius;
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

	void internal_clamp(){
		if(mSolid!=null && mSolid.mSimulator!=null){
			switch(mType){
				case AABOX:
					mSolid.mSimulator.clampPosition(mAABox.mins);
					mSolid.mSimulator.clampPosition(mAABox.maxs);
				break;
				default:
				break;
			}
		}
	}

	Type mType;
	AABox mAABox;
	Sphere mSphere;
	Capsule mCapsule;
	Solid mSolid=null;
}
