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

#include <toadlet/tadpole/mesh/Sequence.h>

using namespace toadlet::egg;

namespace toadlet{
namespace tadpole{
namespace mesh{

Sequence::Sequence():
	hasScale(false),
	length(0)
{
}

Sequence::~Sequence(){
	destroy();
}

void Sequence::destroy(){
}

void Sequence::compile(){
	int i,j;

	hasScale=false;
	length=0;
	for(i=0;i<tracks.size();++i){
		Track *track=tracks[i];

		track->compile();

		if(length<track->length){
			length=track->length;
		}

		for(j=0;j<track->keyFrames.size();++j){
			const Vector3 &scale=track->keyFrames[j].scale;
			hasScale|=(scale.x!=Math::ONE || scale.y!=Math::ONE || scale.z!=Math::ONE);
		}
	}
}

}
}
}
