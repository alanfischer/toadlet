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

#include "GLRenderer.h"
#include "GLBufferPeer.h"
#include <toadlet/peeper/CapabilitySet.h>
#include <toadlet/egg/Logger.h>
#if defined(TOADLET_BIG_ENDIAN)
	#include <toadlet/egg/EndianConversion.h>
	#include <toadlet/peeper/VertexBuffer.h>
#endif
#include <stdlib.h>
#if defined(TOADLET_PLATFORM_POSIX)
	#include <string.h>
#endif

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

GLBufferPeer::GLBufferPeer(GLRenderer *renderer,Buffer *buffer):
	renderer(NULL),
	type(Buffer::Type_INDEX),
	bufferHandle(0),
	bufferTarget(0),
	data(NULL),
	bufferedData(false)
	#if !defined(TOADLET_HAS_GLMAPBUFFER)
		,usageType(Buffer::UsageType_STATIC),
		accessType(Buffer::AccessType_READ_ONLY),
		bufferSize(0)
	#endif
	#if defined(TOADLET_BIG_ENDIAN)
		,lockType(Buffer::LockType_READ_ONLY),
		numVertexes(0),
		vertexSize(0)
		//colorElementsToEndianSwap
	#endif
{
	this->renderer=renderer;
	type=buffer->getType();

	if(type==Buffer::Type_INDEX){
		bufferTarget=GL_ELEMENT_ARRAY_BUFFER;
	}
	else if(type==Buffer::Type_VERTEX){
		bufferTarget=GL_ARRAY_BUFFER;

		#if defined(TOADLET_BIG_ENDIAN)
			numVertexes=((VertexBuffer*)buffer)->getSize();
			const VertexFormat &vertexFormat=((VertexBuffer*)buffer)->getVertexFormat();
			vertexSize=vertexFormat.getVertexSize();
			int i;
			for(i=0;i<vertexFormat.getNumVertexElements();++i){
				const VertexElement &vertexElement=vertexFormat.getVertexElement(i);
				if(vertexElement.type==VertexElement::Type_COLOR && vertexElement.format==VertexElement::Format_COLOR_RGBA){
					colorElementsToEndianSwap.addElement(vertexElement);
				}
			}
		#endif
	}

	if(bufferTarget!=0){
		glGenBuffers(1,&bufferHandle);
		#if defined(TOADLET_HAS_GLMAPBUFFER)
			Logger::log(Categories::TOADLET_PEEPER,Logger::Level_EXCESSIVE,
				String("Allocating GLBufferPeer of size:")+buffer->getBufferSize());

			GLenum usage=getBufferUsage(buffer->getUsageType(),buffer->getAccessType());
			glBindBuffer(bufferTarget,bufferHandle);
			glBufferData(bufferTarget,buffer->getBufferSize(),buffer->internal_getData(),usage);
		#else
			usageType=buffer->getUsageType();
			accessType=buffer->getAccessType();
			bufferSize=buffer->getBufferSize();

			uint8 *bufferData=buffer->internal_getData();
			if(bufferData!=NULL){
				uint8 *data=lock(Buffer::LockType_WRITE_ONLY);
				if(data!=NULL){
					memcpy(data,bufferData,bufferSize);
				}
				unlock();
			}
		#endif
	}

	TOADLET_CHECK_GLERROR("GLBufferPeer::GLBufferPeer");
}

GLBufferPeer::~GLBufferPeer(){
	if(bufferHandle!=0){
		glDeleteBuffers(1,&bufferHandle);
		bufferHandle=0;
	}

	TOADLET_CHECK_GLERROR("GLBufferPeer::~GLBufferPeer");
}

