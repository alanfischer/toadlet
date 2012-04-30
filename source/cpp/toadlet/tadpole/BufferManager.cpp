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

namespace toadlet{
namespace tadpole{

BufferManager::BufferManager(Engine *engine):
	mEngine(NULL)
{
	mEngine=engine;
}

BufferManager::~BufferManager(){
	int i;
	for(i=0;i<mVertexFormats.size();++i){
		mVertexFormats[i]->setDestroyedListener(NULL);
	}
	for(i=0;i<mIndexBuffers.size();++i){
		mIndexBuffers[i]->setDestroyedListener(NULL);
	}
	for(i=0;i<mVertexBuffers.size();++i){
		mVertexBuffers[i]->setDestroyedListener(NULL);
	}
	for(i=0;i<mPixelBuffers.size();++i){
		mPixelBuffers[i]->setDestroyedListener(NULL);
	}
}

void BufferManager::destroy(){
	int i;
	for(i=0;i<mVertexFormats.size();++i){
		VertexFormat::ptr vertexFormat=mVertexFormats[i];
		vertexFormat->setDestroyedListener(NULL);
 		vertexFormat->destroy();
	}
	mVertexFormats.clear();

	for(i=0;i<mIndexBuffers.size();++i){
		IndexBuffer::ptr buffer=mIndexBuffers[i];
		buffer->setDestroyedListener(NULL);
		buffer->destroy();
	}
	mIndexBuffers.clear();

	for(i=0;i<mVertexBuffers.size();++i){
		VertexBuffer::ptr buffer=mVertexBuffers[i];
		buffer->setDestroyedListener(NULL);
		buffer->destroy();
	}
	mVertexBuffers.clear();

	for(i=0;i<mPixelBuffers.size();++i){
		PixelBuffer::ptr buffer=mPixelBuffers[i];
		buffer->setDestroyedListener(NULL);
		buffer->destroy();
	}
	mPixelBuffers.clear();

	for(i=0;i<mVariableBuffers.size();++i){
		VariableBuffer::ptr buffer=mVariableBuffers[i];
		buffer->setDestroyedListener(NULL);
		buffer->destroy();
	}
	mVariableBuffers.clear();
}

VertexFormat::ptr BufferManager::createVertexFormat(){
	RenderDevice *renderDevice=mEngine->getRenderDevice();
	VertexFormat::ptr vertexFormat;
	if(renderDevice==NULL /*mEngine->isBackable()*/){ /// @todo: Engine uses this before it has a renderer.  Need to change the Engine so it wont call any resource items without on
		BackableVertexFormat::ptr backableVertexFormat(new BackableVertexFormat());
		backableVertexFormat->create();
		if(renderDevice!=NULL){
			VertexFormat::ptr back(renderDevice->createVertexFormat());
			backableVertexFormat->setBack(back);
		}
		vertexFormat=backableVertexFormat;
	}
	else{
		vertexFormat=VertexFormat::ptr(renderDevice->createVertexFormat());
		if(vertexFormat==NULL){
			return NULL;
		}
		vertexFormat->create();
	}

	if(vertexFormat!=NULL){
		mVertexFormats.add(vertexFormat);

		vertexFormat->setDestroyedListener(this);
	}

	return vertexFormat;
}

IndexBuffer::ptr BufferManager::createIndexBuffer(int usage,int access,IndexBuffer::IndexFormat indexFormat,int size){
	RenderDevice *renderDevice=mEngine->getRenderDevice();
	IndexBuffer::ptr buffer;
	if(mEngine->isBackable()){
		BackableBuffer::ptr backableBuffer=new BackableBuffer();
		backableBuffer->create(usage,access,indexFormat,size);
		if(renderDevice!=NULL){
			IndexBuffer::ptr back=renderDevice->createIndexBuffer();
			backableBuffer->setBack(back);
		}
		buffer=backableBuffer;
	}
	else if(renderDevice!=NULL){
		buffer=renderDevice->createIndexBuffer();
		if(buffer==NULL){
			return NULL;
		}
		buffer->create(usage,access,indexFormat,size);
	}

	if(buffer!=NULL){
		mIndexBuffers.add(buffer);

		buffer->setDestroyedListener(this);
	}

	return buffer;
}

VertexBuffer::ptr BufferManager::createVertexBuffer(int usage,int access,VertexFormat::ptr vertexFormat,int size){
	RenderDevice *renderDevice=mEngine->getRenderDevice();
	VertexBuffer::ptr buffer;
	if(mEngine->isBackable()){
		BackableBuffer::ptr backableBuffer=new BackableBuffer();
		backableBuffer->create(usage,access,vertexFormat,size);
		if(renderDevice!=NULL){
			VertexBuffer::ptr back=renderDevice->createVertexBuffer();
			backableBuffer->setBack(back);
		}
		buffer=backableBuffer;
	}
	else if(renderDevice!=NULL){
		buffer=renderDevice->createVertexBuffer();
		if(buffer==NULL){
			return NULL;
		}
		buffer->create(usage,access,vertexFormat,size);
	}

	if(buffer!=NULL){
		mVertexBuffers.add(buffer);

		buffer->setDestroyedListener(this);
	}

	return buffer;
}

PixelBuffer::ptr BufferManager::createPixelBuffer(int usage,int access,int pixelFormat,int width,int height,int depth){
	RenderDevice *renderDevice=mEngine->getRenderDevice();
	TextureFormat::ptr textureFormat(new TextureFormat(TextureFormat::Dimension_D2,pixelFormat,width,height,depth,1));
	PixelBuffer::ptr buffer;
	if(mEngine->isBackable()){
		BackableBuffer::ptr backableBuffer=new BackableBuffer();
		backableBuffer->create(usage,access,textureFormat);
		if(renderDevice!=NULL){
			PixelBuffer::ptr back=renderDevice->createPixelBuffer();
			backableBuffer->setBack(back,renderDevice);
		}
		buffer=backableBuffer;
	}
	else if(renderDevice!=NULL){
		buffer=renderDevice->createPixelBuffer();
		if(buffer==NULL){
			return NULL;
		}
		buffer->create(usage,access,textureFormat);
	}

	if(buffer!=NULL){
		mPixelBuffers.add(buffer);

		buffer->setDestroyedListener(this);
	}

	return buffer;
}

VariableBuffer::ptr BufferManager::createVariableBuffer(int usage,int access,VariableBufferFormat::ptr format){
	RenderDevice *renderDevice=mEngine->getRenderDevice();
	VariableBuffer::ptr buffer;
	if(mEngine->hasShader(Shader::ShaderType_VERTEX)){
		BackableBuffer::ptr backableBuffer(new BackableBuffer());
		backableBuffer->create(usage,access,format);
		if(renderDevice!=NULL){
			VariableBuffer::ptr back(renderDevice->createVariableBuffer());
			backableBuffer->setBack(back);
		}
		buffer=backableBuffer;
	}
	else if(renderDevice!=NULL){
		buffer=VariableBuffer::ptr(renderDevice->createVariableBuffer());
		if(buffer==NULL){
			return NULL;
		}
		buffer->create(usage,access,format);
	}

	if(buffer!=NULL){
		mVariableBuffers.add(buffer);

		buffer->setDestroyedListener(this);
	}

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
				int dstElementSize=VertexFormat::getFormatSize(vertexFormat->getElementFormat(i));
				int srcElementSize=VertexFormat::getFormatSize(oldVertexFormat->getElementFormat(oldElementIndex));
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
		int oldElementIndex=oldVertexFormat->findElement(vertexFormat->getElementSemantic(i));
		if(oldElementIndex>0){
			int elementSize=VertexFormat::getFormatSize(vertexFormat->getElementFormat(i));
			int srcOffset=oldVertexFormat->getElementOffset(oldElementIndex);
			int dstOffset=vertexFormat->getElementOffset(i);
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

	for(i=0;i<mVariableBuffers.size();++i){
		VariableBuffer::ptr buffer=mVariableBuffers[i];
		if(buffer->getRootVariableBuffer()!=buffer){
			VariableBuffer::ptr back(renderDevice->createVariableBuffer());
			shared_static_cast<BackableBuffer>(buffer)->setBack(back);
		}
	}
}

void BufferManager::contextDeactivate(RenderDevice *renderDevice){
	Logger::debug("BufferManager::contextDeactivate");

	int i;
	for(i=0;i<mVariableBuffers.size();++i){
		VariableBuffer::ptr buffer=mVariableBuffers[i];
		if(buffer->getRootVariableBuffer()!=buffer){
			shared_static_cast<BackableBuffer>(buffer)->setBack(VariableBuffer::ptr());
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
	for(i=0;i<mVariableBuffers.size();++i){
		mVariableBuffers[i]->resetDestroy();
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
	for(i=0;i<mVariableBuffers.size();++i){
		mVariableBuffers[i]->resetCreate();
	}
}

void BufferManager::resourceDestroyed(Resource *resource){
	if(mIndexBuffers.remove(resource)==false){
		if(mVertexBuffers.remove(resource)==false){
			if(mPixelBuffers.remove(resource)==false){
				if(mVariableBuffers.remove(resource)==false){
					mVertexFormats.remove(resource);
				}
			}
		}
	}
}

bool BufferManager::useTriFan(){
	return !mEngine->isBackable() && mEngine->getRenderCaps().triangleFan;
}

void BufferManager::outputVariableBufferFormat(VariableBufferFormat::ptr format){
	String string;

	Logger::alert(Categories::TOADLET_TADPOLE,"VariableBufferFormat:"+format->getName()+" primary:"+format->getPrimary()+" dataSize:"+format->getDataSize()+" numVariables:"+format->getSize());
	int i;
	for(i=0;i<format->getSize();++i){
		outputVariable(format->getVariable(i),"\t");
	}
}

void BufferManager::outputVariable(VariableBufferFormat::Variable *variable,const String &tabs){
	Logger::alert(Categories::TOADLET_TADPOLE,tabs+"Variable:"+variable->getName()+" format:"+variable->getFormat()+" offset:"+variable->getOffset()+" index:"+variable->getIndex()+" arraySize:"+variable->getArraySize()+" structSize:"+variable->getStructSize());

	int i;
	for(i=0;i<variable->getStructSize();++i){
		outputVariable(variable->getStructVariable(i),tabs+"\t");
	}
}

}
}

