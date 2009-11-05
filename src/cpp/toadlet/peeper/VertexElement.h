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
		Type_UNKNOWN=			-1,

		Type_POSITION=			0,
		Type_BLEND_WEIGHTS=		1,
		Type_BLEND_INDICES=		2,
		Type_NORMAL=			3,
		Type_COLOR_DIFFUSE=		4,
		Type_COLOR_SPECULAR=	5,
		Type_MASK_NON_TEX_COORD=0x3FF,

		Type_TEX_COORD=			10,
		Type_TEX_COORD_2=		11,
		Type_TEX_COORD_3=		12,
		Type_TEX_COORD_4=		13,
		Type_TEX_COORD_5=		14,
		Type_TEX_COORD_6=		15,
		Type_TEX_COORD_7=		16,
		Type_TEX_COORD_8=		17,
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
		Format_MASK_TYPES=		0x3FF,

		// Format counts
		Format_COUNT_SHIFT=		10,
		Format_BIT_COUNT_1=		1<<10,
		Format_BIT_COUNT_2=		1<<11,
		Format_BIT_COUNT_3=		1<<12,
		Format_BIT_COUNT_4=		1<<13,
		Format_MASK_COUNTS=		0xFFC00,
		// Formats
		Format_COLOR_RGBA=		1<<20,
	};

	VertexElement():
		type(Type_UNKNOWN),
		format(Format_UNKNOWN),
		offset(0)
	{}

	VertexElement(int type1,int format1):
		type(type1),
		format(format1),
		offset(0)
	{}

	void set(const VertexElement &vertexElement){
		type=vertexElement.type;
		format=vertexElement.format;
		offset=vertexElement.offset;
	}

	int getSize() const;

	bool equals(const VertexElement &element) const;
	inline bool operator==(const VertexElement &element) const{return equals(element);}

	int type;
	int format;
	int offset;
};

}
}

#endif
