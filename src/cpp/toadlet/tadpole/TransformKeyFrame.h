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

#ifndef TOADLET_TADPOLE_TRANSFORMKEYFRAME_H
#define TOADLET_TADPOLE_TRANSFORMKEYFRAME_H

#include <toadlet/tadpole/Types.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API TransformKeyFrame{
public:
	TransformKeyFrame(scalar time1=0,const Vector3 &translate1=Math::ZERO_VECTOR3,const Quaternion &rotate1=Math::IDENTITY_QUATERNION,const Vector3 &scale1=Math::ONE_VECTOR3);

	scalar time;
	Vector3 translate;
	Quaternion rotate;
	Vector3 scale;
};

}
}

#endif
