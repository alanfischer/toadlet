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

#include <toadlet/peeper/IndexBufferAccessor.h>
#include <stdlib.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

IndexBufferAccessor::IndexBufferAccessor():
	mIndexBuffer(NULL),
	mIndexFormat(IndexBuffer::IndexFormat_UINT_8),
	mData(NULL)
{
}

IndexBufferAccessor::IndexBufferAccessor(IndexBuffer *indexBuffer,Buffer::LockType lockType):
	mIndexBuffer(NULL),
	mIndexFormat(IndexBuffer::IndexFormat_UINT_8),
	mData(NULL)
{
	lock(indexBuffer,lockType);
}

IndexBufferAccessor::~IndexBufferAccessor(){
	unlock();
}

void IndexBufferAccessor::lock(IndexBuffer *indexBuffer,Buffer::LockType lockType){
	unlock();

	mIndexBuffer=indexBuffer;
	mIndexFormat=mIndexBuffer->getIndexFormat();
	mData=mIndexBuffer->lock(lockType);
}

void IndexBufferAccessor::unlock(){
	if(mIndexBuffer!=NULL){
		mIndexBuffer->unlock();
		mIndexBuffer=NULL;
		mData=NULL;
	}
}

}
}
