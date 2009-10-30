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

#include <toadlet/peeper/IndexBuffer.h>
#if defined(TOADLET_DEBUG)
	#include <toadlet/egg/Error.h>
#endif

namespace toadlet{
namespace peeper{

class TOADLET_API IndexBufferAccessor{
public:
	IndexBufferAccessor();
	IndexBufferAccessor(IndexBuffer *indexBuffer,Buffer::AccessType accessType=Buffer::AccessType_READ_WRITE);

	virtual ~IndexBufferAccessor();

	void lock(IndexBuffer *indexBuffer,Buffer::AccessType accessType=Buffer::AccessType_READ_WRITE);
	void unlock();

	inline int getSize() const{return mIndexBuffer->getSize();}

	inline uint8 *getData(){return mData;}

	inline void set(int i,uint32 x){
		#if defined(TOADLET_DEBUG)
			if(i<0 || i>=getSize()){egg::Error::unknown(egg::Categories::TOADLET_PEEPER,"index out of bounds");return;}
			if(x>=(uint32)(1<<(8*mIndexFormat))){egg::Error::unknown(egg::Categories::TOADLET_PEEPER,"out of datatype range");return;}
		#endif
		if(mIndexFormat==IndexBuffer::IndexFormat_UINT_8){
			((uint8*)mData)[i]=x;
		}
		else if(mIndexFormat==IndexBuffer::IndexFormat_UINT_16){
			((uint16*)mData)[i]=x;
		}
		else{ // IndexBuffer::IndexFormat_UINT_32
			((uint32*)mData)[i]=x;
		}
	}

	inline uint32 get(int i){
		#if defined(TOADLET_DEBUG)
			if(i<0 || i>=getSize()){egg::Error::unknown(egg::Categories::TOADLET_PEEPER,"index out of bounds");return 0;}
		#endif
		if(mIndexFormat==IndexBuffer::IndexFormat_UINT_8){
			return ((uint8*)mData)[i];
		}
		else if(mIndexFormat==IndexBuffer::IndexFormat_UINT_16){
			return ((uint16*)mData)[i];
		}
		else{ // IndexBuffer::IndexFormat_UINT_32
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
