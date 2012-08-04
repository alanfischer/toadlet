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

#include <toadlet/tadpole/animation/MeshAnimation.h>
#include <toadlet/tadpole/SkeletonComponent.h>
#include <toadlet/egg/Error.h>

namespace toadlet{
namespace tadpole{
namespace animation{

MeshAnimation::MeshAnimation(MeshComponent *target,int sequenceIndex):
	mTarget(target),
	mSequenceIndex(sequenceIndex),
	mValue(0)
{}

MeshAnimation::MeshAnimation(MeshComponent *target,const String &sequenceName):
	mTarget(target),
	mSequenceIndex(0),
	mValue(0)
{
	Skeleton *skeleton=target->getMesh()->getSkeleton();
	int i;
	for(i=0;i<skeleton->sequences.size();++i){
		if(skeleton->sequences[i]->getName()==sequenceName){
			mSequenceIndex=i;
			break;
		}
	}
}

MeshAnimation::~MeshAnimation(){
}

void MeshAnimation::setTarget(MeshComponent *target){
	mTarget=target;

	if(mListener!=NULL){
		mListener->animationExtentsChanged(this);
	}
}


void MeshAnimation::setSequenceIndex(int index){
	mSequenceIndex=index;

	if(mListener!=NULL){
		mListener->animationExtentsChanged(this);
	}
}

void MeshAnimation::setValue(scalar value){
	if(mTarget!=NULL && mTarget->getSkeleton()){
		mTarget->getSkeleton()->updateBones(mSequenceIndex,value);
	}
}

}
}
}
