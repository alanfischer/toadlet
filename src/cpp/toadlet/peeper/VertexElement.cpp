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

#include <toadlet/peeper/VertexElement.h>

namespace toadlet{
namespace peeper{

int VertexElement::getSize() const{
	int size=0;

	if((format&(Format_BIT_UINT_8|Format_BIT_INT_8))>0){
		size=1;
	}
	else if((format&Format_BIT_INT_16)>0){
		size=2;
	}
	else if((format&(Format_BIT_INT_32|Format_BIT_FIXED_32|Format_BIT_FLOAT_32))>0){
		size=4;
	}
	else if((format&Format_BIT_DOUBLE_64)>0){
		size=8;
	}
	else if(format==Format_COLOR_RGBA){
		size=4;
	}

	if((format&Format_BIT_COUNT_2)>0){
		size*=2;
	}
	else if((format&Format_BIT_COUNT_3)>0){
		size*=3;
	}
	else if((format&Format_BIT_COUNT_4)>0){
		size*=4;
	}

	return size;
}

bool VertexElement::equals(const VertexElement &element) const{
	return element.type==type && element.format==format && element.index==index && element.offset==offset;
}

}
}
