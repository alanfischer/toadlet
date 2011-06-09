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
#include <toadlet/peeper/Types.h>
#include <toadlet/peeper/VertexFormatDestroyedListener.h>

namespace toadlet{
namespace peeper{

class TOADLET_API VertexFormat{
public:
	TOADLET_SHARED_POINTERS(VertexFormat);

	enum Semantic{
		Semantic_UNKNOWN=			-1,

		Semantic_POSITION=			0,
		Semantic_BLEND_WEIGHTS=		1,
		Semantic_BLEND_INDICES=		2,
		Semantic_NORMAL=			3,
		Semantic_COLOR=				4,
		Semantic_TEXCOORD=			5,
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

	// static methods aren't allowed in Interfaces, but technically enums shouldn't be either, so these need to be separated to an alongside class
	static int getFormatSize(int format){
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
	
	virtual ~VertexFormat(){}

	virtual VertexFormat *getRootVertexFormat()=0;

	virtual void setVertexFormatDestroyedListener(VertexFormatDestroyedListener *listener)=0;

	virtual bool create()=0;
	virtual void destroy()=0;

	virtual bool addElement(int semantic,int index,int format)=0;
	virtual bool addElement(const egg::String &name,int index,int format)=0;
	virtual int getNumElements() const=0;
	virtual int getSemantic(int i) const=0;
	virtual egg::String getName(int i) const=0;
	virtual int getIndex(int i) const=0;
	virtual int getFormat(int i) const=0;
	virtual int getOffset(int i) const=0;
	virtual int getVertexSize() const=0;
	virtual int findElement(int semantic)=0;
	virtual int findElement(const egg::String &name)=0;
};

}
}

#endif
