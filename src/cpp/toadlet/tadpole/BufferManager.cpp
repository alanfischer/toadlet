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
#include <toadlet/peeper/BackableBuffer.h>
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
		mVertexFormats[i]->setVertexFormatDestroyedListener(NULL);
	}
	for(i=0;i<mIndexBuffers.size();++i){
		mIndexBuffers[i]->setBufferDestroyedListener(NULL);
	}
	for(i=0;i<mVertexBuffers.size();++i){
		mVertexBuffers[i]->setBufferDestroyedListener(NULL);
	}
	for(i=0;i<mPixelBuffers.size();++i){
		mPixelBuffers[i]->setBufferDestroyedListener(NULL);
	}
}

void BufferManager::destroy(){
	int i;
	for(i=0;i<mVertexFormats.size();++i){
		VertexFormat::ptr vertexFormat=mVertexFormats[i];
		vertexFormat->setVertexFormatDestroyedListener(NULL);
 		vertexFormat->destroy();
	}
	mVertexFormats.clear();

	for(i=0;i<mIndexBuffers.size();++i){
		IndexBuffer::ptr buffer=mIndexBuffers[i];
		buffer->setBufferDestroyedListener(NULL);
		buffer->destroy();
	}
	mIndexBuffers.clear();

	for(i=0;i<mVertexBuffers.size();++i){
		VertexBuffer::ptr buffer=mVertexBuffers[i];
		buffer->setBufferDestroyedListener(NULL);
		buffer->destroy();
	}
	mVertexBuffers.clear();

	for(i=0;i<mPixelBuffers.size();++i){
		PixelBuffer::ptr buffer=mPixelBuffers[i];
		buffer->setBufferDestroyedListener(NULL);
		buffer->destroy();
	}
	mPixelBuffers.clear();

	for(i=0;i<mConstantBuffers.size();++i){
		ConstantBuffer::ptr buffer=mConstantBuffers[i];
		buffer->setBufferDestroyedListener(NULL);
		buffer->destroy();
	}
	mConstantBuffers.clear();
}

VertexFormat::ptr BufferManager::createVertexFormat(){
	VertexFormat::ptr vertexFormat;
	if(mBackable || mEngine->getRenderDevice()==NULL){
		BackableVertexFormat::ptr backableVertexFormat(new BackableVertexFormat());
		backableVertexFormat->create();
		if(mEngine->getRenderDevice()!=NULL){
			VertexFormat::ptr back(mEngine->getRenderDevice()->createVertexFormat());
			backableVertexFormat->setBack(back);
		}
		vertexFormat=backableVertexFormat;
	}
	else{
		vertexFormat=VertexFormat::ptr(mEngine->getRenderDevice()->createVertexFormat());
		if(vertexFormat==NULL){
			return NULL;
		}
		vertexFormat->create();
	}

	mVertexFormats.add(vertexFormat);

	vertexFormat->setVertexFormatDestroyedListener(this);

	return vertexFormat;
}

IndexBuffer::ptr BufferManager::createIndexBuffer(int usage,int access,IndexBuffer::IndexFormat indexFormat,int size){
	IndexBuffer::ptr buffer;
	if(mBackable || mEngine->getRenderDevice()==NULL){
		BackableBuffer::ptr backableBuffer(new BackableBuffer());
		backableBuffer->create(usage,access,indexFormat,size);
		if(mEngine->getRenderDevice()!=NULL){
			IndexBuffer::ptr back(mEngine->getRenderDevice()->createIndexBuffer());
			backableBuffer->setBack(back);
		}
		buffer=backableBuffer;
	}
	else{
		buffer=IndexBuffer::ptr(mEngine->getRenderDevice()->createIndexBuffer());
		if(buffer==NULL){
			return NULL;
		}
		buffer->create(usage,access,indexFormat,size);
	}

	mIndexBuffers.add(buffer);

	buffer->setBufferDestroyedListener(this);

	return buffer;
}

