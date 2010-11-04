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

#ifndef TOADLET_PEEPER_INDEXBUFFERACCESSOR_H
#define TOADLET_PEEPER_INDEXBUFFERACCESSOR_H

#include <toadlet/egg/Extents.h>
#include <toadlet/peeper/IndexBuffer.h>

namespace toadlet{
namespace peeper{

class TOADLET_API IndexBufferAccessor{
public:
	IndexBufferAccessor();
	IndexBufferAccessor(IndexBuffer *indexBuffer,int access=Buffer::Access_READ_WRITE);

	virtual ~IndexBufferAccessor();

	void lock(IndexBuffer *indexBuffer,int access=Buffer::Access_READ_WRITE);
	void unlock();

	inline int getSize() const{return mIndexBuffer->getSize();}

	inline uint8 *getData(){return mData;}

	inline void set(int i,uint32 x){
		TOADLET_ASSERT(i>=0 && i<=getSize()); // "index out of bounds"
		if(mIndexFormat==IndexBuffer::IndexFormat_UINT8){
			TOADLET_ASSERT(x<egg::Extents::MAX_UINT8); // "out of datatype range"
			((uint8*)mData)[i]=x;
		}
		else if(mIndexFormat==IndexBuffer::IndexFormat_UINT16){
			TOADLET_ASSERT(x<egg::Extents::MAX_UINT16); // "out of datatype range"
			((uint16*)mData)[i]=x;
		}
		else{ // IndexBuffer::IndexFormat_UINT32
			TOADLET_ASSERT(x<egg::Extents::MAX_UINT32); // "out of datatype range"
			((uint32*)mData)[i]=x;
		}
	}

	inline uint32 get(int i){
		TOADLET_ASSERT(i>=0 && i<=getSize()); // "index out of bounds"
		if(mIndexFormat==IndexBuffer::IndexFormat_UINT8){
			return ((uint8*)mData)[i];
		}
		else if(mIndexFormat==IndexBuffer::IndexFormat_UINT16){
			return ((uint16*)mData)[i];
		}
		else{ // IndexBuffer::IndexFormat_UINT32
			return ((uint32*)mData)[i];
		}
	}

protected:
	IndexBuffer *mIndexBuffer;
	IndexBuffer::IndexFormat mIndexFormat;
	uint8 *mData;
};

}
}

#endif
