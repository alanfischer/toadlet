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

#ifndef TOADLET_EGG_MATH_EULERANGLE_H
#define TOADLET_EGG_MATH_EULERANGLE_H

#include <toadlet/egg/math/BaseMath.h>

namespace toadlet{
namespace egg{
namespace math{

class TOADLET_API EulerAngle{
public:
	enum EulerOrder{
		EulerOrder_XYZ,
		EulerOrder_ZXY,
		EulerOrder_ZYX,
	};

	EulerOrder order;
	real x,y,z;

	inline EulerAngle():order(EulerOrder_XYZ),x(0),y(0),z(0){}

	inline EulerAngle(EulerOrder order1,real x1=0,real y1=0,real z1=0):order(order1),x(x1),y(y1),z(z1){}

	inline EulerAngle &set(const EulerAngle &a){
		order=a.order;
		x=a.x;
		y=a.y;
		z=a.z;

		return *this;
	}

	inline EulerAngle &set(EulerOrder order1,real x1,real y1,real z1){
		order=order1;
		x=x1;
		y=y1;
		z=z1;

		return *this;
	}

	inline EulerAngle &reset(){
		order=EulerOrder_XYZ;
		x=0;
		y=0;
		z=0;

		return *this;
	}

	inline bool equals(const EulerAngle &angle) const{
		return (angle.order==order && angle.x==x && angle.y==y && angle.z==z);
	}

	inline bool operator==(const EulerAngle &angle) const{
		return equals(angle);
	}

	inline bool operator!=(const EulerAngle &angle) const{
		return !equals(angle);
	}

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
 