VertexBuffer::ptr BufferManager::createVertexBuffer(int usage,int access,VertexFormat::ptr vertexFormat,int size){
	VertexBuffer::ptr buffer;
	if(mBackable || mEngine->getRenderDevice()==NULL){
		BackableBuffer::ptr backableBuffer(new BackableBuffer());
		backableBuffer->create(usage,access,vertexFormat,size);
		if(mEngine->getRenderDevice()!=NULL){
			VertexBuffer::ptr back(mEngine->getRenderDevice()->createVertexBuffer());
			backableBuffer->setBack(back);
		}
		buffer=backableBuffer;
	}
	else{
		buffer=VertexBuffer::ptr(mEngine->getRenderDevice()->createVertexBuffer());
		if(buffer==NULL){
			return NULL;
		}
		buffer->create(usage,access,vertexFormat,size);
	}

	mVertexBuffers.add(buffer);

	buffer->setBufferDestroyedListener(this);

	return buffer;
}

PixelBuffer::ptr BufferManager::createPixelBuffer(int usage,int access,int pixelFormat,int width,int height,int depth){
	PixelBuffer::ptr buffer;
	if(mBackable || mEngine->getRenderDevice()==NULL){
		BackableBuffer::ptr backableBuffer(new BackableBuffer());
		backableBuffer->create(usage,access,pixelFormat,width,height,depth);
		if(mEngine->getRenderDevice()!=NULL){
			PixelBuffer::ptr back(mEngine->getRenderDevice()->createPixelBuffer());
			backableBuffer->setBack(back,mEngine->getRenderDevice());
		}
		buffer=backableBuffer;
	}
	else{
		buffer=PixelBuffer::ptr(mEngine->getRenderDevice()->createPixelBuffer());
		if(buffer==NULL){
			return NULL;
		}
		buffer->create(usage,access,pixelFormat,width,height,depth);
	}

	mPixelBuffers.add(buffer);

	buffer->setBufferDestroyedListener(this);

	return buffer;
}

ConstantBuffer::ptr BufferManager::createConstantBuffer(int usage,int access,int size){
	ConstantBuffer::ptr buffer;
	if(mBackable || mEngine->getRenderDevice()==NULL){
		BackableBuffer::ptr backableBuffer(new BackableBuffer());
		backableBuffer->create(usage,access,size);
		if(mEngine->getRenderDevice()!=NULL){
			ConstantBuffer::ptr back(mEngine->getRenderDevice()->createConstantBuffer());
			backableBuffer->setBack(back);
		}
		buffer=backableBuffer;
	}
	else{
		buffer=ConstantBuffer::ptr(mEngine->getRenderDevice()->createConstantBuffer());
		if(buffer==NULL){
			return NULL;
		}
		buffer->create(usage,access,size);
	}

	mConstantBuffers.add(buffer);

	buffer->setBufferDestroyedListener(this);

	return buffer;
}

