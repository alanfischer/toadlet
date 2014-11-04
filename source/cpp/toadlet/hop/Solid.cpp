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

namespace toadlet{
namespace hop{

const scalar Solid::INFINITE_MASS=-Math::ONE;

Solid::Solid():
	mScope(0),
	mInternalScope(0),
	mCollisionScope(0),
	mCollideWithScope(0),
	mMass(0),
	mInvMass(0),
	//mPosition,
	//mVelocity,
	//mForce,
	mCoefficientOfGravity(0),
	mCoefficientOfRestitution(0),
	mCoefficientOfRestitutionOverride(false),
	mCoefficientOfStaticFriction(0),
	mCoefficientOfDynamicFriction(0),
	mCoefficientOfEffectiveDrag(0),

	//mShapes,
	mShapeTypes(0),
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
	mTouching(NULL),
	//mTouchingNormal,
	mLastDT(0),

	mDoUpdateCallback(false),
	mManager(NULL),

	mSimulator(NULL)
{
	reset();
}

void Solid::reset(){
	destroy();

	mScope=-1;
	mInternalScope=0;
	mCollisionScope=-1;
	mCollideWithScope=-1;
	mMass=Math::ONE;
	mInvMass=Math::ONE;
	mPosition.reset();
	mVelocity.reset();
	mForce.reset();
	mCoefficientOfGravity=Math::ONE;
	mCoefficientOfRestitution=Math::HALF;
	mCoefficientOfRestitutionOverride=false;
	mCoefficientOfStaticFriction=Math::HALF;
	mCoefficientOfDynamicFriction=Math::HALF;
	mCoefficientOfEffectiveDrag=0;

	mShapeTypes=0;
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
	setPositionDirect(position);

	// Attempt to gather touching information
	if(mSimulator!=NULL){
		Vector3 velocity=mVelocity;
		Vector3 force=mForce;

		mSimulator->updateSolid(this,mLastDT,Math::fromMilli(mLastDT));

		setPositionDirect(position);
		mVelocity=velocity;
		mForce=force;
		activate();
	}
}

void Solid::setVelocity(const Vector3 &velocity){
	mVelocity.set(velocity);
	activate();
}

void Solid::addForce(const Vector3 &force){
	Math::add(mForce,force);
	activate();
}

void Solid::addShape(Shape *shape){
	mShapes.push_back(shape);
	shape->mSolid=this;
	updateLocalBound();
	activate();
}

void Solid::removeShape(Shape *shape){
	mShapes.erase(stlit::remove(mShapes.begin(),mShapes.end(),shape),mShapes.end());
	shape->mSolid=NULL;
	updateLocalBound();
	activate();
}

void Solid::removeAllShapes(){
	mShapes.clear();
	updateLocalBound();
	activate();
}

void Solid::activate(){
	if(mDeactivateCount>0){
		mDeactivateCount=0;
	}
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

void Solid::setStayActive(bool active){
	mDeactivateCount=(active?-1:0);
	activate();
}

void Solid::deactivate(){
	mActive=false;
	mDeactivateCount=0;
}

void Solid::setPositionDirect(const Vector3 &position){
	mPosition.set(position);
	Math::add(mWorldBound,mLocalBound,mPosition);
}

void Solid::updateLocalBound(){
	mShapeTypes=0;
	if(mShapes.size()==0){
		mLocalBound.reset();
	}
	else{
		mShapeTypes|=mShapes[0]->getType();
		mShapes[0]->getBound(mLocalBound);
		AABox box;
		int i;
		for(i=1;i<mShapes.size();++i){
			mShapeTypes|=mShapes[i]->getType();
			mShapes[i]->getBound(box);
			mLocalBound.merge(box);
		}
	}

	// Update world here to make sure everything is current
	Math::add(mWorldBound,mLocalBound,mPosition);
}

void Solid::internal_setSimulator(Simulator *s){
	mSimulator=s;
}

void Solid::internal_addConstraint(Constraint *constraint){
	mConstraints.push_back(constraint);
}

void Solid::internal_removeConstraint(Constraint *constraint){
	mConstraints.erase(stlit::remove(mConstraints.begin(),mConstraints.end(),constraint),mConstraints.end());
}

}
}

