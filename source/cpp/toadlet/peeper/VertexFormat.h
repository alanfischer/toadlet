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

#ifndef TOADLET_PEEPER_VERTEXFORMAT_H
#define TOADLET_PEEPER_VERTEXFORMAT_H

#include <toadlet/egg/String.h>
#include <toadlet/egg/Resource.h>
#include <toadlet/peeper/Types.h>

namespace toadlet{
namespace peeper{

class TOADLET_API VertexFormat:public Resource{
public:
	TOADLET_IPTR(VertexFormat);

	enum Semantic{
		Semantic_UNKNOWN=			-1,

		Semantic_POSITION=			0,
		Semantic_BLEND_WEIGHTS=		1,
		Semantic_BLEND_INDICES=		2,
		Semantic_NORMAL=			3,
		Semantic_COLOR=				4,
		Semantic_TEXCOORD=			5,
		Semantic_SCALE=				6,
		Semantic_ROTATE=			7,
	};

	enum Format{
		Format_UNKNOWN=			0,

		// Format types
		Format_MASK_TYPES=		0xFF,
		Format_TYPE_UINT_8=		1,
		Format_TYPE_INT_8=		2,
		Format_TYPE_INT_16=		3,
		Format_TYPE_INT_32=		4,
		Format_TYPE_FIXED_32=	5,
		Format_TYPE_FLOAT_32=	6,
		Format_TYPE_DOUBLE_64=	7,
		Format_TYPE_COLOR_RGBA=	8,

		// Format counts
		Format_SHIFT_COUNTS=	16,
		Format_MASK_COUNTS=		0xFF<<16,
		Format_COUNT_1=			1<<16,
		Format_COUNT_2=			2<<16,
		Format_COUNT_3=			3<<16,
		Format_COUNT_4=			4<<16,
	};

	// static methods aren't allowed in Interfaces, but technically enums shouldn't be either, so these need to be separated to an alongside class
	static int getFormatSize(int format){
		int size=0;
		int type=(format&Format_MASK_TYPES);
		if(type==Format_TYPE_UINT_8 || type==Format_TYPE_INT_8){
			size=1;
		}
		else if(type==Format_TYPE_INT_16){
			size=2;
		}
		else if(type==Format_TYPE_INT_32 || type==Format_TYPE_FIXED_32 || type==Format_TYPE_FLOAT_32){
			size=4;
		}
		else if(type==Format_TYPE_DOUBLE_64){
			size=8;
		}
		else if(format==Format_TYPE_COLOR_RGBA){
			size=4;
		}

		int count=(format&Format_MASK_COUNTS)>>Format_SHIFT_COUNTS;
		if(count>0){
			size*=count;
		}

		return size;
	}
	
	virtual ~VertexFormat(){}

	virtual VertexFormat *getRootVertexFormat()=0;

	virtual bool create()=0;
	virtual void destroy()=0;

	virtual bool addElement(int semantic,int index,int format)=0;
	virtual bool addElement(const String &name,int index,int format)=0;
	virtual int getNumElements() const=0;
	virtual int getElementSemantic(int i) const=0;
	virtual String getElementName(int i) const=0;
	virtual int getElementIndex(int i) const=0;
	virtual int getElementFormat(int i) const=0;
	virtual int getElementOffset(int i) const=0;
	virtual int getVertexSize() const=0;
	virtual int findElement(int semantic)=0;
	virtual int findElement(const String &name)=0;
};

}
}

#endif
