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

#include <toadlet/tadpole/action/AnimationAction.h>

namespace toadlet{
namespace tadpole{
namespace action{

AnimationAction::AnimationAction(Animation *animation):
	//mListeners,
	mTime(0),
	mMinValue(0),
	mMaxValue(0),
	mMinTime(0),
	mMaxTime(0),
	mCycling(Cycling_NONE),
	mActualCycling(Cycling_NONE),
	mResetOnStart(true),
	mStopGently(false),
	//mInterpolator,
	mTimeScale(Math::ONE),
	mRunning(false)
{
	if(animation!=NULL){
		attach(animation);
	}
}

AnimationAction::~AnimationAction(){
}

void AnimationAction::setTime(int time){
	mTime=time;

	setValue(Math::fromMilli(mTime));
}

void AnimationAction::setCycling(Cycling cycling){
	mCycling=cycling;
}

void AnimationAction::setInterpolator(Interpolator *interpolator){
	mInterpolator=interpolator;
}

void AnimationAction::setTimeScale(scalar scale){
	mTimeScale=scale;
}

void AnimationAction::addActionListener(ActionListener *listener){
	mListeners.push_back(listener);
}

void AnimationAction::removeActionListener(ActionListener *listener){
	mListeners.erase(stlit::remove(mListeners.begin(),mListeners.end(),listener),mListeners.end());
}

void AnimationAction::start(){
	if(mResetOnStart){
		mTime=0;
	}

	mActualCycling=mCycling;

	mRunning=true;

	int i;
	for(i=0;i<mAnimations.size();++i){
		mAnimations[i]->setWeight(Math::ONE);
	}

	for(i=0;i<mListeners.size();++i){
		mListeners[i]->actionStarted(this);
	}
}

void AnimationAction::stop(){
	if(mStopGently){
		mActualCycling=Cycling_NONE;
		return;
	}

	mRunning=false;

	int i;
	for(i=0;i<mAnimations.size();++i){
		mAnimations[i]->setWeight(0);
	}

	for(i=0;i<mListeners.size();++i){
		mListeners[i]->actionStopped(this);
	}
}

void AnimationAction::update(int dt){
	int i;

	if(mRunning==false){
		return;
	}

	if(mTimeScale>0){
		mTime+=Math::toMilli(Math::mul(Math::fromMilli(dt),mTimeScale));

		if(mMaxTime!=0 && mTime>=mMaxTime){
			mTime=mMaxTime;

			setValue(Math::fromMilli(mTime));

			if(mActualCycling==Cycling_LOOP){
				mTime=0;
			}
			else if(mActualCycling==Cycling_REFLECT){
				mTimeScale*=-1;
			}
			else{
				mRunning=false;

				for(i=0;i<mListeners.size();++i){
					mListeners[i]->actionStopped(this);
				}
			}
		}
		else{
			setValue(Math::fromMilli(mTime));
		}
	}
	else if(mTimeScale<0){
		mTime+=Math::toMilli(Math::mul(Math::fromMilli(dt),mTimeScale));

		if(mTime<0){
			mTime=0;

			setValue(Math::fromMilli(0));

			if(mActualCycling==Cycling_LOOP){
				mTime=mMaxTime;
			}
			else if(mActualCycling==Cycling_REFLECT){
				mTimeScale*=-1;
			}
			else{
				mRunning=false;

				for(i=0;i<mListeners.size();++i){
					mListeners[i]->actionStopped(this);
				}
			}
		}
		else{
			setValue(Math::fromMilli(mTime));
		}
	}
}

void AnimationAction::setValue(scalar value){
	if(mInterpolator!=NULL){
		value=Math::div(value-mMinValue,mMaxValue-mMinValue);
		value=mInterpolator->interpolate(value);
		value=Math::mul(value,mMaxValue-mMinValue)+mMinValue;
	}
	int i;
	for(i=0;i<mAnimations.size();++i){
		Animation *animation=mAnimations[i];
		animation->setValue(value);
	}
}

void AnimationAction::attach(Animation *animation){
	mAnimations.push_back(animation);

	animation->setAnimationListener(this);

	animationExtentsChanged(animation);
}

void AnimationAction::remove(Animation *animation){
	animation->setAnimationListener(NULL);

	mAnimations.erase(stlit::remove(mAnimations.begin(),mAnimations.end(),animation),mAnimations.end());

	animationExtentsChanged(NULL);
}

void AnimationAction::animationExtentsChanged(Animation *animation){
	mMinValue=0;
	mMaxValue=0;

	int i;
	for(i=0;i<mAnimations.size();++i){
		Animation *animation=mAnimations[i];

		scalar minValue=animation->getMinValue();
		if(mMinValue<minValue){
			mMinValue=minValue;
		}

		scalar maxValue=animation->getMaxValue();
		if(mMaxValue<maxValue){
			mMaxValue=maxValue;
		}
	}

	mMinTime=Math::toMilli(mMinValue);
	mMaxTime=Math::toMilli(mMaxValue);
}

}
}
}
