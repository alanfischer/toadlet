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

#include "D3D9WindowRenderTarget.h"
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

#if defined(TOADLET_SET_D3DM)
	TOADLET_C_API RenderTarget *new_D3DMWindowRenderTarget(HWND wnd,WindowRenderTargetFormat *format){
		return new D3D9WindowRenderTarget(wnd,format);
	}
#else
	TOADLET_C_API RenderTarget *new_D3D9WindowRenderTarget(HWND wnd,WindowRenderTargetFormat *format){
		return new D3D9WindowRenderTarget(wnd,format);
	}
#endif

D3D9WindowRenderTarget::D3D9WindowRenderTarget():D3D9RenderTarget(),
	mLibrary(0),
	mD3D(NULL),
	mD3DDevice(NULL),
	mColorSurface(NULL),
	mDepthSurface(NULL),
	mWindow(0),
	mWidth(0),
	mHeight(0)
{
}

D3D9WindowRenderTarget::D3D9WindowRenderTarget(HWND wnd,WindowRenderTargetFormat *format):D3D9RenderTarget(),
	mLibrary(0),
	mD3D(NULL),
	mD3DDevice(NULL),
	mColorSurface(NULL),
	mDepthSurface(NULL),
	mWindow(0),
	mWidth(0),
	mHeight(0)
{
	createContext(wnd,format);
}

D3D9WindowRenderTarget::~D3D9WindowRenderTarget(){
	destroyContext();
}

bool D3D9WindowRenderTarget::activate(){
	IDirect3DDevice9 *device=mD3DDevice;

	#if defined(TOADLET_SET_D3DM)
		HRESULT result=device->SetRenderTarget(mColorSurface,mDepthSurface);
		TOADLET_CHECK_D3D9ERROR(result,"SetRenderTarget");
	#else
		HRESULT result=device->SetRenderTarget(0,mColorSurface);
		TOADLET_CHECK_D3D9ERROR(result,"SetRenderTarget");

		result=device->SetDepthStencilSurface(mDepthSurface);
		TOADLET_CHECK_D3D9ERROR(result,"SetDepthStencilSurface");
	#endif

	return true;
}

void D3D9WindowRenderTarget::reset(){
	if(mColorSurface!=NULL){
		mColorSurface->Release();
		mColorSurface=NULL;
	}

	if(mDepthSurface!=NULL){
		mDepthSurface->Release();
		mDepthSurface=NULL;
	}

	fillPresentParameters(mPresentParameters);

	HRESULT result=mD3DDevice->Reset(&mPresentParameters);
	TOADLET_CHECK_D3D9ERROR(result,"Reset");

	#if defined(TOADLET_SET_D3DM)
		mD3DDevice->GetRenderTarget(&mColorSurface);
	#else
		mD3DDevice->GetRenderTarget(0,&mColorSurface);
	#endif
	mD3DDevice->GetDepthStencilSurface(&mDepthSurface);
}

