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

#include <toadlet/peeper/IndexBuffer.h>
#include <toadlet/peeper/Renderer.h>
#include <toadlet/egg/Error.h>
#include <string.h> // memcpy

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

IndexBuffer::IndexBuffer(UsageType usageType,AccessType accessType,IndexFormat indexFormat,int size):Buffer(),
	mIndexFormat(IndexFormat_UINT_8),
	mSize(0)
{
	mType=Type_INDEX;
	mUsageType=usageType;
	mAccessType=accessType;
	mIndexFormat=indexFormat;
	mSize=size;
	mBufferSize=mIndexFormat*mSize;

	mData=new uint8[mBufferSize];
}

IndexBuffer::IndexBuffer(Renderer *renderer,UsageType usageType,AccessType accessType,IndexFormat indexFormat,int size):Buffer(),
	mIndexFormat(IndexFormat_UINT_8),
	mSize(0)
{
	mType=Type_INDEX;
	mUsageType=usageType;
	mAccessType=accessType;
	mIndexFormat=indexFormat;
	mSize=size;
	mBufferSize=mIndexFormat*mSize;

	mBufferPeer=renderer->createBufferPeer(this);
	mOwnsBufferPeer=true;
	if(mBufferPeer==NULL || mBufferPeer->supportsRead()==false){
		mData=new uint8[mBufferSize];
	}
}

IndexBuffer *IndexBuffer::clone(){
	IndexBuffer *buffer=new IndexBuffer(mUsageType,mAccessType,mIndexFormat,mSize);

	uint8 *srcData=lock(LockType_READ_ONLY);
	uint8 *dstData=buffer->lock(LockType_WRITE_ONLY);

	memcpy(dstData,srcData,mIndexFormat*mSize);

	buffer->unlock();
	unlock();

	return buffer;
}

IndexBuffer *IndexBuffer::cloneWithNewParameters(UsageType usageType,AccessType accessType,IndexFormat indexFormat,int size){
	#if defined(TOADLET_DEBUG)
		{
			if(indexFormat!=mIndexFormat){
				Error::invalidParameters(Categories::TOADLET_PEEPER,
					"cloneWithNewParameters does not support changing size of elements");
				return NULL;
			}
		}
	#endif

	IndexBuffer *buffer=new IndexBuffer(usageType,accessType,indexFormat,size);

	int numElements=mSize<size?mSize:size;
	uint8 *srcData=lock(LockType_READ_ONLY);
	uint8 *dstData=buffer->lock(LockType_WRITE_ONLY);

	memcpy(dstData,srcData,indexFormat*numElements);

	buffer->unlock();
	unlock();

	return buffer;
}

}
}
