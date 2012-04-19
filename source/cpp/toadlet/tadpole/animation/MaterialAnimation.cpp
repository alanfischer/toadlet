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

#include <toadlet/tadpole/animation/MaterialAnimation.h>
#include <toadlet/tadpole/animation/Controller.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>

namespace toadlet{
namespace tadpole{
namespace animation{

MaterialAnimation::MaterialAnimation(Material *target,ColorSequence *sequence,int trackIndex):
	mTarget(target),
//	mSequence,
//	mTrack,
	mValue(0)
{
	setSequence(sequence,trackIndex);
}

MaterialAnimation::~MaterialAnimation(){
}

void MaterialAnimation::setTarget(Material *target){
	mTarget=target;
}

void MaterialAnimation::setSequence(ColorSequence *sequence,int trackIndex){
	mSequence=sequence;
	mTrack=mSequence->getTrack(trackIndex);

	if(mListener!=NULL){
		mListener->animationExtentsChanged(this);
	}
}

void MaterialAnimation::setValue(scalar value){
	if(mTarget!=NULL){
		Vector4 color;
		const ColorKeyFrame *f0=NULL,*f1=NULL;
		int hint=0;
		scalar time=mTrack->getKeyFramesAtTime(value,f0,f1,hint);
		Math::lerp(color,f0->color,f1->color,time);

		MaterialState state;
		mTarget->getRenderState()->getMaterialState(state);
		state.set(color);
		mTarget->getRenderState()->setMaterialState(state);
	}
}

}
}
}
