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
#include "GLRenderer.h"
#include <toadlet/peeper/CapabilitySet.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

GLBuffer::GLBuffer(GLRenderer *renderer):
	mRenderer(NULL),

	mUsageFlags(0),
	mAccessType(AccessType_NO_ACCESS),
	mSize(0),
	mDataSize(0),

	mIndexFormat(IndexFormat_UINT_8),
	//mVertexFormat,
	mVertexSize(0),
	//mColorElements,

	mHandle(0),
	mTarget(0),
	mLockType(AccessType_NO_ACCESS),
	mBacking(false),
	mData(NULL)
{
	mRenderer=renderer;
}

GLBuffer::~GLBuffer(){
	destroy();
}

bool GLBuffer::create(int usageFlags,AccessType accessType,IndexFormat indexFormat,int size){
	destroy();

	mUsageFlags=usageFlags;
	mAccessType=accessType;
	mSize=size;
	mIndexFormat=indexFormat;
	mDataSize=mIndexFormat*mSize;
	
	mTarget=GL_ELEMENT_ARRAY_BUFFER;
	if(mRenderer->getCapabilitySet().hardwareIndexBuffers){
		glGenBuffers(1,&mHandle);
		glBindBuffer(mTarget,mHandle);
		GLenum usage=getBufferUsage(mUsageFlags,mAccessType);
		glBufferData(mTarget,mDataSize,NULL,usage);
	}

	mMapping=mRenderer->useMapping();
	if(mRenderer->getCapabilitySet().hardwareIndexBuffers==false || mMapping==false){
		mData=new uint8[mDataSize];
		mBacking=true;
	}
	
	return true;
}

bool GLBuffer::create(int usageFlags,AccessType accessType,VertexFormat::ptr vertexFormat,int size){
	destroy();

	mUsageFlags=usageFlags;
	mAccessType=accessType;
	mSize=size;
	mVertexFormat=vertexFormat;
	mVertexSize=vertexFormat->getVertexSize();
	mDataSize=mVertexSize*mSize;
	
	mTarget=GL_ARRAY_BUFFER;
	if(mRenderer->getCapabilitySet().hardwareVertexBuffers){
		glGenBuffers(1,&mHandle);
		glBindBuffer(mTarget,mHandle);
		GLenum usage=getBufferUsage(mUsageFlags,mAccessType);
		glBufferData(mTarget,mDataSize,NULL,usage);
	}

	#if defined(TOADLET_BIG_ENDIAN)
		int i;
		for(i=0;i<mVertexFormat.getNumVertexElements();++i){
			const VertexElement &vertexElement=mVertexFormat.getVertexElement(i);
			if(vertexElement.type==VertexElement::Type_COLOR && vertexElement.format==VertexElement::Format_COLOR_RGBA){
				mColorElementsToEndianSwap.add(vertexElement);
			}
		}
	#endif

	mMapping=mRenderer->getCapabilitySet().hardwareVertexBuffers && mRenderer->useMapping();
	if(mMapping==false){
		mData=new uint8[mDataSize];
		mBacking=true;
	}
	
	return true;
}

bool GLBuffer::destroy(){
	if(mHandle!=0){
		glDeleteBuffers(1,&mHandle);
		mHandle=0;
	}
	
	if(mData!=NULL && mBacking){
		delete[] mData;
		mData=NULL;
	}

	return true;
}

uint8 *GLBuffer::lock(AccessType accessType){
	mRenderer->internal_getStatisticsSet().bufferLockCount++;

	mLockType=accessType;

	if(mMapping){
		GLenum lock=0;
		switch(accessType){
			case AccessType_READ_ONLY:
				lock=GL_READ_ONLY;
			break;
			case AccessType_WRITE_ONLY:
				lock=GL_WRITE_ONLY;
			break;
			case AccessType_READ_WRITE:
				lock=GL_READ_WRITE;
			break;
		}

		glBindBuffer(mTarget,mHandle);
		mData=(uint8*)glMapBuffer(mTarget,lock);
	}

	#if defined(TOADLET_BIG_ENDIAN)
		if(mLockType!=AccessType_WRITE_ONLY){
			int i,j;
			for(i=0;i<mColorElementsToEndianSwap.size();++i){
				const VertexElement &vertexElement=mColorElementsToEndianSwap[i];
				for(j=0;j<numVertexes;++j){
					littleUInt32InPlace(*(ByteColor*)(mData+vertexSize*j+vertexElement.offset));
				}
			}
		}
	#endif

	return mData;
}

bool GLBuffer::unlock(){
	#if defined(TOADLET_BIG_ENDIAN)
		if(mLockType!=AccessType_READ_ONLY){
			int i,j;
			for(i=0;i<mColorElementsToEndianSwap.size();++i){
				const VertexElement &vertexElement=mColorElementsToEndianSwap[i];
				for(j=0;j<numVertexes;++j){
					littleUInt32InPlace(*(ByteColor*)(mData+vertexSize*j+vertexElement.offset));
				}
			}
		}
	#endif

	if(mMapping){
		glBindBuffer(mTarget,mHandle);
		glUnmapBuffer(mTarget);
		mData=NULL;
	}
	else{
		glBindBuffer(mTarget,mHandle);
		glBufferSubData(mTarget,0,mDataSize,mData);
	}

	return true;
}

GLenum GLBuffer::getBufferUsage(int usageFlags,AccessType accessType){
	GLenum usage=0;
	switch(usageFlags){
		case UsageFlags_STATIC:
			#if defined(TOADLET_HAS_GLES)
				usage=GL_STATIC_DRAW;
			#else
				switch(accessType){
					case AccessType_READ_ONLY:
						usage=GL_STATIC_READ;
					break;
					case AccessType_WRITE_ONLY:
					case AccessType_READ_WRITE:
						usage=GL_STATIC_DRAW;
					break;
					case AccessType_NO_ACCESS:
						usage=GL_STATIC_COPY;
					break;
				}
			#endif
		break;
		case UsageFlags_STREAM:
			#if defined(TOADLET_HAS_GLES)
				usage=GL_DYNAMIC_DRAW;
			#else
				switch(accessType){
					case AccessType_READ_ONLY:
						usage=GL_STREAM_READ;
					break;
					case AccessType_WRITE_ONLY:
					case AccessType_READ_WRITE:
						usage=GL_STREAM_DRAW;
					break;
					case AccessType_NO_ACCESS:
						usage=GL_STREAM_COPY;
					break;
				}
			#endif
		break;
		case UsageFlags_DYNAMIC:
			#if defined(TOADLET_HAS_GLES)
				usage=GL_DYNAMIC_DRAW;
			#else
				switch(accessType){
					case AccessType_READ_ONLY:
						usage=GL_DYNAMIC_READ;
					break;
					case AccessType_WRITE_ONLY:
					case AccessType_READ_WRITE:
						usage=GL_DYNAMIC_DRAW;
					break;
					case AccessType_NO_ACCESS:
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
