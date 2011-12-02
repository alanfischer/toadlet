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

package org.toadlet.peeper;

#include <org/toadlet/peeper/Types.h>

public final class VertexElement{
	public enum Type{
		UNKNOWN,
		POSITION,
		BLEND_WEIGHTS,
		BLEND_INDICES,
		NORMAL,
		COLOR,
		TEX_COORD,
		BINORMAL,
		TANGENT,
		POINT_SIZE,
	}

	public class Format{
		public final static int UNKNOWN=		0;
		// Format types
		public final static int BIT_UINT_8=		1<<0;
		public final static int BIT_INT_8=		1<<1;
		public final static int BIT_INT_16=		1<<2;
		public final static int BIT_INT_32=		1<<3;
		public final static int BIT_FIXED_32=	1<<4;
		public final static int BIT_FLOAT_32=	1<<5;
		public final static int BIT_DOUBLE_64=	1<<6;
		// Format counts
		public final static int BIT_COUNT_1=	1<<10;
		public final static int BIT_COUNT_2=	1<<11;
		public final static int BIT_COUNT_3=	1<<12;
		public final static int BIT_COUNT_4=	1<<13;
		// Formats
		public final static int COLOR_RGBA=		1<<20;
	}
		
	public VertexElement(){}

	public VertexElement(Type type1,int format1){
		type=type1;
		format=format1;
	}

	public VertexElement(Type type1,int format1,short index1){
		type=type1;
		format=format1;
		index=index1;
	}

	public VertexElement(VertexElement vertexElement){
		set(vertexElement);
	}

	public void set(VertexElement vertexElement){
		type=vertexElement.type;
		format=vertexElement.format;
		index=vertexElement.index;
		offset=vertexElement.offset;
	}

	public int getSize(){
		int size=0;

		if((format&(Format.BIT_UINT_8|Format.BIT_INT_8))>0){
			size=1;
		}
		else if((format&Format.BIT_INT_16)>0){
			size=2;
		}
		else if((format&(Format.BIT_INT_32|Format.BIT_FIXED_32|Format.BIT_FLOAT_32))>0){
			size=4;
		}
		else if((format&Format.BIT_DOUBLE_64)>0){
			size=8;
		}
		else if(format==Format.COLOR_RGBA){
			size=4;
		}

		if((format&Format.BIT_COUNT_2)>0){
			size*=2;
		}
		else if((format&Format.BIT_COUNT_3)>0){
			size*=3;
		}
		else if((format&Format.BIT_COUNT_4)>0){
			size*=4;
		}

		return size;
	}

	public boolean equals(Object object){
		if(this==object){
			return true;
		}
		if((object==null) || (object.getClass()!=getClass())){
			return false;
		}
		VertexElement element=(VertexElement)object;
		return element.type==type && element.format==format && element.index==index && element.offset==offset;
	}

	public Type type=Type.UNKNOWN;
	public int format=Format.UNKNOWN;
	public short index=0;
	public int offset=0;
}
