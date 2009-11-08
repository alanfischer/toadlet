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
#include <string.h>

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
	bool result=createContext();

	mMapping=mRenderer->useMapping();
	if(mRenderer->getCapabilitySet().hardwareIndexBuffers==false || mMapping==false){
		mData=new uint8[mDataSize];
		mBacking=true;
	}
	
	return result;
}

bool GLBuffer::create(int usageFlags,AccessType accessType,VertexFormat::ptr vertexFormat,int size){
	destroy();

	mUsageFlags=usageFlags;
	mAccessType=accessType;
	mSize=size;
	mVertexFormat=vertexFormat;
	mVertexSize=vertexFormat->vertexSize;
	mDataSize=mVertexSize*mSize;
	
	mTarget=GL_ARRAY_BUFFER;
	createContext();

	#if defined(TOADLET_BIG_ENDIAN)
		int i;
		for(i=0;i<mVertexFormat->vertexElements.size();++i){
			const VertexElement &vertexElement=mVertexFormat->vertexElements[i];
			if(vertexElement.format==VertexElement::Format_COLOR_RGBA){
				mColorElementsToEndianSwap.add(vertexElement);
			}
		}
	#endif

	mMapping=
		#if defined(TOADLET_HAS_GLES)
			false;
		#else
			mRenderer->getCapabilitySet().hardwareVertexBuffers && mRenderer->useMapping();
		#endif
	if(mMapping==false){
		mData=new uint8[mDataSize];
		mBacking=true;
	}

	int numVertexElements=mVertexFormat->vertexElements.size();
	mElementTypes.resize(numVertexElements);
	mElementCounts.resize(numVertexElements);
	mElementOffsets.resize(numVertexElements);
	int i;
	for(i=0;i<numVertexElements;++i){
		const VertexElement &element=mVertexFormat->vertexElements[i];
		mElementTypes[i]=GLRenderer::getGLDataType(element.format);
		mElementCounts[i]=GLRenderer::getGLElementCount(element.format);
		if(mHandle!=0){
			mElementOffsets[i]=(uint8*)element.offset;
		}
		else{
			mElementOffsets[i]=mData+element.offset;
		}
	}

	return true;
}

void GLBuffer::destroy(){
	destroyContext(false);

	if(mData!=NULL){
		delete[] mData;
		mData=NULL;
	}

	mElementTypes.clear();
	mElementCounts.clear();
	mElementOffsets.clear();
}

bool GLBuffer::createContext(){
	if((mTarget==GL_ELEMENT_ARRAY_BUFFER && mRenderer->getCapabilitySet().hardwareIndexBuffers) ||
		(mTarget==GL_ARRAY_BUFFER && mRenderer->getCapabilitySet().hardwareIndexBuffers))
	{
		glGenBuffers(1,&mHandle);
		glBindBuffer(mTarget,mHandle);
		GLenum usage=getBufferUsage(mUsageFlags,mAccessType);
		glBufferData(mTarget,mDataSize,NULL,usage);
	}

	#if !defined(TOADLET_HAS_GLES)
		if(mMapping && mBacking){
			uint8 *data=lock(AccessType_WRITE_ONLY);
			memcpy(data,mData,mDataSize);
			unlock();

			delete[] mData;
			mData=NULL;
			mBacking=true;
		}
	#endif

	TOADLET_CHECK_GLERROR("createContext");

	return true;
}

void GLBuffer::destroyContext(bool backData){
	#if !defined(TOADLET_HAS_GLES)
		if(mMapping && backData){
			mData=new uint8[mDataSize];
			mBacking=true;

			uint8 *data=lock(AccessType_READ_ONLY);
			memcpy(mData,data,mDataSize);
			unlock();
		}
	#endif

	if(mHandle!=0){
		glDeleteBuffers(1,&mHandle);
		mHandle=0;
	}

	TOADLET_CHECK_GLERROR("destroyContext");
}

uint8 *GLBuffer::lock(AccessType accessType){
	mRenderer->mStatisticsSet.bufferLockCount++;

	mLockType=accessType;

	#if !defined(TOADLET_HAS_GLES)
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
	#endif

	#if defined(TOADLET_BIG_ENDIAN)
		// We do this even if its write only, since the unlocking will write it back, it would get messed up if we didn't swap in all situations
		int i,j;
		for(i=0;i<mColorElementsToEndianSwap.size();++i){
			const VertexElement &vertexElement=mColorElementsToEndianSwap[i];
			for(j=0;j<numVertexes;++j){
				littleUInt32InPlace(*(ByteColor*)(mData+vertexSize*j+vertexElement.offset));
			}
		}
	#endif

	TOADLET_CHECK_GLERROR("lock");

	return mData;
}

bool GLBuffer::unlock(){
	#if defined(TOADLET_BIG_ENDIAN)
		// We do this even if its read only, since we have to do it in all situations for locking
		int i,j;
		for(i=0;i<mColorElementsToEndianSwap.size();++i){
			const VertexElement &vertexElement=mColorElementsToEndianSwap[i];
			for(j=0;j<numVertexes;++j){
				littleUInt32InPlace(*(ByteColor*)(mData+vertexSize*j+vertexElement.offset));
			}
		}
	#endif

	#if !defined(TOADLET_HAS_GLES)
		if(mMapping){
			glBindBuffer(mTarget,mHandle);
			glUnmapBuffer(mTarget);
			mData=NULL;
		}
		else
	#endif
	if(mLockType!=AccessType_READ_ONLY && mHandle!=0){
		glBindBuffer(mTarget,mHandle);
		glBufferSubData(mTarget,0,mDataSize,mData);
	}

	TOADLET_CHECK_GLERROR("unlock");

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
