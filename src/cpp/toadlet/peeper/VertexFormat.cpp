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

#include <toadlet/peeper/VertexFormat.h>

namespace toadlet{
namespace peeper{

VertexFormat::VertexFormat(int numVertexElements):
	//vertexElements,
	vertexSize(0),
	typeBits(0)
	//vertexElementsByType
{
	vertexElements.reserve(numVertexElements);
}

const VertexElement &VertexFormat::addVertexElement(const VertexElement &element){
	short index=vertexElements.size();

	vertexElements.add(element);
	vertexElements[index].offset=vertexSize;

	vertexSize+=element.getSize();
	typeBits|=element.type;

	if(element.type>=vertexElementsByType.size()){
		vertexElementsByType.resize(element.type+1,-1);
	}
	vertexElementsByType[element.type]=index;

	return vertexElements[index];
}

bool VertexFormat::equals(const VertexFormat &format) const{
	if(format.vertexElements.size()!=vertexElements.size()){
		return false;
	}
	int i;
	for(i=0;i<vertexElements.size();++i){
		if(format.vertexElements[i].equals(vertexElements[i])==false){
			return false;
		}
	}
	return true;
}

}
}
