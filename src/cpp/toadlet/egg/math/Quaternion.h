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

#ifndef TOADLET_EGG_MATH_QUATERNION_H
#define TOADLET_EGG_MATH_QUATERNION_H

#include <toadlet/egg/math/BaseMath.h>
#include <toadlet/egg/math/Vector3.h>

namespace toadlet{
namespace egg{
namespace math{

class TOADLET_API TOADLET_ALIGNED Quaternion{
public:
	real x,y,z,w;

	inline Quaternion():x(0),y(0),z(0),w(1){}

	inline Quaternion(real x1,real y1,real z1,real w1):x(x1),y(y1),z(z1),w(w1){}

	inline Quaternion(real v[]):x(v[0]),y(v[1]),z(v[2]),w(v[3]){}

	inline Quaternion &set(const Quaternion &q){
		x=q.x;
		y=q.y;
		z=q.z;
		w=q.w;

		return *this;
	}

	inline Quaternion &set(real x1,real y1,real z1,real w1){
		x=x1;
		y=y1;
		z=z1;
		w=w1;

		return *this;
	}

	inline Quaternion &reset(){
		x=0;
		y=0;
		z=0;
		w=1;

		return *this;
	}

	inline bool equals(const Quaternion &q) const{
		return (q.x==x && q.y==y && q.z==z && q.w==w);
	}

	inline bool operator==(const Quaternion &q) const{
		return equals(q);
	}

	inline bool operator!=(const Quaternion &q) const{
		return !equals(q);
	}

	Quaternion operator*(const Quaternion &q2) const;
	Quaternion &operator*=(const Quaternion &q2);
	Vector3 operator*(const Vector3 &v) const;

	inline real &operator[](int i){
		return *(&x+i);
	}

	inline real operator[](int i) const{
		return *(&x+i);
	}
};

}
}
}

#endif