// THOUGHT: We could store the last bound buffer of this type in the renderer, to make sure I dont rebind it
uint8 *GLBufferPeer::lock(Buffer::LockType lockType){
	renderer->internal_getStatisticsSet().bufferLockCount++;

	#if defined(TOADLET_HAS_GLMAPBUFFER)
		GLenum lock=0;
		switch(lockType){
			case Buffer::LockType_READ_ONLY:
				lock=GL_READ_ONLY;
			break;
			case Buffer::LockType_WRITE_ONLY:
				lock=GL_WRITE_ONLY;
			break;
			case Buffer::LockType_READ_WRITE:
				lock=GL_READ_WRITE;
			break;
		}

		glBindBuffer(bufferTarget,bufferHandle);
		data=(uint8*)glMapBuffer(bufferTarget,lock);
	#else
		data=renderer->allocScratchBuffer(bufferSize);
	#endif

	#if defined(TOADLET_BIG_ENDIAN)
		this->lockType=lockType;
		if(lockType!=Buffer::LockType_WRITE_ONLY){
			int i,j;
			for(i=0;i<colorElementsToEndianSwap.size();++i){
				const VertexElement &vertexElement=colorElementsToEndianSwap[i];
				for(j=0;j<numVertexes;++j){
					littleUInt32InPlace(*(ByteColor*)(data+vertexSize*j+vertexElement.offset));
				}
			}
		}
	#endif

	return data;
}

bool GLBufferPeer::unlock(){
	#if defined(TOADLET_BIG_ENDIAN)
		if(lockType!=Buffer::LockType_READ_ONLY){
			int i,j;
			for(i=0;i<colorElementsToEndianSwap.size();++i){
				const VertexElement &vertexElement=colorElementsToEndianSwap[i];
				for(j=0;j<numVertexes;++j){
					littleUInt32InPlace(*(ByteColor*)(data+vertexSize*j+vertexElement.offset));
				}
			}
		}
	#endif

	#if defined(TOADLET_HAS_GLMAPBUFFER)
		glBindBuffer(bufferTarget,bufferHandle);
		glUnmapBuffer(bufferTarget);
	#else
		if(bufferedData==false){
			bufferedData=true;
			GLenum usage=getBufferUsage(usageType,accessType);
			glBindBuffer(bufferTarget,bufferHandle);
			glBufferData(bufferTarget,bufferSize,data,usage);
		}
		else{
			glBindBuffer(bufferTarget,bufferHandle);
			glBufferSubData(bufferTarget,0,bufferSize,data);
		}

		renderer->freeScratchBuffer(data);
	#endif
	data=NULL;
	return true;
}

GLenum GLBufferPeer::getBufferUsage(Buffer::UsageType usageType,Buffer::AccessType accessType){
	GLenum usage=0;
	switch(usageType){
		case Buffer::UsageType_STATIC:
			#if defined(TOADLET_HAS_GLES)
				usage=GL_STATIC_DRAW;
			#else
				switch(accessType){
					case Buffer::AccessType_READ_ONLY:
						usage=GL_STATIC_READ;
					break;
					case Buffer::AccessType_WRITE_ONLY:
					case Buffer::AccessType_READ_WRITE:
						usage=GL_STATIC_DRAW;
					break;
					case Buffer::AccessType_NO_ACCESS:
						usage=GL_STATIC_COPY;
					break;
				}
			#endif
		break;
		case Buffer::UsageType_STREAM:
			#if defined(TOADLET_HAS_GLES)
				usage=GL_DYNAMIC_DRAW;
			#else
				switch(accessType){
					case Buffer::AccessType_READ_ONLY:
						usage=GL_STREAM_READ;
					break;
					case Buffer::AccessType_WRITE_ONLY:
					case Buffer::AccessType_READ_WRITE:
						usage=GL_STREAM_DRAW;
					break;
					case Buffer::AccessType_NO_ACCESS:
						usage=GL_STREAM_COPY;
					break;
				}
			#endif
		break;
		case Buffer::UsageType_DYNAMIC:
			#if defined(TOADLET_HAS_GLES)
				usage=GL_DYNAMIC_DRAW;
			#else
				switch(accessType){
					case Buffer::AccessType_READ_ONLY:
						usage=GL_DYNAMIC_READ;
					break;
					case Buffer::AccessType_WRITE_ONLY:
					case Buffer::AccessType_READ_WRITE:
						usage=GL_DYNAMIC_DRAW;
					break;
					case Buffer::AccessType_NO_ACCESS:
						usage=GL_DYNAMIC_COPY;
					break;
				}
			#endif
		break;
	}
	return usage;
}

}
}
