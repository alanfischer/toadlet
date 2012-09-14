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
#include "GLTexture.h"
#include "GLVertexFormat.h"
#include <toadlet/egg/EndianConversion.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/peeper/BackableBuffer.h>

namespace toadlet{
namespace peeper{

GLBuffer::GLBuffer(GLRenderDevice *renderDevice):BaseResource(),
	mDevice(NULL),

	mUsage(0),
	mAccess(0),
	mDataSize(0),
	mSize(0),

	mIndexFormat((IndexFormat)0),
	//mVertexFormat,
	//mTextureFormat,
	//mElementOffsets,
	mHasTranspose(false),

	mHandle(0),
	mTarget(0),
	mLockAccess(0),
	mData(NULL)
{
	mDevice=renderDevice;
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
			mElementOffsets.add(reinterpret_cast<uint8*>(mVertexFormat->getElementOffset(i)));
		}
		else{
			mElementOffsets.add(mData+mVertexFormat->getElementOffset(i));
		}
	}

	return true;
}

bool GLBuffer::create(int usage,int access,TextureFormat::ptr textureFormat){
	destroy();

	mUsage=usage;
	mAccess=access;
	mTextureFormat=textureFormat;
	mDataSize=textureFormat->getDataSize();

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
	mDataSize=variableFormat->getDataSize();

	#if defined(TOADLET_HAS_GLUBOS)
		mTarget=GL_UNIFORM_BUFFER;
	#endif
	createContext();

	mMapping=mDevice->hardwareMappingSupported(this);
	if(mMapping==false){
		mData=new uint8[mDataSize];
	}
	else{
		mHasTranspose=false;
		int i;
		for(i=0;i<mVariableFormat->getSize();++i){
			mHasTranspose|=((mVariableFormat->getVariable(i)->getFormat()&VariableBufferFormat::Format_BIT_TRANSPOSE)!=0);
		}
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

	// Don't call BaseResource::destroy()
	if(mListener!=NULL){
		if(mIndexFormat>0){
			mListener->resourceDestroyed((IndexBuffer*)this);
		}
		else if(mVertexFormat!=NULL){
			mListener->resourceDestroyed((VertexBuffer*)this);
		}
		else if(mTextureFormat!=NULL){
			mListener->resourceDestroyed((PixelBuffer*)this);
		}
		else{
			mListener->resourceDestroyed((VariableBuffer*)this);
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
			if((mLockAccess&Access_READ_WRITE)==Access_READ_WRITE){
				gllock=GL_READ_WRITE;
			}
			else if((mLockAccess&Access_BIT_READ)!=0){
				gllock=GL_READ_ONLY;
			}
			else if((mLockAccess&Access_BIT_WRITE)!=0){
				gllock=GL_WRITE_ONLY;
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
				if(mVertexFormat->getElementFormat(i)==VertexFormat::Format_TYPE_COLOR_RGBA){
					tbyte *data=mData+mVertexFormat->getElementOffset(i);
					for(j=0;j<mSize;++j){
						swap4(*(uint32*)(data+vertexSize*j));
					}
				}
			}
		}
	#endif

	if(mVariableFormat!=NULL && mHasTranspose){
		BackableBuffer::unpackVariables(mVariableFormat,mData);
	}

	TOADLET_CHECK_GLERROR("GLBuffer::lock");
	
	return mData;
}

bool GLBuffer::unlock(){
	if(mVariableFormat!=NULL && mHasTranspose){
		BackableBuffer::packVariables(mVariableFormat,mData);
	}

	#if defined(TOADLET_LITTLE_ENDIAN)
		if(mVertexFormat!=NULL){
			// We do this even if its read only, since we have to do it in all situations for locking
			int vertexSize=mVertexFormat->getVertexSize();
			int i,j;
			for(i=0;i<mVertexFormat->getNumElements();++i){
				if(mVertexFormat->getElementFormat(i)==VertexFormat::Format_TYPE_COLOR_RGBA){
					tbyte *data=mData+mVertexFormat->getElementOffset(i);
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

	if(mHasTranspose){
		int i;
		for(i=0;i<mVariableFormat->getSize();++i){
			int offset=mVariableFormat->getVariable(i)->getOffset();
			if(offset>=start && offset<start+size){
				BackableBuffer::transposeVariable(mVariableFormat,mData,i);
			}
		}
	}

	if(mHandle>0){
		glBindBuffer(mTarget,mHandle);
		glBufferSubData(mTarget,start,size,mData+start);
		glBindBuffer(mTarget,0);
	}

	TOADLET_CHECK_GLERROR("GLBuffer::update");

	return true;
}

#if defined(TOADLET_HAS_GLSHADERS)
bool GLBuffer::activateUniforms(){
	math::Matrix4x4 matrix,textureMatrix;

	int i;
	for(i=0;i<mVariableFormat->getSize();++i){
		VariableBufferFormat::Variable *variable=mVariableFormat->getVariable(i);
		int format=variable->getFormat();
		int index=variable->getIndex();
		int offset=variable->getOffset();
		bool transpose=(format&VariableBufferFormat::Format_BIT_TRANSPOSE)!=0;
		tbyte *data=mData+offset;
		int samplerMatrix=((format&VariableBufferFormat::Format_MASK_SAMPLER_MATRIX)>>VariableBufferFormat::Format_SHIFT_SAMPLER_MATRIX)-1;
		if(samplerMatrix>=0){
			GLTexture *texture=mDevice->mLastTextures.size()<=samplerMatrix?NULL:mDevice->mLastTextures[samplerMatrix];
			matrix.set((float*)data);
			if(texture!=NULL){
				MathConversion::scalarToFloat(textureMatrix,texture->getMatrix());
				math::Math::postMul(matrix,textureMatrix);
			}
			data=(tbyte*)matrix.data;
		}
		format&=~VariableBufferFormat::Format_MASK_OPTIONS;

		switch(format){
			case VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_1:
				glUniform1fv(index,1,(float*)data);
			break;
			case VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_2:
				glUniform2fv(index,1,(float*)data);
			break;
			case VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_3:
				glUniform3fv(index,1,(float*)data);
			break;
			case VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4:
				glUniform4fv(index,1,(float*)data);
			break;

			case VariableBufferFormat::Format_TYPE_INT_32|VariableBufferFormat::Format_COUNT_1:
				glUniform1iv(index,1,(int*)data);
			break;
			case VariableBufferFormat::Format_TYPE_INT_32|VariableBufferFormat::Format_COUNT_2:
				glUniform2iv(index,1,(int*)data);
			break;
			case VariableBufferFormat::Format_TYPE_INT_32|VariableBufferFormat::Format_COUNT_3:
				glUniform3iv(index,1,(int*)data);
			break;
			case VariableBufferFormat::Format_TYPE_INT_32|VariableBufferFormat::Format_COUNT_4:
				glUniform4iv(index,1,(int*)data);
			break;

			case VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_2X2:
				glUniformMatrix2fv(index,1,transpose,(float*)data);
			break;
			case VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_3X3:
				glUniformMatrix3fv(index,1,transpose,(float*)data);
			break;
			case VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4X4:
				glUniformMatrix4fv(index,1,transpose,(float*)data);
			break;
			#if !defined(TOADLET_HAS_GLES)
				case VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_2X3:
					glUniformMatrix2x3fv(index,1,transpose,(float*)data);
				break;
				case VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_3X2:
					glUniformMatrix3x2fv(index,1,transpose,(float*)data);
				break;
				case VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_2X4:
					glUniformMatrix2x4fv(index,1,transpose,(float*)data);
				break;
				case VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4X2:
					glUniformMatrix4x2fv(index,1,transpose,(float*)data);
				break;
				case VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_3X4:
					glUniformMatrix3x4fv(index,1,transpose,(float*)data);
				break;
				case VariableBufferFormat::Format_TYPE_FLOAT_32|VariableBufferFormat::Format_COUNT_4X3:
					glUniformMatrix4x3fv(index,1,transpose,(float*)data);
				break;
			#endif

			case VariableBufferFormat::Format_TYPE_RESOURCE:
				glUniform1i(index,variable->getResourceIndex());
			break;
		}
	}

	TOADLET_CHECK_GLERROR("GLBuffer::activateUniforms");

	return true;
}
#endif

GLenum GLBuffer::getBufferUsage(int usage,int access){
	GLenum glusage=0;
	switch(usage){
		case Usage_BIT_STATIC:
			#if defined(TOADLET_HAS_GLES)
				glusage=GL_STATIC_DRAW;
			#else
				if((access&Access_BIT_WRITE)!=0){
					glusage=GL_STATIC_DRAW;
				}
				else if((access&Access_BIT_READ)!=0){
					glusage=GL_STATIC_READ;
				}
				else{
					glusage=GL_STATIC_COPY;
				}
			#endif
		break;
		case Usage_BIT_STREAM:
			#if defined(TOADLET_HAS_GLES)
				glusage=GL_DYNAMIC_DRAW;
			#else
				if((access&Access_BIT_WRITE)!=0){
					glusage=GL_STREAM_DRAW;
				}
				else if((access&Access_BIT_READ)!=0){
					glusage=GL_STREAM_READ;
				}
				else{
					glusage=GL_STREAM_COPY;
				}
			#endif
		break;
		case Usage_BIT_DYNAMIC:
		case Usage_BIT_STAGING:
			#if defined(TOADLET_HAS_GLES)
				glusage=GL_DYNAMIC_DRAW;
			#else
				if((access&Access_BIT_WRITE)!=0){
					glusage=GL_DYNAMIC_DRAW;
				}
				else if((access&Access_BIT_READ)!=0){
					glusage=GL_DYNAMIC_READ;
				}
				else{
					glusage=GL_DYNAMIC_COPY;
				}
			#endif
		break;
	}
	return glusage;
}

}
}
