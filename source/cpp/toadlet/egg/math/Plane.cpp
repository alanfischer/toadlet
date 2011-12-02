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

#include <toadlet/egg/math/Math.h>

namespace toadlet{
namespace egg{
namespace math{

Plane &Plane::set(const Vector3 &point,const Vector3 &normal1){
	normal.x=normal1.x;
	normal.y=normal1.y;
	normal.z=normal1.z;
	distance=Math::dot(normal,point);

	return *this;
}

Plane &Plane::set(const Vector3 &point1,const Vector3 &point2,const Vector3 &point3){
	Vector3 v1,v2;

	Math::sub(v1,point3,point1);
	Math::sub(v2,point2,point1);

	Math::cross(normal,v2,v1);

	distance=Math::dot(normal,point1);

	Math::normalize(*this);

	return *this;
}

}
}
}
