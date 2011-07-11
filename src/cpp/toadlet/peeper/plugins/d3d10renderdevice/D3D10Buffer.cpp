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

#include "D3D10Buffer.h"
#include "D3D10RenderDevice.h"
#include <toadlet/egg/Logger.h>
#include <toadlet/peeper/BackableBuffer.h>

namespace toadlet{
namespace peeper{

D3D10Buffer::D3D10Buffer(D3D10RenderDevice *renderDevice):
	mDevice(NULL),

	mListener(NULL),
	mUsage(0),
	mAccess(0),
	mDataSize(0),
	mSize(0),
	mHasTranspose(false),

	mIndexFormat((IndexFormat)0),
	//mVertexFormat,
	//mVariableBufferFormat,

	mBindFlags(0),
	mBuffer(NULL),
	//mColorElements,
	mLockAccess(0),
	mMapping(false),
	mBacking(false),
	mData(NULL)
{
	mDevice=renderDevice;
}

D3D10Buffer::~D3D10Buffer(){
	destroy();
}

bool D3D10Buffer::create(int usage,int access,IndexFormat indexFormat,int size){
	if((usage&Usage_BIT_STATIC)>0 && (access&Access_BIT_READ)>0){
		Error::invalidParameters(Categories::TOADLET_PEEPER,
			"Buffer can not be static and readable");
		return false;
	}

	mUsage=usage;
	mAccess=access;
	mSize=size;
	mIndexFormat=indexFormat;
	mDataSize=((indexFormat==IndexFormat_UINT8)?2:mIndexFormat)*mSize;

	mBindFlags|=D3D10_BIND_INDEX_BUFFER;

	mMapping=(mUsage&(Usage_BIT_STATIC|Usage_BIT_STREAM))==0;
	if(mMapping){
		createContext();
	}
	else{
		mData=new uint8[mDataSize];
		mBacking=true;
	}

	return true;
}

bool D3D10Buffer::create(int usage,int access,VertexFormat::ptr vertexFormat,int size){
	if((usage&Usage_BIT_STATIC)>0 && (access&Access_BIT_READ)>0){
		Error::invalidParameters(Categories::TOADLET_PEEPER,
			"Buffer can not be static and readable");
		return false;
	}

	mUsage=usage;
	mAccess=access;
	mSize=size;
	mVertexFormat=vertexFormat;
	mDataSize=mVertexFormat->getVertexSize()*mSize;

	mBindFlags|=D3D10_BIND_VERTEX_BUFFER;

	mMapping=(mUsage&(Usage_BIT_STATIC|Usage_BIT_STREAM))==0;
	if(mMapping){
		createContext();
	}
	else{
		mData=new uint8[mDataSize];
		mBacking=true;
	}

	return true;
}

bool D3D10Buffer::create(int usage,int access,VariableBufferFormat::ptr variableFormat){
	if((usage&Usage_BIT_STATIC)>0 && (access&Access_BIT_READ)>0){
		Error::invalidParameters(Categories::TOADLET_PEEPER,
			"Buffer can not be static and readable");
		return false;
	}

	mUsage=usage;
	mAccess=access;
	mVariableFormat=variableFormat;
	mDataSize=variableFormat->getDataSize();

	mBindFlags|=D3D10_BIND_CONSTANT_BUFFER;

	mMapping=(mUsage&(Usage_BIT_STATIC|Usage_BIT_STREAM))==0;
	if(mMapping){
		createContext();

		mHasTranspose=false;
		int i;
		for(i=0;i<mVariableFormat->getSize();++i){
			mHasTranspose|=((mVariableFormat->getVariable(i)->getFormat()&VariableBufferFormat::Format_BIT_TRANSPOSE)!=0);
		}
	}
	else{
		mData=new uint8[mDataSize];
		mBacking=true;
	}

	return true;
}

void D3D10Buffer::destroy(){
	destroyContext();

	if(mData!=NULL){
		delete[] mData;
		mData=NULL;
	}
	
	if(mListener!=NULL){
		if(mIndexFormat!=(IndexFormat)0){
			mListener->bufferDestroyed((IndexBuffer*)this);
		}
		else if(mVertexFormat!=NULL){
			mListener->bufferDestroyed((VertexBuffer*)this);
		}
		else if(mVariableFormat!=NULL){
			mListener->bufferDestroyed((VariableBuffer*)this);
		}
		mListener=NULL;
	}
}

bool D3D10Buffer::createContext(){
	D3D10_BUFFER_DESC desc={0};

	desc.ByteWidth=mDataSize;

	if((mUsage&Usage_BIT_STAGING)==0){
		desc.BindFlags=mBindFlags;
	}

	desc.Usage=D3D10RenderDevice::getD3D10_USAGE(mUsage);

	if((mUsage&(Usage_BIT_DYNAMIC|Usage_BIT_STAGING))>0){
		if((mAccess&Access_BIT_READ)>0){
			desc.CPUAccessFlags|=D3D10_CPU_ACCESS_READ;
		}
		if((mAccess&Access_BIT_WRITE)>0){
			desc.CPUAccessFlags|=D3D10_CPU_ACCESS_WRITE;
		}
	}

	HRESULT result=S_OK;
	if(mMapping){
		result=mDevice->getD3D10Device()->CreateBuffer(&desc,NULL,&mBuffer);
	}
	else{
		D3D10_SUBRESOURCE_DATA data={0};
		data.pSysMem=mData;
		result=mDevice->getD3D10Device()->CreateBuffer(&desc,&data,&mBuffer);
	}
	TOADLET_CHECK_D3D10ERROR(result,"D3D10Buffer: CreateBuffer");

	return SUCCEEDED(result);
}

bool D3D10Buffer::destroyContext(){
	HRESULT result=S_OK;
	if(mBuffer!=NULL){
		result=mBuffer->Release();
		mBuffer=NULL;
	}

	return SUCCEEDED(result);
}

uint8 *D3D10Buffer::lock(int lockAccess){
	mLockAccess=lockAccess;

	if(mMapping){
		D3D10_MAP mapType=D3D10RenderDevice::getD3D10_MAP(mLockAccess,mUsage);

		UINT mapFlags=0;

		HRESULT result=mBuffer->Map(mapType,mapFlags,(void**)&mData);
		TOADLET_CHECK_D3D10ERROR(result,"D3D10Buffer: Lock");
	}
	else{
		if(mBuffer!=NULL && (mUsage&Usage_BIT_STATIC)>0){
			Error::unknown(Categories::TOADLET_PEEPER,
				"re-locking a static buffer");
			return NULL;
		}

		if(mData==NULL){
			mData=new uint8[mDataSize];
		}
	}

	// We do this even if its write only, since the unlocking will write it back, it would get messed up if we didn't swap in all situations
	if(mData!=NULL){
		int i,j;
		if(mVertexFormat!=NULL){
			int vertexSize=mVertexFormat->getVertexSize();
			for(i=0;i<mVertexFormat->getNumElements();++i){
				if(mVertexFormat->getFormat(i)==VertexFormat::Format_TYPE_COLOR_RGBA){
					tbyte *data=mData+mVertexFormat->getOffset(i);
					for(j=0;j<mSize;++j){
						uint32 &color=*(uint32*)(data+vertexSize*j);
						color=Math::argbToRGBA(color);
					}
				}
			}
		}
		if(mIndexFormat==IndexFormat_UINT8){
			// Pack the indexes
			uint16 *data16=(uint16*)mData;
			int i;
			for(i=0;i<mSize;++i){
				mData[i]=data16[i];
			}
		}
	}

	if(mVariableFormat!=NULL && mHasTranspose){
		BackableBuffer::unpackVariables(mVariableFormat,mData);
	}

	return mData;
}

bool D3D10Buffer::unlock(){
	if(mVariableFormat!=NULL && mHasTranspose){
		BackableBuffer::packVariables(mVariableFormat,mData);
	}

	// We do this even if its read only, since we have to do it in all situations for locking
	if(mData!=NULL){
		int i,j;
		if(mVertexFormat!=NULL){
			int vertexSize=mVertexFormat->getVertexSize();
			for(i=0;i<mVertexFormat->getNumElements();++i){
				if(mVertexFormat->getFormat(i)==VertexFormat::Format_TYPE_COLOR_RGBA){
					tbyte *data=mData+mVertexFormat->getOffset(i);
					for(j=0;j<mSize;++j){
						uint32 &color=*(uint32*)(data+vertexSize*j);
						color=Math::rgbaToARGB(color);
					}
				}
			}
		}
		if(mIndexFormat==IndexFormat_UINT8){
			// Unpack the indexes
			uint16 *data16=(uint16*)mData;
			int i;
			for(i=mSize-1;i>=0;--i){
				data16[i]=mData[i];
			}
		}
	}

	if(mMapping){
		mBuffer->Unmap();
		mData=NULL;
	}
	else{
		if(mBuffer==NULL){
			createContext();
		}
		else if((mLockAccess&Access_BIT_WRITE)>0){
			mDevice->getD3D10Device()->UpdateSubresource(mBuffer,0,NULL,mData,0,0);
		}

		// Only delete our data if we have no desire to read it
		if((mAccess&Access_BIT_READ)==0 && mData!=NULL){
			delete[] mData;
			mData=NULL;
		}
	}

	return true;
}

bool D3D10Buffer::update(tbyte *data,int start,int size){
	// UpdateSubresource does not allow partial buffer updates

	tbyte *bdata=lock(Access_BIT_WRITE);
	memcpy(bdata+start,data,size);
	return unlock();
}

}
}
