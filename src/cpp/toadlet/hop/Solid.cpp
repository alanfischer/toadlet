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

#include <toadlet/hop/Solid.h>
#include <toadlet/hop/Simulator.h>

using namespace toadlet::egg;

namespace toadlet{
namespace hop{

const scalar Solid::INFINITE_MASS=-Math::ONE;

Solid::Solid():
	mCollisionBits(0),
	mCollideWithBits(0),
	mMass(0),
	mInvMass(0),
	//mPosition,
	//mVelocity,
	//mForce,
	//mLocalGravity,
	mLocalGravityOverride(false),
	mCoefficientOfRestitution(0),
	mCoefficientOfRestitutionOverride(false),
	mCoefficientOfStaticFriction(0),
	mCoefficientOfDynamicFriction(0),
	mCoefficientOfEffectiveDrag(0),

	//mShapes,
	//mLocalBound,
	//mWorldBound,

	//mConstraints,

	mCollisionListener(NULL),
	mUserData(NULL),

	mActive(true),
	mDeactivateCount(0),

	mTouched1(NULL),
	//mTouched1Normal,
	mTouched2(NULL),
	//mTouched2Normal,
	//mTouching,
	//mTouchingNormal,
	mInside(false),

	mDoUpdateCallback(false),

	mSimulator(NULL)
{
	reset();
}

Solid::~Solid(){
	destroy();
}

void Solid::reset(){
	destroy();

	mCollisionBits=1;
	mCollideWithBits=1;
	mMass=Math::ONE;
	mInvMass=Math::ONE;
	mPosition.reset();
	mVelocity.reset();
	mForce.reset();
	mLocalGravity.reset();
	mLocalGravityOverride=false;
	mCoefficientOfRestitution=Math::HALF;
	mCoefficientOfRestitutionOverride=false;
	mCoefficientOfStaticFriction=Math::HALF;
	mCoefficientOfDynamicFriction=Math::HALF;
	mCoefficientOfEffectiveDrag=0;

	mLocalBound.reset();
	mWorldBound.reset();

	mCollisionListener=NULL;
	mUserData=NULL;

	mActive=true;
	mDeactivateCount=0;	

	mTouched1=NULL;
	mTouched1Normal.reset();
	mTouched2=NULL;
	mTouched2Normal.reset();
	mTouching=NULL;
	mTouchingNormal.reset();
	mInside=false;
	
	mDoUpdateCallback=false;
	
	mSimulator=NULL;
}

void Solid::destroy(){
	while(mConstraints.size()>0){
		Constraint *constraint=mConstraints[0];
		if(constraint->mStartSolid==this){
			constraint->destroy();
		}
		else{
			constraint->setEndSolid(NULL);
			internal_removeConstraint(constraint);
		}
	}

	mShapes.clear();
}

void Solid::setMass(scalar mass){
	mMass=mass;
	if(mMass>0){
		mInvMass=Math::div(Math::ONE,mMass);
	}
	else{
		mInvMass=0;
	}
}

void Solid::setInfiniteMass(){
	mMass=INFINITE_MASS;
	mInvMass=0;
}

// This must remain in the .cpp due to INFINITE_MASS linker issues
bool Solid::hasInfiniteMass() const{return mMass==INFINITE_MASS;}

void Solid::setPosition(const Vector3 &position){
	mActive=true;
	mDeactivateCount=0;
	mPosition.set(position);
	Math::add(mWorldBound,mLocalBound,mPosition);

	// Reset touching information
	mTouched1=NULL;
	mTouched2=NULL;
	mTouching=NULL;
	mInside=false;
}

void Solid::setVelocity(const Vector3 &velocity){
	mActive=true;
	mDeactivateCount=0;
	mVelocity.set(velocity);
}

void Solid::addForce(const Vector3 &force){
	mActive=true;
	mDeactivateCount=0;
	Math::add(mForce,force);
}

void Solid::setLocalGravity(const Vector3 &gravity){
	mActive=true;
	mDeactivateCount=0;
	mLocalGravity.set(gravity);
	mLocalGravityOverride=true;
}

void Solid::setWorldGravity(){
	mActive=true;
	mDeactivateCount=0;
	mLocalGravityOverride=false;
}

void Solid::addShape(Shape::ptr shape){
	mActive=true;
	mDeactivateCount=0;
	mShapes.add(shape);
	shape->mSolid=this;
	updateLocalBound();
}

void Solid::removeShape(Shape *shape){
	mActive=true;
	mDeactivateCount=0;
	mShapes.remove(shape);
	shape->mSolid=NULL;
	updateLocalBound();
}

void Solid::removeAllShapes(){
	mActive=true;
	mDeactivateCount=0;
	mShapes.clear();
	updateLocalBound();
}

void Solid::setCollisionListener(CollisionListener *listener){
	mCollisionListener=listener;
}

void Solid::setUserData(void *data){
	mUserData=data;
}

void Solid::activate(){
	mDeactivateCount=0;
	if(mActive==false){
		mActive=true;
		int i;
		for(i=0;i<mConstraints.size();++i){
			Constraint *c=mConstraints[i];
			if(c->mStartSolid!=this && c->mStartSolid!=NULL){
				c->mStartSolid->activate();
			}
			else if(c->mEndSolid!=this && c->mEndSolid!=NULL){
				c->mEndSolid->activate();
			}
		}
	}
}

void Solid::setPositionNoActivate(const Vector3 &position){
	mPosition.set(position);
	Math::add(mWorldBound,mLocalBound,mPosition);
}

void Solid::updateLocalBound(){
	if(mShapes.size()==0){
		mLocalBound.reset();
	}
	else{
		mShapes[0]->getBound(mLocalBound);
		AABox box;
		int i;
		for(i=1;i<mShapes.size();++i){
			mShapes[i]->getBound(box);
			mLocalBound.mergeWith(box);
		}
	}

	// Update world here to make sure everything is current
	Math::add(mWorldBound,mLocalBound,mPosition);
}

void Solid::internal_setSimulator(Simulator *s){
	mSimulator=s;
	int i;
	for(i=0;i<mShapes.size();++i){
		mShapes[i]->internal_clamp();
	}
}

}
}

