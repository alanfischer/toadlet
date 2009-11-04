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

#ifndef TOADLET_PEEPER_VERTEXELEMENT_H
#define TOADLET_PEEPER_VERTEXELEMENT_H

#include <toadlet/peeper/Types.h>

namespace toadlet{
namespace peeper{

class TOADLET_API VertexElement{
public:
	enum Type{
		Type_UNKNOWN=0,
		Type_POSITION=1<<0,
		Type_BLEND_WEIGHTS=1<<1,
		Type_BLEND_INDICES=1<<2,
		Type_NORMAL=1<<3,
		Type_COLOR=1<<4,
		Type_TEX_COORD,
	};

	enum Format{
		Format_UNKNOWN=			0,
		// Format types
		Format_BIT_UINT_8=		1<<0,
		Format_BIT_INT_8=		1<<1,
		Format_BIT_INT_16=		1<<2,
		Format_BIT_INT_32=		1<<3,
		Format_BIT_FIXED_32=	1<<4,
		Format_BIT_FLOAT_32=	1<<5,
		Format_BIT_DOUBLE_64=	1<<6,
		// Format counts
		Format_BIT_COUNT_1=		1<<10,
		Format_BIT_COUNT_2=		1<<11,
		Format_BIT_COUNT_3=		1<<12,
		Format_BIT_COUNT_4=		1<<13,
		// Formats
		Format_COLOR_RGBA=		1<<20,
	};

	VertexElement():
		type(Type_UNKNOWN),
		format(Format_UNKNOWN),
		index(0),
		offset(0)
	{}

	VertexElement(Type type1,int format1,short index1=0):
		type(type1),
		format(format1),
		index(index1),
		offset(0)
	{}

	void set(const VertexElement &vertexElement){
		type=vertexElement.type;
		format=vertexElement.format;
		index=vertexElement.index;
		offset=vertexElement.offset;
	}

	int getSize() const;

	bool equals(const VertexElement &element) const;
	inline bool operator==(const VertexElement &element) const{return equals(element);}

	Type type;
	int format;
	short index;
	int offset;
};

}
}

#endif
