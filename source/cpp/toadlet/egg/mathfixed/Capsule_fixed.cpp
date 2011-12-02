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

#include <toadlet/egg/mathfixed/Capsule.h>
#include <toadlet/egg/mathfixed/Math.h>

namespace toadlet{
namespace egg{
namespace mathfixed{

void Capsule::setLength(fixed length){
	if(length>=(radius<<1)){
		if(direction.x==0 && direction.y==0 && direction.z==0){
			direction.z=(length-(radius<<1));
		}
		else{
			fixed s=Math::div(length-(radius<<1),Math::length(direction));
			Math::mul(direction,s);
		}
	}
	else{
		direction.x=0;
		direction.y=0;
		direction.z=0;
	}
}

fixed Capsule::getLength() const{
	return Math::length(direction)+(radius<<1);
}

void Capsule::rotate(const Matrix3x3 &rotation){
	Math::mul(direction,rotation);
}

}
}
}
