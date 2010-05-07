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
#include "D3D10Renderer.h"
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

D3D10Buffer::D3D10Buffer(D3D10Renderer *renderer):
	mRenderer(NULL),

	mListener(NULL),
	mUsage(0),
	mAccess(0),
	mSize(0),
	mDataSize(0),

	mIndexFormat(IndexBuffer::IndexFormat_UINT_8),
	//mVertexFormat,
	mVertexSize(0),

	mBindFlags(0),
	mBuffer(NULL),
	//mColorElements,
	mLockAccess(0),
	mMapping(false),
	mBacking(false),
	mData(NULL)
{
	mRenderer=renderer;
}

D3D10Buffer::~D3D10Buffer(){
	destroy();
}

bool D3D10Buffer::create(int usage,int access,IndexFormat indexFormat,int size){
	mUsage=usage;
	mAccess=access;
	mSize=size;
	mIndexFormat=indexFormat;
	mDataSize=((indexFormat==IndexFormat_UINT_8)?2:mIndexFormat)*mSize;

	mBindFlags|=D3D10_BIND_INDEX_BUFFER;

	mMapping=(mUsage&Buffer::Usage_BIT_STATIC)==0;
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
	destroy();

	mUsage=usage;
	mAccess=access;
	mSize=size;
	mVertexFormat=vertexFormat;
	mVertexSize=mVertexFormat->getVertexSize();
	mDataSize=mVertexSize*mSize;

	mBindFlags|=D3D10_BIND_VERTEX_BUFFER;

	mMapping=(mUsage&Buffer::Usage_BIT_STATIC)==0;
	if(mMapping){
		createContext();
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
		if((mBindFlags&D3D10_BIND_INDEX_BUFFER)>0){
			mListener->bufferDestroyed((IndexBuffer*)this);
		}
		else{
			mListener->bufferDestroyed((VertexBuffer*)this);
		}
	}
}

bool D3D10Buffer::createContext(){
	D3D10_BUFFER_DESC desc={0};

	desc.BindFlags=mBindFlags;
	desc.ByteWidth=mDataSize;

	desc.Usage=D3D10_USAGE_DEFAULT;
	if((mUsage&Usage_BIT_STATIC)>0){
		desc.Usage=D3D10_USAGE_IMMUTABLE;
	}
	else if((mUsage&Usage_BIT_STREAM)>0){
		desc.Usage=D3D10_USAGE_DYNAMIC;
	}

	if((mUsage&Usage_BIT_STATIC)==0){
		if((mAccess&Access_BIT_READ)>0){
			desc.CPUAccessFlags|=D3D10_CPU_ACCESS_READ;
		}
		if((mAccess&Access_BIT_WRITE)>0){
			desc.CPUAccessFlags|=D3D10_CPU_ACCESS_WRITE;
		}
	}

	HRESULT result=S_OK;
	if(mMapping){
		result=mRenderer->getD3D10Device()->CreateBuffer(&desc,NULL,&mBuffer);
	}
	else{
		D3D10_SUBRESOURCE_DATA data={0};
		data.pSysMem=mData;
		result=mRenderer->getD3D10Device()->CreateBuffer(&desc,&data,&mBuffer);
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
	mRenderer->mStatisticsSet.bufferLockCount++;

	mLockAccess=lockAccess;

	if(mMapping){
		D3D10_MAP mapType=(D3D10_MAP)0;
		switch(mLockAccess){
			case Access_BIT_READ:
				mapType=D3D10_MAP_READ;
			break;
			case Access_BIT_WRITE:
				mapType=D3D10_MAP_WRITE_DISCARD;
			break;
			case Access_READ_WRITE:
				mapType=D3D10_MAP_READ_WRITE;
			break;
		}

		UINT mapFlags=0;

		HRESULT result=mBuffer->Map(mapType,mapFlags,(void**)&mData);
		TOADLET_CHECK_D3D10ERROR(result,"D3D10Buffer: Lock");
	}
	else{
		if(mData==NULL){
			mData=new uint8[mDataSize];
		}
	}

	// We do this even if its write only, since the unlocking will write it back, it would get messed up if we didn't swap in all situations
	if(mData!=NULL){
		int i,j;
		if(mVertexFormat!=NULL){
			for(i=0;i<mVertexFormat->getNumElements();++i){
				if(mVertexFormat->getFormat(i)==VertexFormat::Format_COLOR_RGBA){
					byte *data=mData+mVertexFormat->getOffset(i);
					for(j=0;j<mSize;++j){
						uint32 &color=*(uint32*)(data+mVertexSize);
						color=(color&0xFF000000)|((color&0x000000FF)<<16)|(color&0x0000FF00)|((color&0x00FF0000)>>16);
					}
				}
			}
		}
		if(mIndexFormat==IndexFormat_UINT_8){
			// Pack the indexes
			uint16 *data16=(uint16*)mData;
			int i;
			for(i=0;i<mSize;++i){
				mData[i]=data16[i];
			}
		}
	}


	return mData;
}

bool D3D10Buffer::unlock(){
	// We do this even if its read only, since we have to do it in all situations for locking
	if(mData!=NULL){
		int i,j;
		if(mVertexFormat!=NULL){
			for(i=0;i<mVertexFormat->getNumElements();++i){
				if(mVertexFormat->getFormat(i)==VertexFormat::Format_COLOR_RGBA){
					byte *data=mData+mVertexFormat->getOffset(i);
					for(j=0;j<mSize;++j){
						uint32 &color=*(uint32*)(data+mVertexSize);
						color=(color&0xFF000000)|((color&0x000000FF)<<16)|(color&0x0000FF00)|((color&0x00FF0000)>>16);
					}
				}
			}
		}
		if(mIndexFormat==IndexFormat_UINT_8){
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
		else{
			mRenderer->getD3D10Device()->UpdateSubresource(mBuffer,0,NULL,mData,0,0);
		}

		if((mUsage&Usage_BIT_STATIC)>0){
			delete[] mData;
			mData=NULL;
		}
	}

	return true;
}

}
}
