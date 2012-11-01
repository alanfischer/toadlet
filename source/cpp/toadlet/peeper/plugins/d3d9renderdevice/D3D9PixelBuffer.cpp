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

#include "D3D9PixelBuffer.h"
#include "D3D9RenderDevice.h"
#include <toadlet/egg/Error.h>

namespace toadlet{
namespace peeper{

D3D9PixelBuffer::D3D9PixelBuffer(D3D9RenderDevice *renderDevice,bool renderTarget):
	mDevice(NULL),

	mRenderTarget(false),
	mSurface(NULL),
	mVolume(NULL),
	mUsage(0),
	mAccess(0),
	//mFormat,
	mDataSize(0)
{
	mDevice=renderDevice;
	mRenderTarget=renderTarget;
}

bool D3D9PixelBuffer::create(int usage,int access,TextureFormat::ptr format){
	mUsage=usage;
	mAccess=access;
	mFormat=TextureFormat::ptr(new TextureFormat(format));
	mFormat->setPixelFormat(mDevice->getClosePixelFormat(format->getPixelFormat()));
	mDataSize=mFormat->getDataSize();

	createContext(false);

	return true;
}

void D3D9PixelBuffer::destroy(){
	destroyContext(false);

	BaseResource::destroy();
}

void D3D9PixelBuffer::resetCreate(){
	if(needsReset()){
		createContext(true);
	}
}

void D3D9PixelBuffer::resetDestroy(){
	if(needsReset()){
		destroyContext(true);
	}
}

bool D3D9PixelBuffer::createContext(bool restore){
	if(mFormat->getDepth()!=1){
		Error::invalidParameters(Categories::TOADLET_PEEPER,"D3D9PixelBuffer may only be created with a depth of 1");
		return false;
	}

	int width=mFormat->getWidth(),height=mFormat->getHeight();
	D3DFORMAT d3dformat=D3D9RenderDevice::getD3DFORMAT(mFormat->getPixelFormat());
	HRESULT result=S_OK;
	IDirect3DSurface9 *d3dsurface=NULL;
	if(mRenderTarget){
		mD3DPool=D3DPOOL_DEFAULT;

		if((mFormat->getPixelFormat()&TextureFormat::Format_MASK_SEMANTICS)==TextureFormat::Format_SEMANTIC_DEPTH){
			#if defined(TOADLET_SET_D3DM)
				result=mDevice->getDirect3DDevice9()->CreateDepthStencilSurface(width,height,d3dformat,D3DMULTISAMPLE_NONE,&d3dsurface TOADLET_SHAREDHANDLE);
			#else
				result=mDevice->getDirect3DDevice9()->CreateDepthStencilSurface(width,height,d3dformat,D3DMULTISAMPLE_NONE,NULL,FALSE,&d3dsurface TOADLET_SHAREDHANDLE);
			#endif
			if(FAILED(result)){
				TOADLET_CHECK_D3D9ERROR(result,"CreateDepthStencilSurface");
				return false;
			}
		}
		else{
			#if defined(TOADLET_SET_D3DM)
				result=mDevice->getDirect3DDevice9()->CreateRenderTarget(width,height,d3dformat,D3DMULTISAMPLE_NONE,FALSE,&d3dsurface TOADLET_SHAREDHANDLE);
			#else
				result=mDevice->getDirect3DDevice9()->CreateRenderTarget(width,height,d3dformat,D3DMULTISAMPLE_NONE,NULL,FALSE,&d3dsurface TOADLET_SHAREDHANDLE);
			#endif
			if(FAILED(result)){
				TOADLET_CHECK_D3D9ERROR(result,"CreateRenderTarget");
				return false;
			}
		}
	}
	else{
		mD3DPool=D3DPOOL_SYSTEMMEM;

		#if !defined(TOADLET_SET_D3DM)
			result=mDevice->getDirect3DDevice9()->CreateOffscreenPlainSurface(width,height,d3dformat,mD3DPool,&d3dsurface TOADLET_SHAREDHANDLE);
			if(FAILED(result)){
				TOADLET_CHECK_D3D9ERROR(result,"CreateDepthStencilSurface");
				return false;
			}
		#else
			Error::unimplemented(Categories::TOADLET_PEEPER,"can not create an offscreen buffer");
			return false;
		#endif
	}

	mSurface=d3dsurface;

	return true;
}

bool D3D9PixelBuffer::destroyContext(bool backup){
	if(mSurface!=NULL){
		mSurface->Release();
		mSurface=NULL;
	}
	else if(mVolume!=NULL){
		mVolume->Release();
		mVolume=NULL;
	}

	return true;
}

uint8 *D3D9PixelBuffer::lock(int lockAccess){
	DWORD d3dflags=0;
	if(lockAccess==Access_BIT_READ){
		d3dflags|=D3DLOCK_READONLY;
	}

	HRESULT result=S_OK;
	uint8 *data=NULL;
	if(mSurface!=NULL){
		D3DLOCKED_RECT d3drect={0};
		result=mSurface->LockRect(&d3drect,NULL,d3dflags);
		TOADLET_CHECK_D3D9ERROR(result,"D3D9PixelBuffer: LockRect");
		data=(uint8*)d3drect.pBits;
	}
	else if(mVolume!=NULL){
		D3DLOCKED_BOX d3dbox={0};
		result=mVolume->LockBox(&d3dbox,NULL,d3dflags);
		TOADLET_CHECK_D3D9ERROR(result,"D3D9PixelBuffer: LockBox");
		data=(uint8*)d3dbox.pBits;
	}
	return data;
}

bool D3D9PixelBuffer::unlock(){
	HRESULT result=S_OK;
	if(mSurface!=NULL){
		result=mSurface->UnlockRect();
		TOADLET_CHECK_D3D9ERROR(result,"D3D9PixelBuffer: UnlockRect");
	}
	else if(mVolume!=NULL){
		result=mVolume->UnlockBox();
		TOADLET_CHECK_D3D9ERROR(result,"D3D9PixelBuffer: UnlockBox");
	}
	return SUCCEEDED(result);
}

bool D3D9PixelBuffer::needsReset(){
	#if defined(TOADLET_SET_D3DM)
		return false;
	#else
		return mD3DPool==D3DPOOL_DEFAULT;
	#endif
}

}
}
