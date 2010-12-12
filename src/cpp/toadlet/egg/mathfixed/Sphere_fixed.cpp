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

#include <toadlet/egg/mathfixed/Sphere.h>
#include <toadlet/egg/mathfixed/Math.h>

namespace toadlet{
namespace egg{
namespace mathfixed{

void Sphere::merge(const Sphere &sphere,fixed epsilon){
	Vector3 originDifference;
	Math::sub(originDifference,sphere.origin,origin);
	fixed lengthSquared=Math::lengthSquared(originDifference);
	fixed radiusDifference=sphere.radius-radius;
	fixed radiusSquared=Math::square(radiusDifference);

	if(radiusSquared>=lengthSquared){
		if(radiusDifference>0){
			origin.set(sphere.origin);
			radius=sphere.radius;
		}
	}
	else{
		fixed length=Math::sqrt(lengthSquared);
		if(length > Math::ONE+epsilon){
			fixed coefficient=Math::div(length+radiusDifference , length<<1);
			Math::madd(origin,originDifference,coefficient,origin);
		}

		radius=(length + radius + sphere.radius)>>1;
	}
}

}
}
}
