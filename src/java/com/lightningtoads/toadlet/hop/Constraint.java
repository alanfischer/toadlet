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

public final class Constraint{
	public Constraint(){}

	public Constraint(Solid startSolid,Solid endSolid){
		reset();

		setStartSolid(startSolid);
		setEndSolid(endSolid);
	}
	
	public Constraint(Solid startSolid,Vector3 endPoint){
		reset();

		setStartSolid(startSolid);
		setEndPoint(endPoint);
	}

	public void reset(){
		destroy();
		
		mSpringConstant=Math.ONE;
		mDampingConstant=Math.ONE;
		mDistanceThreshold=Math.ONE;

		mSimulator=null;
	}

	public void destroy(){
		if(mStartSolid!=null){
			mStartSolid.activate();
			mStartSolid.internal_removeConstraint(this);
			mStartSolid=null;
		}

		if(mEndSolid!=null){
			mEndSolid.activate();
			mEndSolid.internal_removeConstraint(this);
			mEndSolid=null;
		}
	}

	public void setStartSolid(Solid startSolid){
		if(mEndSolid!=null){
			mEndSolid.activate();
		}

		if(mStartSolid!=null){
			mStartSolid.activate();
			mStartSolid.internal_removeConstraint(this);
			mStartSolid=null;
		}

		if(startSolid!=null){
			startSolid.internal_addConstraint(this);
			startSolid.activate();
			mStartSolid=startSolid;
		}
	}

	public Solid getStartSolid(){return mStartSolid;}

	public void setEndSolid(Solid endSolid){
		if(mStartSolid!=null){
			mStartSolid.activate();
		}

		if(mEndSolid!=null){
			mEndSolid.activate();
			mEndSolid.internal_removeConstraint(this);
			mEndSolid=null;
		}

		if(endSolid!=null){
			endSolid.internal_addConstraint(this);
			endSolid.activate();
			mEndSolid=endSolid;
		}
	}

	public Solid getEndSolid(){return mEndSolid;}

	public void setEndPoint(Vector3 endPoint){
		if(mStartSolid!=null){
			mStartSolid.activate();
		}

		if(mEndSolid!=null){
			mEndSolid.activate();
			mEndSolid.internal_removeConstraint(this);
			mEndSolid=null;
		}

		mEndPoint.set(endPoint);
	}

	Vector3 getEndPoint(){return mEndPoint;}

	public void setSpringConstant(scalar springConstant){mSpringConstant=springConstant;}
	public scalar getSpringConstant(){return mSpringConstant;}

	public void setDampingConstant(scalar dampingConstant){mDampingConstant=dampingConstant;}
	public scalar getDampingConstant(){return mDampingConstant;}

	public void setDistanceThreshold(scalar threshold){mDistanceThreshold=threshold;}
	public scalar getDistanceThreshold(){return mDistanceThreshold;}

	public void activate(){}
	public boolean getActive(){return (mSimulator!=null);}

	public Simulator getSimulator(){return mSimulator;}

	void internal_setSimulator(Simulator s){mSimulator=s;}

	Solid mStartSolid=null;
	Solid mEndSolid=null;
	Vector3 mEndPoint=new Vector3();

	scalar mSpringConstant=0;
	scalar mDampingConstant=0;
	scalar mDistanceThreshold=0;
	
	Simulator mSimulator=null;
}

