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
	mVisualTime(0),
	mMinValue(0),
	mMaxValue(0),
	mMinTime(0),
	mMaxTime(0),
	mLoop(false),
	mInterpolation(Interpolation_LINEAR),
	mTimeScale(Math::ONE),
	mRunning(false),
	mFinishedListener(NULL)
{}

void AnimationController::setTime(int time,bool setagain){
	mLogicTime=time;
	mVisualTime=time;

	if(setagain){
		set(Math::fromMilli(mVisualTime));
	}
}

void AnimationController::setLoop(bool loop){
	mLoop=loop;
}

void AnimationController::setInterpolation(Interpolation interpolation){
	mInterpolation=interpolation;
}

void AnimationController::setTimeScale(scalar scale){
	mTimeScale=scale;
}

void AnimationController::setAnimationControllerFinishedListener(AnimationControllerFinishedListener *listener){
	mFinishedListener=listener;
}

void AnimationController::start(){
	mRunning=true;

	mLogicTime=0;
	mVisualTime=0;
}

void AnimationController::stop(){
	mRunning=false;

	mLogicTime=0;
	mVisualTime=0;

	set(0);
}

void AnimationController::logicUpdate(int dt){
	if(mRunning==false){
		return;
	}

	if(mMaxTime>0 && mLogicTime>=mMaxTime){
		return;
	}

	mLogicTime+=Math::mul(dt,mTimeScale);

	if(mMaxTime!=0 && mLogicTime>=mMaxTime){
		mLogicTime=mMaxTime;
		mVisualTime=mMaxTime;

		set(Math::fromMilli(mVisualTime));

		if(mLoop){
			mLogicTime=0;
			mVisualTime=0;
		}
		else if(mFinishedListener!=NULL){
			mRunning=false;
			mFinishedListener->controllerFinished(this); // Must be last since it may delete this
		}
	}
}

void AnimationController::visualUpdate(int dt){
	if(mRunning==false){
		return;
	}

	if(mMaxTime>0 && mVisualTime>=mMaxTime){
		return;
	}

	mVisualTime+=Math::mul(dt,mTimeScale);

	if(mMaxTime>0 && mVisualTime>=mMaxTime){
		mVisualTime=mMaxTime;
	}

	set(Math::fromMilli(mVisualTime));
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

void AnimationController::attachAnimation(Animation::ptr animation){
	mAnimations.add(animation);

	animation->attached(this);

	extentsChanged();
}

void AnimationController::removeAnimation(Animation::ptr animation){
	mAnimations.remove(animation);

	animation->removed(this);

	extentsChanged();
}

}
}
}
