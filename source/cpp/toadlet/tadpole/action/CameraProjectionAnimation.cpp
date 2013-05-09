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

#include <toadlet/tadpole/action/CameraProjectionAnimation.h>

namespace toadlet{
namespace tadpole{
namespace action{

CameraProjectionAnimation::CameraProjectionAnimation(Camera *target,Sequence *sequence,int trackIndex):
	mTarget(target),
//	mSequence,
//	mTrack,
	mValue(0)
{
	setSequence(sequence,trackIndex);
}

void CameraProjectionAnimation::setTarget(Camera *target){
	mTarget=target;
}

bool CameraProjectionAnimation::setSequence(Sequence *sequence,int trackIndex){
	mSequence=sequence;
	mTrack=mSequence->getTrack(trackIndex);
	int elementIndex=0;
	VertexFormat *format=mTrack->getFormat();
	for(int i=0;i<format->getNumElements() && elementIndex<3;++i){
		if(format->getElementSemantic(i)==VertexFormat::Semantic_POSITION){
			mElements[i]=i;
			elementIndex++;
		}
	}
	if(elementIndex<3){
		Error::invalidParameters(Categories::TOADLET_TADPOLE,
			"not enough Semantic_POSITION in Track");
		return false;
	}

	if(mListener!=NULL){
		mListener->animationExtentsChanged(this);
	}

	return true;
}

void CameraProjectionAnimation::setValue(scalar value){
	mValue=value;
	if(mTarget!=NULL){
		int f1=-1,f2=-1;
		int hint=0;
		scalar time=mTrack->getKeyFramesAtTime(value,f1,f2,hint);

		const VertexBufferAccessor &vba=mTrack->getAccessor();

		Vector2 lr1,lr2,lr;
		vba.get2(f1,mElements[0],lr1);
		vba.get2(f2,mElements[0],lr2);
		Math::lerp(lr,lr1,lr2,time);

		Vector2 bt1,bt2,bt;
		vba.get2(f1,mElements[1],bt1);
		vba.get2(f2,mElements[1],bt2);
		Math::lerp(bt,bt1,bt2,time);

		Vector2 nf1,nf2,nf;
		vba.get2(f1,mElements[2],nf1);
		vba.get2(f2,mElements[2],nf2);
		Math::lerp(nf,nf1,nf2,time);

		mTarget->setProjectionFrustum(lr[0],lr[1],bt[0],bt[1],nf[0],nf[1]);
	}
}

}
}
}
