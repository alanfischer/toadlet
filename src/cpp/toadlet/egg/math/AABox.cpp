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

#include <toadlet/egg/math/AABox.h>
#include <toadlet/egg/math/Math.h>

namespace toadlet{
namespace egg{
namespace math{

void AABox::rotate(const Matrix3x3 &rotation){
	int i;
	Vector3 buffer[8];
	Vector3 temp;

	getVertexes(buffer);

	for(i=0;i<8;++i){
		Math::mul(temp,rotation,buffer[i]);
		if(i==0){
			mins=temp;
			maxs=temp;
		}
		else{
			if(mins.x>temp.x)
				mins.x=temp.x;
			if(mins.y>temp.y)
				mins.y=temp.y;
			if(mins.z>temp.z)
				mins.z=temp.z;
			if(maxs.x<temp.x)
				maxs.x=temp.x;
			if(maxs.y<temp.y)
				maxs.y=temp.y;
			if(maxs.z<temp.z)
				maxs.z=temp.z;
		}
	}
}

void AABox::rotate(const Matrix3x3 &rotation,Vector3 buffer[8]){
	int i;
	Vector3 temp;

	getVertexes(buffer);

	for(i=0;i<8;++i){
		Math::mul(temp,rotation,buffer[i]);
		if(i==0){
			mins=temp;
			maxs=temp;
		}
		else{
			if(mins.x>temp.x)
				mins.x=temp.x;
			if(mins.y>temp.y)
				mins.y=temp.y;
			if(mins.z>temp.z)
				mins.z=temp.z;
			if(maxs.x<temp.x)
				maxs.x=temp.x;
			if(maxs.y<temp.y)
				maxs.y=temp.y;
			if(maxs.z<temp.z)
				maxs.z=temp.z;
		}
	}
}

}
}
}
