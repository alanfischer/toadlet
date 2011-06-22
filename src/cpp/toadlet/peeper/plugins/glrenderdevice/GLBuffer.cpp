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

#include "GLBuffer.h"
#include "GLRenderDevice.h"
#include "GLVertexFormat.h"
#include <toadlet/egg/EndianConversion.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <string.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;

namespace toadlet{
namespace peeper{

GLBuffer::GLBuffer(GLRenderDevice *renderDevice):
	mDevice(NULL),

	mListener(NULL),
	mUsage(0),
	mAccess(0),
	mDataSize(0),
	mSize(0),
	mWidth(0),mHeight(0),mDepth(0),

	mIndexFormat((IndexFormat)0),
	//mVertexFormat,
	mPixelFormat(0),
	//mElementOffsets,

	mHandle(0),
	mTarget(0),
	mLockAccess(0),
	mData(NULL)
{
	mDevice=renderDevice;
}

GLBuffer::~GLBuffer(){
	destroy();
}

bool GLBuffer::create(int usage,int access,IndexFormat indexFormat,int size){
	mUsage=usage;
	mAccess=access;
	mSize=size;
	mIndexFormat=indexFormat;
	mDataSize=mIndexFormat*mSize;

	// Even if not supported in hardware, we simulate them
	#if defined(TOADLET_HAS_GLIBOS)
		mTarget=GL_ELEMENT_ARRAY_BUFFER;
	#endif
	createContext();

	mMapping=mDevice->hardwareMappingSupported(this);
	if(mMapping==false){
		mData=new uint8[mDataSize];
	}
	
	return true;
}

bool GLBuffer::create(int usage,int access,VertexFormat::ptr vertexFormat,int size){
	mUsage=usage;
	mAccess=access;
	mSize=size;
	mVertexFormat=vertexFormat;
	mDataSize=mVertexFormat->getVertexSize()*mSize;
	
	// Even if not supported in hardware, we simulate them
	#if defined(TOADLET_HAS_GLVBOS)
		mTarget=GL_ARRAY_BUFFER;
	#endif
	createContext();

	mMapping=mDevice->hardwareMappingSupported(this);
	if(mMapping==false){
		mData=new uint8[mDataSize];
	}

	int i;
	for(i=0;i<mVertexFormat->getNumElements();++i){
		if(mHandle!=0){
			mElementOffsets.add(reinterpret_cast<uint8*>(mVertexFormat->getOffset(i)));
		}
		else{
			mElementOffsets.add(mData+mVertexFormat->getOffset(i));
		}
	}

	return true;
}

bool GLBuffer::create(int usage,int access,int pixelFormat,int width,int height,int depth){
	destroy();

	mUsage=usage;
	mAccess=access;
	mWidth=width;
	mHeight=height;
	mDepth=depth;
	mPixelFormat=pixelFormat;
	mDataSize=ImageFormatConversion::getRowPitch(mPixelFormat,mWidth)*mHeight*mDepth;

	#if defined(TOADLET_HAS_GLPBOS)
		mTarget=GL_PIXEL_UNPACK_BUFFER;
	#else
		Error::unknown(Categories::TOADLET_PEEPER,
			"PixelBuffers not supported");
		return false;
	#endif
	createContext();

	mMapping=mDevice->hardwareMappingSupported(this);
	if(mMapping==false){
		mData=new uint8[mDataSize];
	}

	return true;
}

bool GLBuffer::create(int usage,int access,VariableBufferFormat::ptr variableFormat){
	destroy();

	mUsage=usage;
	mAccess=access;
	mVariableFormat=variableFormat;
	mDataSize=variableFormat->getSize();

	#if defined(TOADLET_HAS_GLUBOS)
		mTarget=GL_UNIFORM_BUFFER;
	#else
		Error::unknown(Categories::TOADLET_PEEPER,
			"VariableBuffers not supported");
		return false;
	#endif
	createContext();

	mMapping=mDevice->hardwareMappingSupported(this);
	if(mMapping==false){
		mData=new uint8[mDataSize];
	}

	return true;
}

void GLBuffer::destroy(){
	destroyContext();

	if(mData!=NULL){
		delete[] mData;
		mData=NULL;
	}

	mElementOffsets.clear();

	if(mListener!=NULL){
		if(mIndexFormat>0){
			mListener->bufferDestroyed((IndexBuffer*)this);
		}
		else if(mVertexFormat!=NULL){
			mListener->bufferDestroyed((VertexBuffer*)this);
		}
		else if(mPixelFormat>0){
			mListener->bufferDestroyed((PixelBuffer*)this);
		}
		else{
			mListener->bufferDestroyed((VariableBuffer*)this);
		}
		mListener=NULL;
	}
}

bool GLBuffer::createContext(){
	bool result=true;
	if(mDevice->hardwareBuffersSupported(this)){
		glGenBuffers(1,&mHandle);
		glBindBuffer(mTarget,mHandle);
		GLenum usage=getBufferUsage(mUsage,mAccess);
		glBufferData(mTarget,mDataSize,NULL,usage);
		glBindBuffer(mTarget,0);

		TOADLET_CHECK_GLERROR("GLBuffer::createContext");
		result=mHandle!=0;
	}

	return result;
}

bool GLBuffer::destroyContext(){
	if(mHandle!=0){
		glDeleteBuffers(1,&mHandle);
		mHandle=0;

		TOADLET_CHECK_GLERROR("GLBuffer::destroyContext");
	}

	return true;
}

uint8 *GLBuffer::lock(int lockAccess){
	mLockAccess=lockAccess;

	#if !defined(TOADLET_HAS_GLES)
		if(mMapping){
			GLenum gllock=0;
			switch(mLockAccess){
				case Access_BIT_READ:
					gllock=GL_READ_ONLY;
				break;
				case Access_BIT_WRITE:
					gllock=GL_WRITE_ONLY;
				break;
				case Access_READ_WRITE:
					gllock=GL_READ_WRITE;
				break;
				default:
				break;
			}

			glBindBuffer(mTarget,mHandle);
			mData=(uint8*)glMapBuffer(mTarget,gllock);
		}
		else
	#endif
	{
		if(mData==NULL){
			mData=new uint8[mDataSize];
		}
	}

	#if defined(TOADLET_LITTLE_ENDIAN)
		if(mVertexFormat!=NULL){
			// We do this even if its write only, since the unlocking will write it back, it would get messed up if we didn't swap in all situations
			int vertexSize=mVertexFormat->getVertexSize();
			int i,j;
			for(i=0;i<mVertexFormat->getNumElements();++i){
				if(mVertexFormat->getFormat(i)==VertexFormat::Format_TYPE_COLOR_RGBA){
					tbyte *data=mData+mVertexFormat->getOffset(i);
					for(j=0;j<mSize;++j){
						swap4(*(uint32*)(data+vertexSize*j));
					}
				}
			}
		}
	#endif

	TOADLET_CHECK_GLERROR("GLBuffer::lock");
	
	return mData;
}

bool GLBuffer::unlock(){
	#if defined(TOADLET_LITTLE_ENDIAN)
		if(mVertexFormat!=NULL){
			// We do this even if its read only, since we have to do it in all situations for locking
			int vertexSize=mVertexFormat->getVertexSize();
			int i,j;
			for(i=0;i<mVertexFormat->getNumElements();++i){
				if(mVertexFormat->getFormat(i)==VertexFormat::Format_TYPE_COLOR_RGBA){
					tbyte *data=mData+mVertexFormat->getOffset(i);
					for(j=0;j<mSize;++j){
						swap4(*(uint32*)(data+vertexSize*j));
					}
				}
			}
		}
	#endif

	#if !defined(TOADLET_HAS_GLES)
		if(mMapping){
			glBindBuffer(mTarget,mHandle);
			glUnmapBuffer(mTarget);
			glBindBuffer(mTarget,0);
			mData=NULL;
		}
		else
	#endif
	{
		if(mLockAccess!=Access_BIT_READ && mHandle!=0){
			glBindBuffer(mTarget,mHandle);
			glBufferSubData(mTarget,0,mDataSize,mData);
			glBindBuffer(mTarget,0);
		}

		// Only delete our data if we have no desire to read it and a hardware back 
		if(mData!=NULL && (mAccess&Access_BIT_READ)==0 && mHandle!=0){
			delete[] mData;
			mData=NULL;
		}
	}

	TOADLET_CHECK_GLERROR("GLBuffer::unlock");

	return true;
}

bool GLBuffer::update(tbyte *data,int start,int size){
	memcpy(mData+start,data,size);

	if(mHandle>0){
		glBindBuffer(mTarget,mHandle);
		glBufferSubData(mTarget,start,size,data);
		glBindBuffer(mTarget,0);
	}

	return true;
}

bool GLBuffer::activateVariableBuffer(int i){
	if(mHandle>0){
		#if defined(TOADLET_HAS_UBOS)
			glUniformBlockBinding(mLastShaderState->mHandle,0,0);
			TOADLET_CHECK_GLERROR("setBuffer0");
			glBindBufferBase(GL_UNIFORM_BUFFER,0,glbuffer->mHandle);
		#endif
	}
	else{

	}

	return true;
}

GLenum GLBuffer::getBufferUsage(int usage,int access){
	GLenum glusage=0;
	switch(usage){
		case Usage_BIT_STATIC:
			#if defined(TOADLET_HAS_GLES)
				glusage=GL_STATIC_DRAW;
			#else
				switch(access){
					case Access_BIT_READ:
						glusage=GL_STATIC_READ;
					break;
					case Access_BIT_WRITE:
					case Access_READ_WRITE:
						glusage=GL_STATIC_DRAW;
					break;
					case 0:
						glusage=GL_STATIC_COPY;
					break;
				}
			#endif
		break;
		case Usage_BIT_STREAM:
			#if defined(TOADLET_HAS_GLES)
				glusage=GL_DYNAMIC_DRAW;
			#else
				switch(access){
					case Access_BIT_READ:
						glusage=GL_STREAM_READ;
					break;
					case Access_BIT_WRITE:
					case Access_READ_WRITE:
						glusage=GL_STREAM_DRAW;
					break;
					case 0:
						glusage=GL_STREAM_COPY;
					break;
				}
			#endif
		break;
		case Usage_BIT_DYNAMIC:
		case Usage_BIT_STAGING:
			#if defined(TOADLET_HAS_GLES)
				glusage=GL_DYNAMIC_DRAW;
			#else
				switch(access){
					case Access_BIT_READ:
						glusage=GL_DYNAMIC_READ;
					break;
					case Access_BIT_WRITE:
					case Access_READ_WRITE:
						glusage=GL_DYNAMIC_DRAW;
					break;
					case 0:
						glusage=GL_DYNAMIC_COPY;
					break;
				}
			#endif
		break;
	}
	return glusage;
}

}
}
