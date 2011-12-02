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

#include <toadlet/tadpole/animation/SkeletonAnimation.h>
#include <toadlet/tadpole/animation/Controller.h>
#include <toadlet/tadpole/node/MeshNodeSkeleton.h>
#include <toadlet/egg/Error.h>

namespace toadlet{
namespace tadpole{
namespace animation{

SkeletonAnimation::SkeletonAnimation():
	mController(NULL),
	//mTarget,
	mSequenceIndex(0)
{}

SkeletonAnimation::~SkeletonAnimation(){
}

void SkeletonAnimation::setTarget(MeshNodeSkeleton::ptr target){
	mTarget=target;
}

void SkeletonAnimation::setSequenceIndex(int index){
	mSequenceIndex=index;

	if(mController!=NULL){
		mController->extentsChanged();
	}
}

void SkeletonAnimation::set(scalar value){
	if(mTarget!=NULL){
		mTarget->updateBones(mSequenceIndex,value);
	}
}

scalar SkeletonAnimation::getMin() const{
	return 0;
}

scalar SkeletonAnimation::getMax() const{
	if(mTarget!=NULL){
		return mTarget->getSkeleton()->sequences[mSequenceIndex]->length;
	}
	else{
		return 0;
	}
}

void SkeletonAnimation::attached(Controller *controller){
	if(mController!=NULL){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"cannot attach animation to more than one controller");
		return;
	}

	mController=controller;
}

void SkeletonAnimation::removed(Controller *controller){
	mController=NULL;
}

}
}
}
