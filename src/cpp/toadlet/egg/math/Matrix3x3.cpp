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

#include <toadlet/egg/math/Matrix3x3.h>
#include <toadlet/egg/math/Math.h>

namespace toadlet{
namespace egg{
namespace math{

Matrix3x3 Matrix3x3::operator*(const Matrix3x3 &m2) const{
	Matrix3x3 r;
	Math::mul(r,*this,m2);
	return r;
}

void Matrix3x3::operator*=(const Matrix3x3 &m2){
	Math::postMul(*this,m2);
}

Vector3 Matrix3x3::operator*(const Vector3 &v) const{
	Vector3 r;
	Math::mul(r,*this,v);
	return r;
}

}
}
}

