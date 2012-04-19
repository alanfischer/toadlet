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

#include <toadlet/tadpole/TransformSequence.h>

namespace toadlet{
namespace tadpole{

TransformSequence::TransformSequence():
	mLength(0),
	mScaled(false)
{
}

TransformSequence::~TransformSequence(){
	destroy();
}

void TransformSequence::destroy(){
}

void TransformSequence::compile(){
	int i,j;

	mScaled=false;
	mLength=0;
	for(i=0;i<mTracks.size();++i){
		TransformTrack *track=mTracks[i];

		track->compile();

		if(mLength<track->length){
			mLength=track->length;
		}

		for(j=0;j<track->keyFrames.size();++j){
			const Vector3 &scale=track->keyFrames[j].scale;
			mScaled|=(scale.x!=Math::ONE || scale.y!=Math::ONE || scale.z!=Math::ONE);
		}
	}
}

}
}
