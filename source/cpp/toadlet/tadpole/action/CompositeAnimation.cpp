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

#include <toadlet/tadpole/action/CompositeAnimation.h>

namespace toadlet{
namespace tadpole{
namespace action{

CompositeAnimation::CompositeAnimation():
	//mAnimations;
	mValue(0),mMinValue(0),mMaxValue(0),
	mWeight(0)
{
}

CompositeAnimation::~CompositeAnimation(){
	int i;
	for(i=0;i<mAnimations.size();++i){
		mAnimations[i]->setAnimationListener(NULL);
	}
	mAnimations.clear();
}

void CompositeAnimation::attach(Animation *animation){
	mAnimations.push_back(animation);

	animation->setAnimationListener(this);

	animationExtentsChanged(animation);
}

void CompositeAnimation::remove(Animation *animation){
	animation->setAnimationListener(NULL);

	mAnimations.erase(std::remove(mAnimations.begin(),mAnimations.end(),animation),mAnimations.end());

	animationExtentsChanged(animation);
}

void CompositeAnimation::setValue(scalar value){
	mValue=value;

	int i;
	for(i=0;i<mAnimations.size();++i){
		mAnimations[i]->setValue(value);
	}
}

void CompositeAnimation::setWeight(scalar weight){
	mWeight=weight;

	int i;
	for(i=0;i<mAnimations.size();++i){
		mAnimations[i]->setWeight(weight);
	}
}

void CompositeAnimation::animationExtentsChanged(Animation *animation){
	mMinValue=0;
	mMaxValue=0;

	int i;
	for(i=0;i<mAnimations.size();++i){
		Animation *animation=mAnimations[i];
		if(i==0){
			mMinValue=animation->getMinValue();
			mMaxValue=animation->getMaxValue();
		}
		else{
			if(mMinValue>animation->getMinValue()){
				mMinValue=animation->getMinValue();
			}
			if(mMaxValue<animation->getMaxValue()){
				mMaxValue=animation->getMaxValue();
			}
		}
	}

	if(mListener!=NULL){
		mListener->animationExtentsChanged(this);
	}
}

}
}
}
