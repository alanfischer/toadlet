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

#ifndef TOADLET_EGG_MATHFIXED_EULERANGLE_H
#define TOADLET_EGG_MATHFIXED_EULERANGLE_H

#include <toadlet/egg/mathfixed/BaseMath.h>

namespace toadlet{
namespace egg{
namespace mathfixed{

class TOADLET_API EulerAngle{
public:
	enum EulerOrder{
		EulerOrder_XYZ,
		EulerOrder_ZXY,
		EulerOrder_ZYX,
	};

	EulerOrder order;
	fixed x,y,z;

	inline EulerAngle():order(EulerOrder_XYZ),x(0),y(0),z(0){}

	inline EulerAngle(EulerOrder order1,fixed x1=0,fixed y1=0,fixed z1=0):order(order1),x(x1),y(y1),z(z1){}

	inline EulerAngle &set(const EulerAngle &a){
		order=a.order;
		x=a.x;
		y=a.y;
		z=a.z;

		return *this;
	}

	inline EulerAngle &set(EulerOrder order1,fixed x1,fixed y1,fixed z1){
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
 
