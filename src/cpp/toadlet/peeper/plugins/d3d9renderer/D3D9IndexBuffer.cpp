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

#include "D3D9IndexBuffer.h"
#include "D3D9Renderer.h"
#include <toadlet/peeper/CapabilitySet.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

D3D9IndexBuffer::D3D9IndexBuffer(D3D9Renderer *renderer):
	mRenderer(NULL),

	mUsageFlags(0),
	mAccessType(AccessType_NO_ACCESS),
	mSize(0),
	mIndexFormat(IndexFormat_UINT_8),
	mDataSize(0),

	mD3DFormat(D3DFMT_INDEX16),
	mD3DUsage(0),
	mD3DPool(D3DPOOL_MANAGED),
	mIndexBuffer(NULL),
	mLockType(AccessType_NO_ACCESS),
	mData(NULL),
	mBacking(false),
	mBackingData(NULL)
{
	mRenderer=renderer;
}

D3D9IndexBuffer::~D3D9IndexBuffer(){
	destroy();
}

bool D3D9IndexBuffer::create(int usageFlags,AccessType accessType,IndexFormat indexFormat,int size){
	destroy();

	mUsageFlags=usageFlags;
	mAccessType=accessType;
	mSize=size;
	mIndexFormat=indexFormat;
	mDataSize=mIndexFormat*mSize;
	if(mIndexFormat==IndexFormat_UINT_8) mDataSize*=2;
	mD3DFormat=getD3DFORMAT(mIndexFormat);

	createContext();

	return true;
}

void D3D9IndexBuffer::destroy(){
	destroyContext(false);

	if(mBackingData!=NULL){
		delete[] mBackingData;
		mBackingData=NULL;
	}
}

bool D3D9IndexBuffer::createContext(){
	mD3DUsage=0;
	mD3DPool=D3DPOOL_MANAGED;
	if((mUsageFlags&UsageFlags_DYNAMIC)>0){
		mD3DUsage|=D3DUSAGE_DYNAMIC;
		#if !defined(TOADLET_HAS_DIRECT3DMOBILE)
			mD3DPool=D3DPOOL_DEFAULT;
		#endif
	}

	if(mAccessType==AccessType_WRITE_ONLY){
		mD3DUsage|=D3DUSAGE_WRITEONLY;
	}

	Logger::log(Categories::TOADLET_PEEPER,Logger::Level_EXCESSIVE,
		String("Allocating D3D9IndexBuffer of size:")+mDataSize);

	HRESULT result=mRenderer->getDirect3DDevice9()->CreateIndexBuffer(mDataSize,mD3DUsage,mD3DFormat,mD3DPool,&mIndexBuffer TOADLET_SHAREDHANDLE);
	TOADLET_CHECK_D3D9ERROR(result,"D3D9VertexBuffer: CreateVertexBuffer");

	if(mBacking){
		uint8 *data=lock(AccessType_WRITE_ONLY);
		memcpy(data,mBackingData,mDataSize);
		unlock();

		delete[] mBackingData;
		mBackingData=NULL;
		mBacking=true;
	}

	return SUCCEEDED(result);
}

void D3D9IndexBuffer::destroyContext(bool backData){
	if(backData){
		mBackingData=new uint8[mDataSize];
		mBacking=true;

		uint8 *data=lock(AccessType_READ_ONLY);
		memcpy(mBackingData,data,mDataSize);
		unlock();
	}

	if(mIndexBuffer!=NULL){
		mIndexBuffer->Release();
		mIndexBuffer=NULL;
	}
}

bool D3D9IndexBuffer::contextNeedsReset(){
	#if defined(TOADLET_HAS_DIRECT3DMOBILE)
		return false;
	#else
		return mD3DPool==D3DPOOL_DEFAULT;
	#endif
}

uint8 *D3D9IndexBuffer::lock(AccessType lockType){
	mLockType=lockType;

	DWORD d3dflags=0;
	switch(mLockType){
		case AccessType_WRITE_ONLY:
			if((mUsageFlags&UsageFlags_DYNAMIC)>0){
				d3dflags|=D3DLOCK_DISCARD;
			}
		break;
		case AccessType_READ_WRITE:
			d3dflags|=D3DLOCK_READONLY;
		break;
	}

	HRESULT result=mIndexBuffer->Lock(0,0,(void**)&mData,d3dflags);
	TOADLET_CHECK_D3D9ERROR(result,"D3D9IndexBuffer: Lock");

	if(mData!=NULL && mLockType!=AccessType_WRITE_ONLY && mIndexFormat==IndexFormat_UINT_8){
		// Pack the indexes
		uint16 *data16=(uint16*)mData;
		int i;
		for(i=0;i<mSize;++i){
			mData[i]=data16[i];
		}
	}

	return mData;
}

bool D3D9IndexBuffer::unlock(){
	if(mData!=NULL && mLockType!=AccessType_READ_ONLY && mIndexFormat==IndexFormat_UINT_8){
		// Unpack the indexes
		uint16 *data16=(uint16*)mData;
		int i;
		for(i=mSize-1;i>=0;--i){
			data16[i]=mData[i];
		}
	}

	HRESULT result=mIndexBuffer->Unlock();
	TOADLET_CHECK_D3D9ERROR(result,"D3D9IndexBuffer: Unlock");

	mData=NULL;

	return SUCCEEDED(result);
}

D3DFORMAT D3D9IndexBuffer::getD3DFORMAT(IndexFormat format){
	D3DFORMAT d3dFormat=(D3DFORMAT)0;
	switch(format){
		case IndexFormat_UINT_8:
		case IndexFormat_UINT_16:
			d3dFormat=D3DFMT_INDEX16;
		break;
		case IndexFormat_UINT_32:
			d3dFormat=D3DFMT_INDEX32;
		break;
		default:
			Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ERROR,
				"D3D9IndexBuffer::getD3DFORMAT: Invalid format");
			return (D3DFORMAT)0;
		break;
	}

	return d3dFormat;
}

}
}
