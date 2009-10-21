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

#include <toadlet/peeper/VertexBuffer.h>
#include <toadlet/peeper/Renderer.h>
#include <toadlet/egg/Error.h>
#include <string.h> // memcpy

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

VertexBuffer::VertexBuffer(UsageType usageType,AccessType accessType,VertexFormat::ptr vertexFormat,int size):Buffer(){
	mType=Type_VERTEX;
	mUsageType=usageType;
	mAccessType=accessType;
	mVertexFormat=vertexFormat;
	mSize=size;
	mBufferSize=mVertexFormat->getVertexSize()*mSize;

	mData=new uint8[mBufferSize];
}

VertexBuffer::VertexBuffer(Renderer *renderer,UsageType usageType,AccessType accessType,VertexFormat::ptr vertexFormat,int size):Buffer(){
	mType=Type_VERTEX;
	mUsageType=usageType;
	mAccessType=accessType;
	mVertexFormat=vertexFormat;
	mSize=size;
	mBufferSize=mVertexFormat->getVertexSize()*mSize;

	mBufferPeer=renderer->createBufferPeer(this);
	mOwnsBufferPeer=true;
	if(mBufferPeer==NULL || mBufferPeer->supportsRead()==false){
		mData=new uint8[mBufferSize];
	}
}

VertexBuffer *VertexBuffer::clone(){
	VertexBuffer *buffer=new VertexBuffer(mUsageType,mAccessType,mVertexFormat,mSize);

	uint8 *srcData=lock(LockType_READ_ONLY);
	uint8 *dstData=buffer->lock(LockType_WRITE_ONLY);

	memcpy(dstData,srcData,mVertexFormat->getVertexSize()*mSize);

	buffer->unlock();
	unlock();

	return buffer;
}

VertexBuffer *VertexBuffer::cloneWithNewParameters(UsageType usageType,AccessType accessType,VertexFormat::ptr vertexFormat,int size){
	#if defined(TOADLET_DEBUG)
		{
			int i;
			for(i=0;i<vertexFormat->getNumVertexElements();++i){
				const VertexElement &dstElement=vertexFormat->getVertexElement(i);
				int dstElementSize=dstElement.getSize();
				if(mVertexFormat->hasVertexElementOfType(dstElement.type,dstElement.index)){
					const VertexElement &srcElement=mVertexFormat->getVertexElementOfType(dstElement.type,dstElement.index);
					int srcElementSize=srcElement.getSize();
					if(dstElementSize!=srcElementSize){
						Error::invalidParameters(Categories::TOADLET_PEEPER,
							"cloneWithNewParameters does not support changing size of elements");
						return NULL;
					}
				}
			}
		}
	#endif

	VertexBuffer *buffer=new VertexBuffer(usageType,accessType,vertexFormat,size);

	uint8 *srcData=lock(LockType_READ_ONLY);
	uint8 *dstData=buffer->lock(LockType_WRITE_ONLY);

	int numVerts=mSize<size?mSize:size;
	int srcVertSize=mVertexFormat->getVertexSize();
	int dstVertSize=vertexFormat->getVertexSize();
	int i,j;
	for(i=0;i<vertexFormat->getNumVertexElements();++i){
		const VertexElement &dstElement=vertexFormat->getVertexElement(i);
		int elementSize=dstElement.getSize();
		if(mVertexFormat->hasVertexElementOfType(dstElement.type,dstElement.index)){
			const VertexElement &srcElement=mVertexFormat->getVertexElementOfType(dstElement.type,dstElement.index);
			for(j=0;j<numVerts;++j){
				memcpy(dstData+dstVertSize*j+dstElement.offset,srcData+srcVertSize*j+srcElement.offset,elementSize);
			}
		}
	}

	buffer->unlock();
	unlock();

	return buffer;
}

}
}
