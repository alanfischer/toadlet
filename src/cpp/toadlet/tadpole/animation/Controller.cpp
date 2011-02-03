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

#include <toadlet/tadpole/animation/Controller.h>
#include <toadlet/tadpole/Engine.h>

using namespace toadlet::egg;

namespace toadlet{
namespace tadpole{
namespace animation{

Controller::Controller():
	mTime(0),
	mMinValue(0),
	mMaxValue(0),
	mMinTime(0),
	mMaxTime(0),
	mCycling(Cycling_NONE),
	mInterpolation(Interpolation_LINEAR),
	mTimeScale(Math::ONE),
	mRunning(false),
	mFinishedListener(NULL),
	mOwnsFinishedListener(false)
{}

Controller::~Controller(){
	if(mOwnsFinishedListener && mFinishedListener!=NULL){
		delete mFinishedListener;
	}
}

void Controller::setTime(int time,bool setagain){
	mTime=time;

	if(setagain){
		set(Math::fromMilli(mTime));
	}
}

void Controller::setCycling(Cycling cycling){
	mCycling=cycling;
}

void Controller::setInterpolation(Interpolation interpolation){
	mInterpolation=interpolation;
}

void Controller::setTimeScale(scalar scale){
	mTimeScale=scale;
}

/// @todo: this should go to a sharedptr, like the node listener
void Controller::setControllerFinishedListener(ControllerFinishedListener *listener,bool owns){
	if(mOwnsFinishedListener && mFinishedListener!=NULL){
		delete mFinishedListener;
	}
	mFinishedListener=listener;
	mOwnsFinishedListener=owns;
}

void Controller::start(){
	// Update our extends whenever we start an animation
	extentsChanged();

	mRunning=true;

	mTime=0;
}

void Controller::stop(){
	mRunning=false;

	mTime=0;

	set(0);
}

void Controller::update(int dt){
	if(mRunning==false){
		return;
	}

	if(mTimeScale>0){
		if(mMaxTime>0 && mTime>=mMaxTime){
			return;
		}

		mTime+=Math::toMilli(Math::mul(Math::fromMilli(dt),mTimeScale));

		if(mMaxTime!=0 && mTime>=mMaxTime){
			mTime=mMaxTime;

			set(Math::fromMilli(mTime));

			if(mCycling==Cycling_LOOP){
				mTime=0;
			}
			else if(mCycling==Cycling_REFLECT){
				mTimeScale*=-1;
			}
			else if(mFinishedListener!=NULL){
				mRunning=false;
				mFinishedListener->controllerFinished(this); // Must be last since it may delete this
			}
		}
		else{
			set(Math::fromMilli(mTime));
		}
	}
	else if(mTimeScale<0){
		mTime+=Math::toMilli(Math::mul(Math::fromMilli(dt),mTimeScale));

		if(mTime<0){
			mTime=0;

			set(Math::fromMilli(0));

			if(mCycling==Cycling_LOOP){
				mTime=mMaxTime;
			}
			else if(mCycling==Cycling_REFLECT){
				mTimeScale*=-1;
			}
			else if(mFinishedListener!=NULL){
				mRunning=false;
				mFinishedListener->controllerFinished(this); // Must be last since it may delete this
			}
		}
	}
}

void Controller::set(scalar value){
	if(mInterpolation==Interpolation_COS && mMaxValue!=0){
		value=Math::mul(mMaxValue,Math::div(Math::ONE-Math::cos(Math::mul(Math::div(value,mMaxValue),Math::PI)),Math::TWO));
	}

	int i;
	for(i=0;i<mAnimatables.size();++i){
		Animatable *animatable=mAnimatables[i];
		animatable->set(value);
	}
}

void Controller::extentsChanged(){
	mMinValue=0;
	mMaxValue=0;

	int i;
	for(i=0;i<mAnimatables.size();++i){
		Animatable *animatable=mAnimatables[i];
		
		scalar minValue=animatable->getMin();
		if(mMinValue<minValue){
			mMinValue=minValue;
		}

		scalar maxValue=animatable->getMax();
		if(mMaxValue<maxValue){
			mMaxValue=maxValue;
		}
	}

	mMinTime=Math::toMilli(mMinValue);
	mMaxTime=Math::toMilli(mMaxValue);
}

void Controller::attach(Animatable::ptr animatable){
	mAnimatables.add(animatable);

	extentsChanged();
}

void Controller::remove(Animatable::ptr animatable){
	mAnimatables.remove(animatable);

	extentsChanged();
}

}
}
}
