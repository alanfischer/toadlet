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

#include <toadlet/hop/Constraint.h>
#include <toadlet/hop/Solid.h>

namespace toadlet{
namespace hop{

Constraint::Constraint():
	//mStartSolid,
	//mEndSolid,
	//mEndPoint,

	mSpringConstant(0),
	mDampingConstant(0),
	mDistanceThreshold(0),

	mSimulator(NULL)
{
	reset();
}

Constraint::Constraint(Solid::ptr startSolid,Solid::ptr endSolid):
	//mStartSolid,
	//mEndSolid,
	//mEndPoint,

	mSpringConstant(0),
	mDampingConstant(0),
	mDistanceThreshold(0),

	mSimulator(NULL)
{
	reset();

	setStartSolid(startSolid);
	setEndSolid(endSolid);
}

Constraint::Constraint(Solid::ptr startSolid,const Vector3 &endPoint):
	//mStartSolid,
	//mEndSolid,
	//mEndPoint,

	mSpringConstant(0),
	mDampingConstant(0),
	mDistanceThreshold(0),

	mSimulator(NULL)
{
	reset();

	setStartSolid(startSolid);
	setEndPoint(endPoint);
}

Constraint::~Constraint(){
	destroy();
}

void Constraint::reset(){
	destroy();

	mSpringConstant=Math::ONE;
	mDampingConstant=Math::ONE;
	mDistanceThreshold=Math::ONE;

	mSimulator=NULL;
}

void Constraint::destroy(){
	if(mStartSolid!=NULL){
		mStartSolid->activate();
		mStartSolid->internal_removeConstraint(this);
		mStartSolid=NULL;
	}

	if(mEndSolid!=NULL){
		mEndSolid->activate();
		mEndSolid->internal_removeConstraint(this);
		mEndSolid=NULL;
	}
}

void Constraint::setStartSolid(Solid::ptr startSolid){
	if(mEndSolid!=NULL){
		mEndSolid->activate();
	}

	if(mStartSolid!=NULL){
		mStartSolid->activate();
		mStartSolid->internal_removeConstraint(this);
		mStartSolid=NULL;
	}

	if(startSolid!=NULL){
		startSolid->internal_addConstraint(this);
		startSolid->activate();
		mStartSolid=startSolid;
	}
}

void Constraint::setEndSolid(Solid::ptr endSolid){
	if(mStartSolid!=NULL){
		mStartSolid->activate();
	}

	if(mEndSolid!=NULL){
		mEndSolid->activate();
		mEndSolid->internal_removeConstraint(this);
		mEndSolid=NULL;
	}

	if(endSolid!=NULL){
		endSolid->internal_addConstraint(this);
		endSolid->activate();
		mEndSolid=endSolid;
	}
}

void Constraint::setEndPoint(const Vector3 &endPoint){
	if(mStartSolid!=NULL){
		mStartSolid->activate();
	}

	if(mEndSolid!=NULL){
		mEndSolid->activate();
		mEndSolid->internal_removeConstraint(this);
		mEndSolid=NULL;
	}

	mEndPoint.set(endPoint);
}

void Constraint::setSpringConstant(scalar springConstant){
	mSpringConstant=springConstant;
}

void Constraint::setDampingConstant(scalar dampingConstant){
	mDampingConstant=dampingConstant;
}

void Constraint::setDistanceThreshold(scalar threshold){
	mDistanceThreshold=threshold;
}

void Constraint::internal_setSimulator(Simulator *s){
	mSimulator=s;
}

}
}

