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

#ifndef TOADLET_PEEPER_INDEXDATA_H
#define TOADLET_PEEPER_INDEXDATA_H

#include <toadlet/peeper/IndexBuffer.h>

namespace toadlet{
namespace peeper{

class TOADLET_API IndexData{
public:
	TOADLET_SHARED_POINTERS(IndexData,IndexData);

	enum Primitive{
		Primitive_POINTS=0,
		Primitive_LINES,
		Primitive_LINESTRIP,
		Primitive_TRIS,
		Primitive_TRISTRIP,
		Primitive_TRIFAN,
	};

	IndexData(Primitive primitive,IndexBuffer::ptr indexBuffer);
	IndexData(Primitive primitive,IndexBuffer::ptr indexBuffer,int start,int count);

	inline void setPrimitive(Primitive primitive){mPrimitive=primitive;}
	inline Primitive getPrimitive() const{return mPrimitive;}

	inline void setIndexBuffer(IndexBuffer::ptr indexBuffer){mIndexBuffer=indexBuffer;}
	inline const IndexBuffer::ptr &getIndexBuffer() const{return mIndexBuffer;}

	inline void setStart(int start){mStart=start;}
	inline int getStart() const{return mStart;}

	inline void setCount(int count){mCount=count;}
	inline int getCount() const{return mCount;}

protected:
	Primitive mPrimitive;
	IndexBuffer::ptr mIndexBuffer;
	int mStart;
	int mCount;
};

}
}

#endif
