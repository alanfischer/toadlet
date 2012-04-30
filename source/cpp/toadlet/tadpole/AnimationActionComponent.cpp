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

#include <toadlet/egg/Logger.h>
#include <toadlet/tadpole/AnimationActionComponent.h>

namespace toadlet{
namespace tadpole{

AnimationActionComponent::AnimationActionComponent(const String &name):BaseActionComponent(name),
	mNode(NULL),
	mTime(0),
	mMinValue(0),
	mMaxValue(0),
	mMinTime(0),
	mMaxTime(0),
	mCycling(Cycling_NONE),
	//mInterpolator,
	mTimeScale(Math::ONE),
	mRunning(false)
//	mListener(NULL)
{}

AnimationActionComponent::~AnimationActionComponent(){
}

void AnimationActionComponent::setTime(int time){
	mTime=time;

	setValue(Math::fromMilli(mTime));
}

void AnimationActionComponent::setCycling(Cycling cycling){
	mCycling=cycling;
}

void AnimationActionComponent::setInterpolator(Interpolator *interpolator){
	mInterpolator=interpolator;
}

void AnimationActionComponent::setTimeScale(scalar scale){
	mTimeScale=scale;
}

void AnimationActionComponent::start(){
	if(mNode!=NULL){
		mNode->activate();
	}

	mRunning=true;

	mTime=0;
}

void AnimationActionComponent::stop(){
	mRunning=false;

	mTime=0;

	setValue(0);
}

void AnimationActionComponent::frameUpdate(int dt,int scope){
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

			setValue(Math::fromMilli(mTime));

			if(mCycling==Cycling_LOOP){
				mTime=0;
			}
			else if(mCycling==Cycling_REFLECT){
				mTimeScale*=-1;
			}
			else{
				mRunning=false;
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

			if(mCycling==Cycling_LOOP){
				mTime=mMaxTime;
			}
			else if(mCycling==Cycling_REFLECT){
				mTimeScale*=-1;
			}
			else{
				mRunning=false;
			}
		}
		else{
			setValue(Math::fromMilli(mTime));
		}
	}
}

void AnimationActionComponent::setValue(scalar value){
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

void AnimationActionComponent::attach(Animation *animation){
	mAnimations.add(animation);

	animation->setAnimationListener(this);

	animationExtentsChanged(animation);
}

void AnimationActionComponent::remove(Animation *animation){
	animation->setAnimationListener(NULL);

	mAnimations.remove(animation);

	animationExtentsChanged(NULL);
}

void AnimationActionComponent::animationExtentsChanged(Animation *animation){
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
