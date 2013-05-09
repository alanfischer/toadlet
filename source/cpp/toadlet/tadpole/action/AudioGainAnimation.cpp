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

#include <toadlet/tadpole/action/AudioGainAnimation.h>

namespace toadlet{
namespace tadpole{
namespace action{

AudioGainAnimation::AudioGainAnimation(AudioComponent *target,Sequence *sequence,int trackIndex):
	mTarget(target),
//	mSequence,
//	mTrack,
	mValue(0)
{
	setSequence(sequence,trackIndex);
}

void AudioGainAnimation::setTarget(AudioComponent *target){
	mTarget=target;
}

bool AudioGainAnimation::setSequence(Sequence *sequence,int trackIndex){
	mSequence=sequence;
	mTrack=mSequence->getTrack(trackIndex);

	if(mListener!=NULL){
		mListener->animationExtentsChanged(this);
	}

	return true;
}

void AudioGainAnimation::setValue(scalar value){
	mValue=value;
	if(mTarget!=NULL){
		int f1=-1,f2=-1;
		int hint=0;
		scalar time=mTrack->getKeyFramesAtTime(value,f1,f2,hint);

		const VertexBufferAccessor &vba=mTrack->getAccessor();

		float v1,v2,v;
		v1=vba.get(f1,0);
		v2=vba.get(f2,0);
		v=Math::lerp(v1,v2,time);

		mTarget->setGain(v);
	}
}

}
}
}
