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

#include <toadlet/tadpole/BufferManager.h>
#include <toadlet/tadpole/Engine.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{

BufferManager::BufferManager(Engine *engine):ResourceManager(engine){
	mEngine=engine;
}

IndexBuffer::ptr BufferManager::createIndexBuffer(int usageFlags,Buffer::AccessType accessType,IndexBuffer::IndexFormat indexFormat,int size){
	IndexBuffer::ptr indexBuffer(mEngine->getRenderer()->createIndexBuffer());
	indexBuffer->create(usageFlags,accessType,indexFormat,size);
	return indexBuffer;
}

VertexBuffer::ptr BufferManager::createVertexBuffer(int usageFlags,Buffer::AccessType accessType,VertexFormat::ptr vertexFormat,int size){
	VertexBuffer::ptr vertexBuffer(mEngine->getRenderer()->createVertexBuffer());
	vertexBuffer->create(usageFlags,accessType,vertexFormat,size);
	return vertexBuffer;
}

IndexBuffer::ptr BufferManager::cloneIndexBuffer(IndexBuffer::ptr oldIndexBuffer,int usageFlags,Buffer::AccessType accessType,IndexBuffer::IndexFormat indexFormat,int size){
	IndexBuffer::IndexFormat oldIndexFormat=oldIndexBuffer->getIndexFormat();
	int oldSize=oldIndexBuffer->getSize();

	#if defined(TOADLET_DEBUG)
		if(indexFormat!=oldIndexFormat){
			Error::invalidParameters(Categories::TOADLET_TADPOLE,
				"cloneWithNewParameters does not support changing size of elements");
			return NULL;
		}
	#endif

	IndexBuffer::ptr indexBuffer=createIndexBuffer(usageFlags,accessType,indexFormat,size);

	int numElements=oldSize<size?oldSize:size;
	uint8 *srcData=oldIndexBuffer->lock(Buffer::AccessType_READ_ONLY);
	uint8 *dstData=indexBuffer->lock(Buffer::AccessType_WRITE_ONLY);

	memcpy(dstData,srcData,indexFormat*numElements);

	indexBuffer->unlock();
	oldIndexBuffer->unlock();

	return indexBuffer;
}

VertexBuffer::ptr BufferManager::cloneVertexBuffer(VertexBuffer::ptr oldVertexBuffer,int usageFlags,Buffer::AccessType accessType,VertexFormat::ptr vertexFormat,int size){
	int i,j;

	VertexFormat::ptr oldVertexFormat=oldVertexBuffer->getVertexFormat();
	int oldSize=oldVertexBuffer->getSize();

	#if defined(TOADLET_DEBUG)
		for(i=0;i<vertexFormat->getNumVertexElements();++i){
			const VertexElement &dstElement=vertexFormat->getVertexElement(i);
			int dstElementSize=dstElement.getSize();
			if(oldVertexFormat->hasVertexElementOfType(dstElement.type,dstElement.index)){
				const VertexElement &srcElement=oldVertexFormat->getVertexElementOfType(dstElement.type,dstElement.index);
				int srcElementSize=srcElement.getSize();
				if(dstElementSize!=srcElementSize){
					Error::invalidParameters(Categories::TOADLET_TADPOLE,
						"cloneWithNewParameters does not support changing size of elements");
					return NULL;
				}
			}
		}
	#endif

	VertexBuffer::ptr vertexBuffer=createVertexBuffer(usageFlags,accessType,vertexFormat,size);

	uint8 *srcData=oldVertexBuffer->lock(Buffer::AccessType_READ_ONLY);
	uint8 *dstData=vertexBuffer->lock(Buffer::AccessType_WRITE_ONLY);

	int numVerts=oldSize<size?oldSize:size;
	int srcVertSize=oldVertexFormat->getVertexSize();
	int dstVertSize=vertexFormat->getVertexSize();
	for(i=0;i<vertexFormat->getNumVertexElements();++i){
		const VertexElement &dstElement=vertexFormat->getVertexElement(i);
		int elementSize=dstElement.getSize();
		if(oldVertexFormat->hasVertexElementOfType(dstElement.type,dstElement.index)){
			const VertexElement &srcElement=oldVertexFormat->getVertexElementOfType(dstElement.type,dstElement.index);
			for(j=0;j<numVerts;++j){
				memcpy(dstData+dstVertSize*j+dstElement.offset,srcData+srcVertSize*j+srcElement.offset,elementSize);
			}
		}
	}

	vertexBuffer->unlock();
	oldVertexBuffer->unlock();

	return vertexBuffer;
}

}
}

