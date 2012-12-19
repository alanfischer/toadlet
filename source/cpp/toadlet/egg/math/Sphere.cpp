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

void Sphere::merge(const Sphere &sphere,real epsilon){
	Vector3 originDifference;
	Math::sub(originDifference,sphere.origin,origin);
	real lengthSquared=Math::lengthSquared(originDifference);
	real radiusDifference=sphere.radius-radius;
	real radiusSquared=Math::square(radiusDifference);

	if(radiusSquared>=lengthSquared){
		if(radiusDifference>0){
			origin.set(sphere.origin);
			radius=sphere.radius;
		}
	}
	else{
		real length=Math::sqrt(lengthSquared);
		if(length > Math::ONE+epsilon){
			real coefficient=(length+radiusDifference) / (length*2.0f);
			Math::madd(origin,originDifference,coefficient,origin);
		}

		radius=(length + radius + sphere.radius) * 0.5f;
	}
}

}
}
}
