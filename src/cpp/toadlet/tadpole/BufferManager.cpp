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

#include <toadlet/egg/Error.h>
#include <toadlet/tadpole/BufferManager.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/peeper/BackableIndexBuffer.h>
#include <toadlet/peeper/BackableVertexBuffer.h>
#include <string.h> //memcpy

using namespace toadlet::egg;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{

BufferManager::BufferManager(Engine *engine){
	mEngine=engine;
}

BufferManager::~BufferManager(){
	int i;
	for(i=0;i<mIndexBuffers.size();++i){
		mIndexBuffers[i]->setBufferDestroyedListener(NULL);
	}
	for(i=0;i<mVertexBuffers.size();++i){
		mVertexBuffers[i]->setBufferDestroyedListener(NULL);
	}
}

void BufferManager::destroy(){
	int i;
	for(i=0;i<mIndexBuffers.size();++i){
		IndexBuffer::ptr indexBuffer=mIndexBuffers[i];
		indexBuffer->setBufferDestroyedListener(NULL);
		indexBuffer->destroy();
	}
	mIndexBuffers.clear();

	for(i=0;i<mVertexBuffers.size();++i){
		VertexBuffer::ptr vertexBuffer=mVertexBuffers[i];
		vertexBuffer->setBufferDestroyedListener(NULL);
		vertexBuffer->destroy();
	}
	mVertexBuffers.clear();
}

IndexBuffer::ptr BufferManager::createIndexBuffer(int usageFlags,Buffer::AccessType accessType,IndexBuffer::IndexFormat indexFormat,int size){
	BackableIndexBuffer::ptr indexBuffer(new BackableIndexBuffer());
	indexBuffer->create(usageFlags,accessType,indexFormat,size);
	if(mEngine->getRenderer()!=NULL){
		IndexBuffer::ptr back(mEngine->getRenderer()->createIndexBuffer());
		back->create(usageFlags,accessType,indexFormat,size);
		indexBuffer->setBack(back,true);
	}
	mIndexBuffers.add(indexBuffer);

	indexBuffer->setBufferDestroyedListener(this);

	return indexBuffer;
}

VertexBuffer::ptr BufferManager::createVertexBuffer(int usageFlags,Buffer::AccessType accessType,VertexFormat::ptr vertexFormat,int size){
	BackableVertexBuffer::ptr vertexBuffer(new BackableVertexBuffer());
	vertexBuffer->create(usageFlags,accessType,vertexFormat,size);
	if(mEngine->getRenderer()!=NULL){
		VertexBuffer::ptr back(mEngine->getRenderer()->createVertexBuffer());
		back->create(usageFlags,accessType,vertexFormat,size);
		vertexBuffer->setBack(back,true);
	}
	mVertexBuffers.add(vertexBuffer);

	vertexBuffer->setBufferDestroyedListener(this);

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
			int oldElementIndex=oldVertexFormat->getVertexElementIndexOfType(dstElement.type);
			if(oldElementIndex>0){
				const VertexElement &srcElement=oldVertexFormat->vertexElements[oldElementIndex];
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
		int oldElementIndex=oldVertexFormat->getVertexElementIndexOfType(dstElement.type);
		if(oldElementIndex>0){
			const VertexElement &srcElement=oldVertexFormat->vertexElements[oldElementIndex];
			for(j=0;j<numVerts;++j){
				memcpy(dstData+dstVertSize*j+dstElement.offset,srcData+srcVertSize*j+srcElement.offset,elementSize);
			}
		}
	}

	vertexBuffer->unlock();
	oldVertexBuffer->unlock();

	return vertexBuffer;
}

void BufferManager::contextActivate(Renderer *renderer){
	int i;
	for(i=0;i<mIndexBuffers.size();++i){
		IndexBuffer::ptr indexBuffer=mIndexBuffers[i];
		if(indexBuffer->getRootIndexBuffer()!=indexBuffer){
			IndexBuffer::ptr back(renderer->createIndexBuffer());
			back->create(indexBuffer->getUsageFlags(),indexBuffer->getAccessType(),indexBuffer->getIndexFormat(),indexBuffer->getSize());
			shared_static_cast<BackableIndexBuffer>(indexBuffer)->setBack(back);
		}
		else{
			indexBuffer->createContext();
		}
	}
	for(i=0;i<mVertexBuffers.size();++i){
		VertexBuffer::ptr vertexBuffer=mVertexBuffers[i];
		if(vertexBuffer->getRootVertexBuffer()!=vertexBuffer){
			VertexBuffer::ptr back(renderer->createVertexBuffer());
			back->create(vertexBuffer->getUsageFlags(),vertexBuffer->getAccessType(),vertexBuffer->getVertexFormat(),vertexBuffer->getSize());
			shared_static_cast<BackableVertexBuffer>(vertexBuffer)->setBack(back);
		}
		else{
			vertexBuffer->createContext();
		}
	}
}

void BufferManager::contextDeactivate(Renderer *renderer){
	int i;
	for(i=0;i<mIndexBuffers.size();++i){
		IndexBuffer::ptr indexBuffer=mIndexBuffers[i];
		if(indexBuffer->getRootIndexBuffer()!=indexBuffer){
			shared_static_cast<BackableIndexBuffer>(indexBuffer)->setBack(NULL);
		}
		else{
			indexBuffer->destroyContext(true);
		}
	}
	for(i=0;i<mVertexBuffers.size();++i){
		VertexBuffer::ptr vertexBuffer=mVertexBuffers[i];
		if(vertexBuffer->getRootVertexBuffer()!=vertexBuffer){
			shared_static_cast<BackableVertexBuffer>(vertexBuffer)->setBack(NULL);
		}
		else{
			vertexBuffer->destroyContext(true);
		}
	}
}

void BufferManager::preContextReset(Renderer *renderer){
	int i;
	for(i=0;i<mIndexBuffers.size();++i){
		IndexBuffer::ptr indexBuffer=mIndexBuffers[i];
		if(indexBuffer->contextNeedsReset()){
			indexBuffer->destroyContext(true);
		}
	}
	for(i=0;i<mVertexBuffers.size();++i){
		VertexBuffer::ptr vertexBuffer=mVertexBuffers[i];
		if(vertexBuffer->contextNeedsReset()){
			vertexBuffer->destroyContext(true);
		}
	}
}

void BufferManager::postContextReset(Renderer *renderer){
	int i;
	for(i=0;i<mIndexBuffers.size();++i){
		IndexBuffer::ptr indexBuffer=mIndexBuffers[i];
		if(indexBuffer->contextNeedsReset()){
			indexBuffer->createContext();
		}
	}
	for(i=0;i<mVertexBuffers.size();++i){
		VertexBuffer::ptr vertexBuffer=mVertexBuffers[i];
		if(vertexBuffer->contextNeedsReset()){
			vertexBuffer->createContext();
		}
	}
}

void BufferManager::bufferDestroyed(Buffer *buffer){
	mIndexBuffers.remove(buffer);
	mVertexBuffers.remove(buffer);
}

}
}

