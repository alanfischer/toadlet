
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

#include <toadlet/tadpole/action/MaterialStateAnimation.h>

namespace toadlet{
namespace tadpole{
namespace action{

MaterialStateAnimation::MaterialStateAnimation(RenderState *target,Sequence *sequence,int trackIndex):
	mTarget(target),
//	mSequence,
//	mTrack,
	mElement(0),
	mValue(0)
{
	setSequence(sequence,trackIndex);
}

void MaterialStateAnimation::setTarget(RenderState *target){
	mTarget=target;
}

bool MaterialStateAnimation::setSequence(Sequence *sequence,int trackIndex){
	mSequence=sequence;
	mTrack=mSequence->getTrack(trackIndex);
	mElement=mTrack->getFormat()->findElement(VertexFormat::Semantic_COLOR);
	if(mElement<0){
		Error::invalidParameters(Categories::TOADLET_TADPOLE,
			"no Semantic_COLOR in Track");
		return false;
	}

	if(mListener!=NULL){
		mListener->animationExtentsChanged(this);
	}

	return true;
}

void MaterialStateAnimation::setValue(scalar value){
	mValue=value;
	if(mTarget!=NULL){
		int f1=-1,f2=-1;
		int hint=0;
		scalar time=mTrack->getKeyFramesAtTime(value,f1,f2,hint);

		const VertexBufferAccessor &vba=mTrack->getAccessor();

		Vector4 c1,c2,color;
		vba.get4(f1,mElement,c1);
		vba.get4(f2,mElement,c2);
		Math::lerp(color,c1,c2,time);

		MaterialState state;
		mTarget->getMaterialState(state);
		state.set(color,state.light);
		mTarget->setMaterialState(state);
	}
}

}
}
}
