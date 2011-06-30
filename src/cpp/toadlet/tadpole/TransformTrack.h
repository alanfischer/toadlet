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

#ifndef TOADLET_TADPOLE_TRANSFORMTRACK_H
#define TOADLET_TADPOLE_TRANSFORMTRACK_H

#include <toadlet/egg/WeakPointer.h>
#include <toadlet/egg/Collection.h>
#include <toadlet/tadpole/TransformKeyFrame.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API TransformTrack{
public:
	TOADLET_SHARED_POINTERS(TransformTrack);

	TransformTrack();

	scalar getKeyFramesAtTime(scalar time,const TransformKeyFrame *&f1,const TransformKeyFrame *&f2,int &trackHint) const;

	void compile();

	int index;
	scalar length; // calculated by compile
	Collection<TransformKeyFrame> keyFrames;
};

}
}

#endif

