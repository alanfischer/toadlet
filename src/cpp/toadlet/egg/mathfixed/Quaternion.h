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

#ifndef TOADLET_EGG_MATHFIXED_QUATERNION_H
#define TOADLET_EGG_MATHFIXED_QUATERNION_H

#include <toadlet/egg/mathfixed/BaseMath.h>
#include <toadlet/egg/mathfixed/Vector3.h>

namespace toadlet{
namespace egg{
namespace mathfixed{

class Quaternion{
public:
	fixed x,y,z,w;

	inline Quaternion():x(0),y(0),z(0),w(1){}

	inline Quaternion(fixed x1,fixed y1,fixed z1,fixed w1):x(x1),y(y1),z(z1),w(w1){}

	inline Quaternion(fixed v[]):x(v[0]),y(v[1]),z(v[2]),w(v[3]){}

	inline Quaternion &set(const Quaternion &q){
		x=q.x;
		y=q.y;
		z=q.z;
		w=q.w;

		return *this;
	}

	inline Quaternion &set(fixed x1,fixed y1,fixed z1,fixed w1){
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
		w=Math::ONE;

		return *this;
	}

	inline bool equals(const Quaternion &q) const{
		return (q.x==x && q.y==y && q.z==z && q.w==w);
	}

	inline bool operator==(const Quaternion &q) const{
		return (q.x==x && q.y==y && q.z==z && q.w==w);
	}

	inline bool operator!=(const Quaternion &q) const{
		return (q.x!=x || q.y!=y || q.z!=z || q.w!=w);
	}

	Quaternion operator*(const Quaternion &q2) const;
	Quaternion &operator*=(const Quaternion &q2);
	Vector3 operator*(const Vector3 &v) const;

	inline fixed &operator[](int i){
		return *(&x+i);
	}

	inline fixed operator[](int i) const{
		return *(&x+i);
	}
};

}
}
}

#endif
