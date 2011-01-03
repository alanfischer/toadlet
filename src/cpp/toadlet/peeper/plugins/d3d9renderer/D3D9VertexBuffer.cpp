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

#include "D3D9VertexBuffer.h"
#include "D3D9VertexFormat.h"
#include "D3D9Renderer.h"
#include <toadlet/peeper/CapabilitySet.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

D3D9VertexBuffer::D3D9VertexBuffer(D3D9Renderer *renderer):
	mRenderer(NULL),

	mListener(NULL),
	mUsage(0),
	mAccess(0),
	mSize(0),
	//mVertexFormat,
	mVertexSize(0),
	mDataSize(0),

	mD3DUsage(0),
	mD3DPool(D3DPOOL_MANAGED),
	mVertexBuffer(NULL),
	mLockAccess(0),
	mData(NULL),
	mBackingData(NULL)
{
	mRenderer=renderer;
}

D3D9VertexBuffer::~D3D9VertexBuffer(){
	destroy();
}

bool D3D9VertexBuffer::create(int usage,int access,VertexFormat::ptr vertexFormat,int size){
	mUsage=usage;
	mAccess=access;
	mSize=size;
	mVertexFormat=vertexFormat;
	mVertexSize=mVertexFormat->getVertexSize();
	mDataSize=mVertexSize*mSize;

	D3D9VertexFormat *d3dvertexFormat=(D3D9VertexFormat*)mVertexFormat->getRootVertexFormat();
	if(d3dvertexFormat->getContextCreated()==false){
		d3dvertexFormat->createContext();
	}

	createContext(false);
	
	return true;
}

void D3D9VertexBuffer::destroy(){
	destroyContext(false);

	if(mBackingData!=NULL){
		delete[] mBackingData;
		mBackingData=NULL;
	}

	if(mListener!=NULL){
		mListener->bufferDestroyed(this);
	}
}

void D3D9VertexBuffer::resetCreate(){
	if(needsReset()){
		createContext(true);
	}
}

void D3D9VertexBuffer::resetDestroy(){
	if(needsReset()){
		destroyContext(true);
	}
}

bool D3D9VertexBuffer::createContext(bool restore){
	mD3DUsage=0;
	if((mUsage&Usage_BIT_DYNAMIC)>0){
		mD3DUsage|=D3DUSAGE_DYNAMIC;
	}
	if(mAccess==Access_BIT_WRITE){
		mD3DUsage|=D3DUSAGE_WRITEONLY;
	}

	#if defined(TOADLET_SET_D3DM)
		mD3DPool=D3DPOOL_MANAGED;
	#else
		if((mUsage&Usage_BIT_STAGING)>0){
			mD3DPool=D3DPOOL_SYSTEMMEM;
		}
		else if((mUsage&Usage_BIT_DYNAMIC)>0){
			mD3DPool=D3DPOOL_DEFAULT;
		}
		else{
			mD3DPool=D3DPOOL_MANAGED;
		}
	#endif


	D3D9VertexFormat *d3dvertexFormat=(D3D9VertexFormat*)mVertexFormat->getRootVertexFormat();
	HRESULT result=mRenderer->getDirect3DDevice9()->CreateVertexBuffer(mDataSize,mD3DUsage,d3dvertexFormat->getFVF(),mD3DPool,&mVertexBuffer TOADLET_SHAREDHANDLE);
	TOADLET_CHECK_D3D9ERROR(result,"D3D9VertexBuffer: CreateVertexBuffer");

	if(restore && (mUsage&Usage_BIT_DYNAMIC)==0){
		byte *data=lock(Access_BIT_WRITE);
		memcpy(data,mBackingData,mDataSize);
		unlock();

		delete[] mBackingData;
		mBackingData=NULL;
	}

	return SUCCEEDED(result);
}

bool D3D9VertexBuffer::destroyContext(bool backData){
	if(backData && (mUsage&Usage_BIT_DYNAMIC)==0){
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
	if(mVertexBuffer!=NULL){
		result=mVertexBuffer->Release();
		mVertexBuffer=NULL;
	}

	return SUCCEEDED(result);
}

uint8 *D3D9VertexBuffer::lock(int lockAccess){
	if(mVertexBuffer==NULL){
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

	HRESULT result=mVertexBuffer->Lock(0,0,(void**)&mData,d3dflags);
	TOADLET_CHECK_D3D9ERROR(result,"D3D9VertexBuffer: Lock");

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
	}

	return mData;
}

bool D3D9VertexBuffer::unlock(){
	if(mVertexBuffer==NULL){
		return false;
	}

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
	}

	HRESULT result=mVertexBuffer->Unlock();
	TOADLET_CHECK_D3D9ERROR(result,"D3D9VertexBuffer: Unlock");

	mData=NULL;

	return SUCCEEDED(result);
}

bool D3D9VertexBuffer::needsReset(){
	#if defined(TOADLET_SET_D3DM)
		return false;
	#else
		return mD3DPool==D3DPOOL_DEFAULT;
	#endif
}

}
}