bool D3D9WindowRenderTarget::createContext(HWND wnd,WindowRenderTargetFormat *format){
	HRESULT result;

	mLibrary=LoadLibrary(format->debug?TOADLET_D3D9_DEBUG_DLL_NAME:TOADLET_D3D9_DLL_NAME);

	if(mLibrary==0){
		Error::libraryNotFound(Categories::TOADLET_PEEPER,
			String("D3D9RenderWindow: Error loading ")+TOADLET_D3D9_DLL_NAME);
		return false;
	}

	void *symbol=GetProcAddress(mLibrary,TOADLET_D3D9_CREATE_NAME);
	if(symbol==NULL){
		Error::symbolNotFound(Categories::TOADLET_PEEPER,
			String("D3D9RenderWindow: Error finding ")+TOADLET_D3D9_CREATE_NAME);
		return NULL;
	}

	typedef IDirect3D9*(WINAPI *Direct3DCreate9)(UINT);
	mD3D=((Direct3DCreate9)symbol)(D3D_SDK_VERSION);
	if(mD3D==NULL){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D9RenderWindow: Error creating Direct3D object");
		return false;
	}

	mVisual=format->visual;

	mAdaptor=D3DADAPTER_DEFAULT;
	D3DADAPTER_IDENTIFIER9 identifier={0};
	mD3D->GetAdapterIdentifier(mAdaptor,0,&identifier);
	Logger::alert(Categories::TOADLET_PEEPER,
		String("D3D Driver:") + identifier.Driver);
	Logger::alert(Categories::TOADLET_PEEPER,
		String("D3D Description:") + identifier.Description);

	#if defined(TOADLET_SET_D3DM)
		mDevType=D3DMDEVTYPE_DEFAULT;
	#else
		mDevType=D3DDEVTYPE_HAL;
	#endif
	
	int flags=format->flags;

	#if !defined(TOADLET_SET_D3DM)
		result=mD3D->CheckDeviceType(mAdaptor,mDevType,D3DFMT_X8R8G8B8,D3DFMT_X8R8G8B8,FALSE);
		if(FAILED(result)){
			Error::unknown(Categories::TOADLET_PEEPER,
				"D3D9RenderWindow: Error creating 8,8,8 bit back buffer");
			return false;
		}

		result=mD3D->CheckDeviceFormat(mAdaptor,mDevType,D3DFMT_X8R8G8B8,D3DUSAGE_DEPTHSTENCIL,D3DRTYPE_SURFACE,D3DFMT_D24S8);
		if(FAILED(result)){
			Error::unknown(Categories::TOADLET_PEEPER,
				"D3D9RenderWindow: Error creating 16 bit depth, 8 bit stencil back buffer");
			return false;
		}

		D3DCAPS9 caps;
		ZeroMemory(&caps,sizeof(D3DCAPS9));
		result=mD3D->GetDeviceCaps(mAdaptor,mDevType,&caps);
		if(FAILED(result)){
			Error::unknown(Categories::TOADLET_PEEPER,
				"D3D9RenderWindow: Error getting device caps");
			return false;
		}

		if((flags&(D3DCREATE_HARDWARE_VERTEXPROCESSING|D3DCREATE_SOFTWARE_VERTEXPROCESSING))==0){
			if(caps.VertexProcessingCaps!=0){
				flags|=D3DCREATE_HARDWARE_VERTEXPROCESSING;
			}
			else{
				flags|=D3DCREATE_SOFTWARE_VERTEXPROCESSING;
			}
		}
	#endif

	if(format->threads>1){
		flags|=D3DCREATE_MULTITHREADED;
	}

	mWindow=wnd;

	fillPresentParameters(mPresentParameters);

	result=mD3D->CreateDevice(mAdaptor,mDevType,wnd,flags,&mPresentParameters,&mD3DDevice);
	if(FAILED(result)){
		TOADLET_CHECK_D3D9ERROR(result,"CreateDevice");
		return false;
	}

	#if defined(TOADLET_SET_D3DM)
		mD3DDevice->GetRenderTarget(&mColorSurface);
	#else
		mD3DDevice->GetRenderTarget(0,&mColorSurface);
	#endif
	mD3DDevice->GetDepthStencilSurface(&mDepthSurface);

	return true;
}

bool D3D9WindowRenderTarget::destroyContext(){
	if(mColorSurface!=NULL){
		mColorSurface->Release();
		mColorSurface=NULL;
	}

	if(mDepthSurface!=NULL){
		mDepthSurface->Release();
		mDepthSurface=NULL;
	}

	if(mD3DDevice!=NULL){
		mD3DDevice->Release();
		mD3DDevice=NULL;
	}

	if(mD3D!=NULL){
		mD3D->Release();
		mD3D=NULL;
	}

	if(mLibrary!=0){
		FreeLibrary(mLibrary);
		mLibrary=0;
	}

	return true;
}

void D3D9WindowRenderTarget::fillPresentParameters(D3DPRESENT_PARAMETERS &presentParameters){
	RECT rect={0};
	GetClientRect(mWindow,&rect);
	mWidth=rect.right-rect.left;
	mHeight=rect.bottom-rect.top;

	memset(&presentParameters,0,sizeof(presentParameters));
	#if defined(TOADLET_SET_D3DM)
		presentParameters.AutoDepthStencilFormat=D3DMFMT_D16;
		presentParameters.EnableAutoDepthStencil=TRUE;
		presentParameters.Windowed			=TRUE;
		presentParameters.SwapEffect		=D3DMSWAPEFFECT_DISCARD;
		presentParameters.BackBufferFormat	=D3DMFMT_UNKNOWN;
	#else
		presentParameters.AutoDepthStencilFormat=D3DFMT_D24S8;
		presentParameters.EnableAutoDepthStencil=TRUE;
		presentParameters.Windowed			=TRUE;
		presentParameters.SwapEffect		=D3DSWAPEFFECT_DISCARD;
		presentParameters.BackBufferWidth	=mWidth;
		presentParameters.BackBufferHeight	=mHeight;
		presentParameters.BackBufferFormat	=D3DFMT_X8R8G8B8;
	#endif

	#if !defined(TOADLET_SET_D3DM)
		if(mVisual.vsync){
			mPresentParameters.PresentationInterval=D3DPRESENT_INTERVAL_DEFAULT;
		}
		else{
			mPresentParameters.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;
		}

		if(mVisual.multisamples>0){
			int samples=mVisual.multisamples;
			HRESULT result;
			do{
				result=mD3D->CheckDeviceMultiSampleType(mAdaptor,mDevType,mPresentParameters.BackBufferFormat,mPresentParameters.Windowed,(D3DMULTISAMPLE_TYPE)samples,NULL);
			}while(FAILED(result) && --samples>0);
			mPresentParameters.MultiSampleType=(D3DMULTISAMPLE_TYPE)samples;
		}
	#endif

}

}
}
