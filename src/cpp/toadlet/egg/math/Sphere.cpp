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

#include <toadlet/egg/math/Sphere.h>
#include <toadlet/egg/math/Math.h>

namespace toadlet{
namespace egg{
namespace math{

void Sphere::merge(const Sphere &sphere){
	Vector3 o=(origin+sphere.origin)/2;
	scalar r=Math::maxVal(Math::length(origin,o)+Math::maxVal(radius,0),Math::length(sphere.origin,o)+Math::maxVal(sphere.radius,0));
	origin.set(o);
	radius=r;
}

}
}
}
