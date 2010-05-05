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
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

D3D9IndexBuffer::D3D9IndexBuffer(D3D9Renderer *renderer):
	mRenderer(NULL),

	mListener(NULL),
	mUsage(0),
	mAccess(0),
	mSize(0),
	mIndexFormat(IndexFormat_UINT_8),
	mDataSize(0),

	mD3DFormat(D3DFMT_INDEX16),
	mD3DUsage(0),
	mD3DPool(D3DPOOL_MANAGED),
	mIndexBuffer(NULL),
	mLockAccess(0),
	mData(NULL),
	mBackingData(NULL)
{
	mRenderer=renderer;
}

D3D9IndexBuffer::~D3D9IndexBuffer(){
	destroy();
}

bool D3D9IndexBuffer::create(int usage,int access,IndexFormat indexFormat,int size){
	destroy();

	mUsage=usage;
	mAccess=access;
	mSize=size;
	mIndexFormat=indexFormat;
	mDataSize=mIndexFormat*mSize;
	if(mIndexFormat==IndexFormat_UINT_8) mDataSize*=2;
	mD3DFormat=getD3DFORMAT(mIndexFormat);

	createContext(false);

	return true;
}

void D3D9IndexBuffer::destroy(){
	destroyContext(false);

	if(mBackingData!=NULL){
		delete[] mBackingData;
		mBackingData=NULL;
	}

	if(mListener!=NULL){
		mListener->bufferDestroyed(this);
	}
}

void D3D9IndexBuffer::resetCreate(){
	if(needsReset()){
		createContext(true);
	}
}

void D3D9IndexBuffer::resetDestroy(){
	if(needsReset()){
		destroyContext(true);
	}
}

bool D3D9IndexBuffer::createContext(bool restore){
	mD3DUsage=0;
	mD3DPool=D3DPOOL_MANAGED;
	if((mUsage&Usage_BIT_DYNAMIC)>0){
		mD3DUsage|=D3DUSAGE_DYNAMIC;
		#if !defined(TOADLET_SET_D3DM)
			mD3DPool=D3DPOOL_DEFAULT;
		#endif
	}

	if(mAccess==Access_BIT_WRITE){
		mD3DUsage|=D3DUSAGE_WRITEONLY;
	}

	HRESULT result=mRenderer->getDirect3DDevice9()->CreateIndexBuffer(mDataSize,mD3DUsage,mD3DFormat,mD3DPool,&mIndexBuffer TOADLET_SHAREDHANDLE);
	TOADLET_CHECK_D3D9ERROR(result,"D3D9VertexBuffer: CreateVertexBuffer");

	if(restore){
		byte *data=lock(Access_BIT_WRITE);
		memcpy(data,mBackingData,mDataSize);
		unlock();

		delete[] mBackingData;
		mBackingData=NULL;
	}
	
	return SUCCEEDED(result);
}

bool D3D9IndexBuffer::destroyContext(bool backup){
	if(backup){
		mBackingData=new uint8[mDataSize];

		TOADLET_TRY
			byte *data=lock(Access_BIT_READ);
			if(data!=NULL){
				memcpy(mBackingData,data,mDataSize);
				unlock();
			}
		TOADLET_CATCH(const Exception &){}
	}

	HRESULT result=S_OK;
	if(mIndexBuffer!=NULL){
		result=mIndexBuffer->Release();
		mIndexBuffer=NULL;
	}

	return SUCCEEDED(result);
}

uint8 *D3D9IndexBuffer::lock(int lockAccess){
	if(mIndexBuffer==NULL){
		return NULL;
	}

	if(mAccess==0 || (mAccess==Access_BIT_READ && lockAccess==Access_BIT_WRITE) || (mAccess==Access_BIT_WRITE && lockAccess==Access_BIT_READ)){
		Logger::error(Categories::TOADLET_PEEPER,"invalid lock type on buffer");
		return NULL;
	}

	mLockAccess=lockAccess;

	DWORD d3dflags=0;
	switch(mLockAccess){
		case Access_BIT_WRITE:
			if((mUsage&Usage_BIT_DYNAMIC)>0){
				d3dflags|=D3DLOCK_DISCARD;
			}
		break;
		case Access_BIT_READ:
			d3dflags|=D3DLOCK_READONLY;
		break;
	}

	HRESULT result=mIndexBuffer->Lock(0,0,(void**)&mData,d3dflags);
	TOADLET_CHECK_D3D9ERROR(result,"D3D9IndexBuffer: Lock");

	// We do this even if its write only, since the unlocking will write it back, it would get messed up if we didn't swap in all situations
	if(mData!=NULL && mIndexFormat==IndexFormat_UINT_8){
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
	if(mIndexBuffer==NULL){
		return false;
	}

	// We do this even if its read only, since we have to do it in all situations for locking
	if(mData!=NULL && mIndexFormat==IndexFormat_UINT_8){
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

bool D3D9IndexBuffer::needsReset(){
	#if defined(TOADLET_SET_D3DM)
		return false;
	#else
		return mD3DPool==D3DPOOL_DEFAULT;
	#endif
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
			Logger::error(Categories::TOADLET_PEEPER,
				"D3D9IndexBuffer::getD3DFORMAT: Invalid format");
			return (D3DFORMAT)0;
		break;
	}

	return d3dFormat;
}

}
}
