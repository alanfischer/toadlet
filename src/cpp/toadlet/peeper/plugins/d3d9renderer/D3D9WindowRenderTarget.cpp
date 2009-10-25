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

TOADLET_C_API RenderTarget *new_D3D9WindowRenderTarget(HWND wnd,const Visual &visual){
	return new D3D9WindowRenderTarget(wnd,visual);
}

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

D3D9WindowRenderTarget::D3D9WindowRenderTarget(HWND wnd,const Visual &visual):D3D9RenderTarget(),
	mLibrary(0),
	mD3D(NULL),
	mD3DDevice(NULL),
	mColorSurface(NULL),
	mDepthSurface(NULL),
	mWidth(0),
	mHeight(0)
{
	createContext(wnd,visual);
}

D3D9WindowRenderTarget::~D3D9WindowRenderTarget(){
	destroyContext();
}

bool D3D9WindowRenderTarget::makeCurrent(IDirect3DDevice9 *device){
	#if defined(TOADLET_HAS_DIRECT3DMOBILE)
		HRESULT result=device->SetRenderTarget(mColorSurface,mDepthSurface);
		TOADLET_CHECK_D3D9ERROR(result,"Error in SetRenderTarget");
	#else
		HRESULT result=device->SetRenderTarget(0,mColorSurface);
		TOADLET_CHECK_D3D9ERROR(result,"Error in SetRenderTarget");

		result=device->SetDepthStencilSurface(mDepthSurface);
		TOADLET_CHECK_D3D9ERROR(result,"Error in SetDepthStencilSurface");
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
	TOADLET_CHECK_D3D9ERROR(result,"reset");

	#if defined(TOADLET_HAS_DIRECT3DMOBILE)
		mD3DDevice->GetRenderTarget(&mColorSurface);
	#else
		mD3DDevice->GetRenderTarget(0,&mColorSurface);
	#endif
	mD3DDevice->GetDepthStencilSurface(&mDepthSurface);
}

bool D3D9WindowRenderTarget::createContext(HWND wnd,const Visual &visual){
	HRESULT result;

	mLibrary=LoadLibrary(TOADLET_D3D_DLL_NAME);
	if(mLibrary==0){
		Error::libraryNotFound(Categories::TOADLET_PEEPER,
			String("D3D9RenderWindow: Error loading ")+TOADLET_D3D_DLL_NAME);
		return false;
	}

	void *symbol=GetProcAddress(mLibrary,TOADLET_D3D_CREATE_NAME);
	if(symbol==NULL){
		Error::symbolNotFound(Categories::TOADLET_PEEPER,
			String("D3D9RenderWindow: Error finding ")+TOADLET_D3D_CREATE_NAME);
		return NULL;
	}

	typedef IDirect3D9*(WINAPI *Direct3DCreate9)(UINT);
	mD3D=((Direct3DCreate9)symbol)(D3D_SDK_VERSION);
	if(mD3D==NULL){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D9RenderWindow: Error creating Direct3D object");
		return false;
	}

	UINT adaptor=D3DADAPTER_DEFAULT;
	D3DADAPTER_IDENTIFIER9 identifier={0};
	mD3D->GetAdapterIdentifier(adaptor,0,&identifier);
	Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
		String("D3D Driver:") + identifier.Driver);
	Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
		String("D3D Description:") + identifier.Description);

	#if defined(TOADLET_HAS_DIRECT3DMOBILE)
		D3DMDEVTYPE devtype=D3DMDEVTYPE_DEFAULT;
	#else
		D3DDEVTYPE devtype=D3DDEVTYPE_HAL;
	#endif

	DWORD flags=0;

	#if !defined(TOADLET_HAS_DIRECT3DMOBILE)
		result=mD3D->CheckDeviceType(adaptor,devtype,D3DFMT_X8R8G8B8,D3DFMT_X8R8G8B8,FALSE);
		if(FAILED(result)){
			Error::unknown(Categories::TOADLET_PEEPER,
				"D3D9RenderWindow: Error creating 8,8,8 bit back buffer");
			return false;
		}

		result=mD3D->CheckDeviceFormat(adaptor,devtype,D3DFMT_X8R8G8B8,D3DUSAGE_DEPTHSTENCIL,D3DRTYPE_SURFACE,D3DFMT_D24S8);
		if(FAILED(result)){
			Error::unknown(Categories::TOADLET_PEEPER,
				"D3D9RenderWindow: Error creating 16 bit depth, 8 bit stencil back buffer");
			return false;
		}

		D3DCAPS9 caps;
		ZeroMemory(&caps,sizeof(D3DCAPS9));
		result=mD3D->GetDeviceCaps(adaptor,devtype,&caps);
		if(FAILED(result)){
			Error::unknown(Categories::TOADLET_PEEPER,
				"D3D9RenderWindow: Error getting device caps");
			return false;
		}

		if(caps.VertexProcessingCaps!=0){
			flags=D3DCREATE_HARDWARE_VERTEXPROCESSING;
		}
		else{
			flags=D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		}
	#endif

	mWindow=wnd;

	fillPresentParameters(mPresentParameters);

	#if !defined(TOADLET_HAS_DIRECT3DMOBILE)
		if(visual.vsync){
			mPresentParameters.PresentationInterval=D3DPRESENT_INTERVAL_DEFAULT;
		}
		else{
			mPresentParameters.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;
		}
	#endif

	result=mD3D->CreateDevice(adaptor,devtype,wnd,flags,&mPresentParameters,&mD3DDevice);
	if(FAILED(result)){
		TOADLET_CHECK_D3D9ERROR(result,"Error creating D3D Device");
		return false;
	}

	#if defined(TOADLET_HAS_DIRECT3DMOBILE)
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
	#if defined(TOADLET_HAS_DIRECT3DMOBILE)
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
}

}
}