IndexBuffer::ptr BufferManager::cloneIndexBuffer(IndexBuffer::ptr oldIndexBuffer,int usage,int access,IndexBuffer::IndexFormat indexFormat,int size){
	int oldSize=oldIndexBuffer->getSize();

	#if defined(TOADLET_DEBUG)
		IndexBuffer::IndexFormat oldIndexFormat=oldIndexBuffer->getIndexFormat();
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
			int oldElementIndex=oldVertexFormat->findElement(vertexFormat->findElement(i));
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
		int oldElementIndex=oldVertexFormat->findElement(vertexFormat->getSemantic(i));
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

void BufferManager::contextActivate(RenderDevice *renderDevice){
	Logger::debug("BufferManager::contextActivate");

	int i;
	for(i=0;i<mVertexFormats.size();++i){
		VertexFormat::ptr vertexFormat=mVertexFormats[i];
		if(vertexFormat->getRootVertexFormat()!=vertexFormat){
			VertexFormat::ptr back(renderDevice->createVertexFormat());
			shared_static_cast<BackableVertexFormat>(vertexFormat)->setBack(back);
		}
	}

	for(i=0;i<mIndexBuffers.size();++i){
		IndexBuffer::ptr buffer=mIndexBuffers[i];
		if(buffer->getRootIndexBuffer()!=buffer){
			IndexBuffer::ptr back(renderDevice->createIndexBuffer());
			shared_static_cast<BackableBuffer>(buffer)->setBack(back);
		}
	}

	for(i=0;i<mVertexBuffers.size();++i){
		VertexBuffer::ptr buffer=mVertexBuffers[i];
		if(buffer->getRootVertexBuffer()!=buffer){
			VertexBuffer::ptr back(renderDevice->createVertexBuffer());
			shared_static_cast<BackableBuffer>(buffer)->setBack(back);
		}
	}

	for(i=0;i<mPixelBuffers.size();++i){
		PixelBuffer::ptr buffer=mPixelBuffers[i];
		if(buffer->getRootPixelBuffer()!=buffer){
			PixelBuffer::ptr back(renderDevice->createPixelBuffer());
			shared_static_cast<BackableBuffer>(buffer)->setBack(back,renderDevice);
		}
	}

	for(i=0;i<mConstantBuffers.size();++i){
		ConstantBuffer::ptr buffer=mConstantBuffers[i];
		if(buffer->getRootConstantBuffer()!=buffer){
			ConstantBuffer::ptr back(renderDevice->createConstantBuffer());
			shared_static_cast<BackableBuffer>(buffer)->setBack(back);
		}
	}
}

void BufferManager::contextDeactivate(RenderDevice *renderDevice){
	Logger::debug("BufferManager::contextDeactivate");

	int i;
	for(i=0;i<mConstantBuffers.size();++i){
		ConstantBuffer::ptr buffer=mConstantBuffers[i];
		if(buffer->getRootConstantBuffer()!=buffer){
			shared_static_cast<BackableBuffer>(buffer)->setBack(ConstantBuffer::ptr());
		}
	}

	for(i=0;i<mPixelBuffers.size();++i){
		PixelBuffer::ptr buffer=mPixelBuffers[i];
		if(buffer->getRootPixelBuffer()!=buffer){
			shared_static_cast<BackableBuffer>(buffer)->setBack(PixelBuffer::ptr(),NULL);
		}
	}

	for(i=0;i<mVertexBuffers.size();++i){
		VertexBuffer::ptr buffer=mVertexBuffers[i];
		if(buffer->getRootVertexBuffer()!=buffer){
			shared_static_cast<BackableBuffer>(buffer)->setBack(VertexBuffer::ptr());
		}
	}

	for(i=0;i<mIndexBuffers.size();++i){
		IndexBuffer::ptr buffer=mIndexBuffers[i];
		if(buffer->getRootIndexBuffer()!=buffer){
			shared_static_cast<BackableBuffer>(buffer)->setBack(IndexBuffer::ptr());
		}
	}

	for(i=0;i<mVertexFormats.size();++i){
		VertexFormat::ptr vertexFormat=mVertexFormats[i];
		if(vertexFormat->getRootVertexFormat()!=vertexFormat){
			shared_static_cast<BackableVertexFormat>(vertexFormat)->setBack(NULL);
		}
	}
}

void BufferManager::preContextReset(RenderDevice *renderDevice){
	Logger::debug("BufferManager::preContextReset");

	int i;
	for(i=0;i<mConstantBuffers.size();++i){
		mConstantBuffers[i]->resetDestroy();
	}
	for(i=0;i<mPixelBuffers.size();++i){
		mPixelBuffers[i]->resetDestroy();
	}
	for(i=0;i<mVertexBuffers.size();++i){
		mVertexBuffers[i]->resetDestroy();
	}
	for(i=0;i<mIndexBuffers.size();++i){
		mIndexBuffers[i]->resetDestroy();
	}
}

void BufferManager::postContextReset(RenderDevice *renderDevice){
	Logger::debug("BufferManager::postContextReset");

	int i;
	for(i=0;i<mVertexBuffers.size();++i){
		mVertexBuffers[i]->resetCreate();
	}
	for(i=0;i<mIndexBuffers.size();++i){
		mIndexBuffers[i]->resetCreate();
	}
	for(i=0;i<mPixelBuffers.size();++i){
		mPixelBuffers[i]->resetCreate();
	}
	for(i=0;i<mConstantBuffers.size();++i){
		mConstantBuffers[i]->resetCreate();
	}
}

void BufferManager::bufferDestroyed(Buffer *buffer){
	mIndexBuffers.remove(buffer);
	mVertexBuffers.remove(buffer);
	mPixelBuffers.remove(buffer);
	mConstantBuffers.remove(buffer);
}

void BufferManager::vertexFormatDestroyed(VertexFormat *vertexFormat){
	mVertexFormats.remove(vertexFormat);
}

bool BufferManager::useTriFan(){
	return !mBackable && mEngine->getRenderCaps().triangleFan;
}

}
}

