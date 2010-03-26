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

#include <toadlet/tadpole/animation/AnimationController.h>
#include <toadlet/tadpole/Engine.h>

using namespace toadlet::egg;

namespace toadlet{
namespace tadpole{
namespace animation{

AnimationController::AnimationController():
	mLogicTime(0),
	mRenderTime(0),
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

AnimationController::~AnimationController(){
	if(mOwnsFinishedListener && mFinishedListener!=NULL){
		delete mFinishedListener;
	}
}

void AnimationController::setTime(int time,bool setagain){
	mLogicTime=time;
	mRenderTime=time;

	if(setagain){
		set(Math::fromMilli(mRenderTime));
	}
}

void AnimationController::setCycling(Cycling cycling){
	mCycling=cycling;
}

void AnimationController::setInterpolation(Interpolation interpolation){
	mInterpolation=interpolation;
}

void AnimationController::setTimeScale(scalar scale){
	mTimeScale=scale;
}

void AnimationController::setAnimationControllerFinishedListener(AnimationControllerFinishedListener *listener,bool owns){
	if(mOwnsFinishedListener && mFinishedListener!=NULL){
		delete mFinishedListener;
	}
	mFinishedListener=listener;
	mOwnsFinishedListener=owns;
}

void AnimationController::start(){
	mRunning=true;

	mLogicTime=0;
	mRenderTime=0;
}

void AnimationController::stop(){
	mRunning=false;

	mLogicTime=0;
	mRenderTime=0;

	set(0);
}

void AnimationController::logicUpdate(int dt){
	if(mRunning==false){
		return;
	}

	if(mTimeScale>0){
		if(mMaxTime>0 && mLogicTime>=mMaxTime){
			return;
		}

		mLogicTime+=Math::toMilli(Math::mul(Math::fromMilli(dt),mTimeScale));

		if(mMaxTime!=0 && mLogicTime>=mMaxTime){
			mLogicTime=mMaxTime;
			mRenderTime=mMaxTime;

			set(Math::fromMilli(mRenderTime));

			if(mCycling==Cycling_LOOP){
				mLogicTime=0;
				mRenderTime=0;
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
	else if(mTimeScale<0){
		mLogicTime+=Math::toMilli(Math::mul(Math::fromMilli(dt),mTimeScale));

		if(mLogicTime<0){
			mLogicTime=0;
			mRenderTime=0;

			set(Math::fromMilli(0));

			if(mCycling==Cycling_LOOP){
				mLogicTime=mMaxTime;
				mRenderTime=mMaxTime;
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

void AnimationController::renderUpdate(int dt){
	if(mRunning==false){
		return;
	}

	if(mTimeScale>0){
		if(mMaxTime>0 && mRenderTime>=mMaxTime){
			return;
		}

		mRenderTime+=Math::toMilli(Math::mul(Math::fromMilli(dt),mTimeScale));

		if(mMaxTime>0 && mRenderTime>=mMaxTime){
			mRenderTime=mMaxTime;
		}
	}
	else if(mTimeScale<0){
		if(mRenderTime<0){
			return;
		}

		mRenderTime+=Math::toMilli(Math::mul(Math::fromMilli(dt),mTimeScale));

		if(mRenderTime<0){
			mRenderTime=0;
		}
	}

	set(Math::fromMilli(mRenderTime));
}

void AnimationController::set(scalar value){
	if(mInterpolation==Interpolation_COS && mMaxValue!=0){
		value=Math::mul(mMaxValue,Math::div(Math::ONE-Math::cos(Math::mul(Math::div(value,mMaxValue),Math::PI)),Math::TWO));
	}

	int i;
	for(i=0;i<mAnimations.size();++i){
		Animation *animation=mAnimations[i];
		animation->set(value);
	}
}

void AnimationController::extentsChanged(){
	mMinValue=0;
	mMaxValue=0;

	int i;
	for(i=0;i<mAnimations.size();++i){
		Animation *animation=mAnimations[i];
		
		scalar minValue=animation->getMin();
		if(mMinValue<minValue){
			mMinValue=minValue;
		}

		scalar maxValue=animation->getMax();
		if(mMaxValue<maxValue){
			mMaxValue=maxValue;
		}
	}

	mMinTime=Math::toMilli(mMinValue);
	mMaxTime=Math::toMilli(mMaxValue);
}

void AnimationController::attach(Animation::ptr animation){
	mAnimations.add(animation);

	animation->attached(this);

	extentsChanged();
}

void AnimationController::remove(Animation::ptr animation){
	mAnimations.remove(animation);

	animation->removed(this);

	extentsChanged();
}

}
}
}
