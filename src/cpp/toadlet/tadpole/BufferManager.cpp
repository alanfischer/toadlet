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
#include <toadlet/peeper/BackableIndexBuffer.h>
#include <toadlet/peeper/BackableVertexBuffer.h>
#include <string.h> //memcpy

using namespace toadlet::egg;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{

BufferManager::BufferManager(Engine *engine,bool backable):
	mEngine(NULL),
	mBackable(false)
{
	mEngine=engine;
	mBackable=backable;
}

BufferManager::~BufferManager(){
	int i;
	for(i=0;i<mVertexFormats.size();++i){
		/// @todo: add VertexFormatDestroyedListeners
		//mVertexFormats[i]->setVertexFormatDestroyedListener(NULL);
	}
	for(i=0;i<mIndexBuffers.size();++i){
		mIndexBuffers[i]->setBufferDestroyedListener(NULL);
	}
	for(i=0;i<mVertexBuffers.size();++i){
		mVertexBuffers[i]->setBufferDestroyedListener(NULL);
	}
}

void BufferManager::destroy(){
	int i;
	for(i=0;i<mVertexFormats.size();++i){
		VertexFormat::ptr vertexFormat=mVertexFormats[i];
		/// @todo: add VertexFormatDestroyedListeners
		//vertexFormat->setVertexFormatDestroyedListener(NULL);
 		vertexFormat->destroy();
	}
	mVertexFormats.clear();

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

VertexFormat::ptr BufferManager::createVertexFormat(){
	VertexFormat::ptr vertexFormat;
	if(mBackable){
		Error::unimplemented("need to write BackableVertexFormat");
		return NULL;
	}
	else if(mEngine->getRenderer()!=NULL){
		vertexFormat=VertexFormat::ptr(mEngine->getRenderer()->createVertexFormat());
	}
	else{
		Error::nullPointer("can not create a non-backable VertexFormat without a renderer");
		return NULL;
	}

	mVertexFormats.add(vertexFormat);

	return vertexFormat;
}

IndexBuffer::ptr BufferManager::createIndexBuffer(int usage,int access,IndexBuffer::IndexFormat indexFormat,int size){
	IndexBuffer::ptr indexBuffer;
	if(mBackable){
		BackableIndexBuffer::ptr backableIndexBuffer(new BackableIndexBuffer());
		backableIndexBuffer->create(usage,access,indexFormat,size);
		if(mEngine->getRenderer()!=NULL){
			IndexBuffer::ptr back(mEngine->getRenderer()->createIndexBuffer());
			back->create(usage,access,indexFormat,size);
			backableIndexBuffer->setBack(back);
		}
		indexBuffer=backableIndexBuffer;
	}
	else if(mEngine->getRenderer()!=NULL){
		indexBuffer=IndexBuffer::ptr(mEngine->getRenderer()->createIndexBuffer());
		indexBuffer->create(usage,access,indexFormat,size);
	}
	else{
		Error::nullPointer("can not create a non-backable Buffer without a renderer");
		return NULL;
	}

	mIndexBuffers.add(indexBuffer);

	indexBuffer->setBufferDestroyedListener(this);

	return indexBuffer;
}

VertexBuffer::ptr BufferManager::createVertexBuffer(int usage,int access,VertexFormat::ptr vertexFormat,int size){
	VertexBuffer::ptr vertexBuffer;
	if(mBackable){
		BackableVertexBuffer::ptr backableVertexBuffer(new BackableVertexBuffer());
		backableVertexBuffer->create(usage,access,vertexFormat,size);
		if(mEngine->getRenderer()!=NULL){
			VertexBuffer::ptr back(mEngine->getRenderer()->createVertexBuffer());
			back->create(usage,access,vertexFormat,size);
			backableVertexBuffer->setBack(back);
		}
		vertexBuffer=backableVertexBuffer;
	}
	else if(mEngine->getRenderer()!=NULL){
		vertexBuffer=VertexBuffer::ptr(mEngine->getRenderer()->createVertexBuffer());
		vertexBuffer->create(usage,access,vertexFormat,size);
	}
	else{
		Error::nullPointer("can not create a non-backable Buffer without a renderer");
		return NULL;
	}

	mVertexBuffers.add(vertexBuffer);

	vertexBuffer->setBufferDestroyedListener(this);

	return vertexBuffer;
}

IndexBuffer::ptr BufferManager::cloneIndexBuffer(IndexBuffer::ptr oldIndexBuffer,int usage,int access,IndexBuffer::IndexFormat indexFormat,int size){
	IndexBuffer::IndexFormat oldIndexFormat=oldIndexBuffer->getIndexFormat();
	int oldSize=oldIndexBuffer->getSize();

	#if defined(TOADLET_DEBUG)
		if(indexFormat!=oldIndexFormat){
			Error::invalidParameters(Categories::TOADLET_TADPOLE,
				"cloneWithNewParameters does not support changing size of elements");
			return NULL;
		}
	#endif

	IndexBuffer::ptr indexBuffer=createIndexBuffer(usage,access,indexFormat,size);

	int numElements=oldSize<size?oldSize:size;
	uint8 *srcData=oldIndexBuffer->lock(Buffer::Access_BIT_READ);
	uint8 *dstData=indexBuffer->lock(Buffer::Access_BIT_WRITE);

	memcpy(dstData,srcData,indexFormat*numElements);

	indexBuffer->unlock();
	oldIndexBuffer->unlock();

	return indexBuffer;
}

VertexBuffer::ptr BufferManager::cloneVertexBuffer(VertexBuffer::ptr oldVertexBuffer,int usage,int access,VertexFormat::ptr vertexFormat,int size){
	int i,j;

	VertexFormat::ptr oldVertexFormat=oldVertexBuffer->getVertexFormat();
	int oldSize=oldVertexBuffer->getSize();

	#if defined(TOADLET_DEBUG)
		for(i=0;i<vertexFormat->getNumElements();++i){
			int oldElementIndex=oldVertexFormat->findSemantic(vertexFormat->getSemantic(i));
			if(oldElementIndex>=0){
				int dstElementSize=VertexFormat::getFormatSize(vertexFormat->getFormat(i));
				int srcElementSize=VertexFormat::getFormatSize(oldVertexFormat->getFormat(oldElementIndex));
				if(dstElementSize!=srcElementSize){
					Error::invalidParameters(Categories::TOADLET_TADPOLE,
						"cloneVertexBuffer does not support changing size of elements");
					return NULL;
				}
			}
		}
	#endif

	VertexBuffer::ptr vertexBuffer=createVertexBuffer(usage,access,vertexFormat,size);

	uint8 *srcData=oldVertexBuffer->lock(Buffer::Access_BIT_READ);
	uint8 *dstData=vertexBuffer->lock(Buffer::Access_BIT_WRITE);

	int numVerts=oldSize<size?oldSize:size;
	int srcVertSize=oldVertexFormat->getVertexSize();
	int dstVertSize=vertexFormat->getVertexSize();
	for(i=0;i<vertexFormat->getNumElements();++i){
		int oldElementIndex=oldVertexFormat->findSemantic(vertexFormat->getSemantic(i));
		if(oldElementIndex>0){
			int elementSize=VertexFormat::getFormatSize(vertexFormat->getFormat(i));
			int srcOffset=oldVertexFormat->getOffset(oldElementIndex);
			int dstOffset=vertexFormat->getOffset(i);
			for(j=0;j<numVerts;++j){
				memcpy(dstData+dstVertSize*j+dstOffset,srcData+srcVertSize*j+srcOffset,elementSize);
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
			shared_static_cast<BackableIndexBuffer>(indexBuffer)->setBack(back);
		}
	}
	for(i=0;i<mVertexBuffers.size();++i){
		VertexBuffer::ptr vertexBuffer=mVertexBuffers[i];
		if(vertexBuffer->getRootVertexBuffer()!=vertexBuffer){
			VertexBuffer::ptr back(renderer->createVertexBuffer());
			shared_static_cast<BackableVertexBuffer>(vertexBuffer)->setBack(back);
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
	}
	for(i=0;i<mVertexBuffers.size();++i){
		VertexBuffer::ptr vertexBuffer=mVertexBuffers[i];
		if(vertexBuffer->getRootVertexBuffer()!=vertexBuffer){
			shared_static_cast<BackableVertexBuffer>(vertexBuffer)->setBack(NULL);
		}
	}
}

void BufferManager::preContextReset(Renderer *renderer){
	int i;
	for(i=0;i<mIndexBuffers.size();++i){
		IndexBuffer::ptr indexBuffer=mIndexBuffers[i];
		indexBuffer->resetDestroy();
	}
	for(i=0;i<mVertexBuffers.size();++i){
		VertexBuffer::ptr vertexBuffer=mVertexBuffers[i];
		vertexBuffer->resetDestroy();
	}
}

void BufferManager::postContextReset(Renderer *renderer){
	int i;
	for(i=0;i<mIndexBuffers.size();++i){
		IndexBuffer::ptr indexBuffer=mIndexBuffers[i];
		indexBuffer->resetCreate();
	}
	for(i=0;i<mVertexBuffers.size();++i){
		VertexBuffer::ptr vertexBuffer=mVertexBuffers[i];
		vertexBuffer->resetCreate();
	}
}

void BufferManager::bufferDestroyed(Buffer *buffer){
	mIndexBuffers.remove(buffer);
	mVertexBuffers.remove(buffer);
}

}
}

