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

#ifndef TOADLET_EGG_MATHFIXED_PLANE_H
#define TOADLET_EGG_MATHFIXED_PLANE_H

#include <toadlet/egg/mathfixed/Vector3.h>

namespace toadlet{
namespace egg{
namespace mathfixed{

class TOADLET_API TOADLET_ALIGNED Plane{
public:
	inline Plane():distance(0){}

	inline Plane(const Vector3 &normal1,fixed distance1):normal(normal1),distance(distance1){}

	inline Plane(fixed x1,fixed y1,fixed z1,fixed distance1):normal(x1,y1,z1),distance(distance1){}

	inline Plane(const Vector3 &point,const Vector3 &normal1){set(point,normal1);}

	inline Plane(const Vector3 &point1,const Vector3 &point2,const Vector3 &point3){set(point1,point2,point3);}

	inline Plane &set(const Plane &p){
		normal.x=p.normal.x;
		normal.y=p.normal.y;
		normal.z=p.normal.z;

		distance=p.distance;

		return *this;
	}

	inline Plane &set(const Vector3 &normal1,fixed distance1){
		normal.x=normal1.x;
		normal.y=normal1.y;
		normal.z=normal1.z;

		distance=distance1;

		return *this;
	}

	inline Plane &set(fixed x1,fixed y1,fixed z1,fixed distance1){
		normal.x=x1;
		normal.y=y1;
		normal.z=z1;

		distance=distance1;

		return *this;
	}

	Plane &set(const Vector3 &point,const Vector3 &normal1);
	
	Plane &set(const Vector3 &point1,const Vector3 &point2,const Vector3 &point3);
	
	inline Plane &reset(){
		normal.x=0;
		normal.y=0;
		normal.z=0;

		distance=0;

		return *this;
	}

	inline bool equals(const Plane &plane) const{
		return (plane.normal.equals(normal) && plane.distance==distance);
	}

	inline bool operator==(const Plane &plane) const{
		return equals(plane);
	}

	inline bool operator!=(const Plane &plane) const{
		return !equals(plane);
	}

	Vector3 normal;
	fixed distance;
};

}
}
}

#endif
