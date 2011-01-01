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

#include "D3D10WindowRenderTarget.h"
#include "D3D10Renderer.h"
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

#if defined(TOADLET_SET_D3D10)
	TOADLET_C_API RenderTarget *new_D3D10WindowRenderTarget(HWND wnd,WindowRenderTargetFormat *format){
		return new D3D10WindowRenderTarget(wnd,format);
	}
#else
	TOADLET_C_API RenderTarget *new_D3D11WindowRenderTarget(HWND wnd,WindowRenderTargetFormat *format){
		return new D3D10WindowRenderTarget(wnd,format);
	}
#endif

D3D10WindowRenderTarget::D3D10WindowRenderTarget():D3D10RenderTarget(),
	mLibrary(0),
	mD3DDevice(NULL),
	mRenderTargetView(NULL),
	mDepthStencilView(NULL),
	mWindow(0),
	mWidth(0),
	mHeight(0)
{
}

D3D10WindowRenderTarget::D3D10WindowRenderTarget(HWND wnd,WindowRenderTargetFormat *format):D3D10RenderTarget(),
	mLibrary(0),
	mSwapChain(NULL),
	mD3DDevice(NULL),
	mRenderTargetView(NULL),
	mDepthStencilView(NULL),
	mWindow(0),
	mWidth(0),
	mHeight(0)
{
	createContext(wnd,format);
}

D3D10WindowRenderTarget::~D3D10WindowRenderTarget(){
	destroyContext();
}

bool D3D10WindowRenderTarget::activate(){
	mD3DDevice->OMSetRenderTargets(1,&mRenderTargetView,mDepthStencilView);

	return true;
}

void D3D10WindowRenderTarget::clear(int clearFlags,const Color &clearColor){
	if(mRenderTargetView!=NULL && (clearFlags&Renderer::ClearFlag_COLOR)>0){
		mD3DDevice->ClearRenderTargetView(mRenderTargetView,clearColor.getData());
	}
	if(mDepthStencilView!=NULL){
		UINT d3dclearFlags=0;
		if(clearFlags&Renderer::ClearFlag_DEPTH){
			d3dclearFlags|=D3D10_CLEAR_DEPTH;
		}
		if(clearFlags&Renderer::ClearFlag_STENCIL){
			d3dclearFlags|=D3D10_CLEAR_STENCIL;
		}
		mD3DDevice->ClearDepthStencilView(mDepthStencilView,d3dclearFlags,1.0f,0);
	}
}

void D3D10WindowRenderTarget::swap(){
	mSwapChain->Present(0,0);
}

void D3D10WindowRenderTarget::reset(){
	if(mRenderTargetView!=NULL){
		mRenderTargetView->Release();
		mRenderTargetView=NULL;
	}

	if(mDepthStencilView!=NULL){
		mDepthStencilView->Release();
		mDepthStencilView=NULL;
	}

//	fillPresentParameters(mPresentParameters);

	mD3DDevice->OMGetRenderTargets(1,&mRenderTargetView,&mDepthStencilView);
}

bool D3D10WindowRenderTarget::createContext(HWND wnd,WindowRenderTargetFormat *format){
	HRESULT result;

	mLibrary=LoadLibrary(TOADLET_D3D10_DLL_NAME);
	if(mLibrary==0){
		Error::libraryNotFound(Categories::TOADLET_PEEPER,
			String("D3D10RenderWindow: Error loading ")+TOADLET_D3D10_DLL_NAME);
		return false;
	}

	mWindow=wnd;
	RECT rect={0};
	GetClientRect(mWindow,&rect);
	mWidth=rect.right-rect.left;
	mHeight=rect.bottom-rect.top;

	DXGI_SWAP_CHAIN_DESC desc={0};
	desc.BufferCount=1;
	desc.BufferDesc.Width=mWidth;
	desc.BufferDesc.Height=mHeight;
	desc.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow=mWindow;
	desc.SampleDesc.Count=1;
	desc.SampleDesc.Quality=0;
	desc.Windowed=TRUE;

//	void *symbol=GetProcAddress(mLibrary,TOADLET_D3D10_CREATE_DEVICE_NAME);
//	if(symbol==NULL){
//		Error::symbolNotFound(Categories::TOADLET_PEEPER,
//			String("D3D10RenderWindow: Error finding ")+TOADLET_D3D10_CREATE_DEVICE_NAME);
//		return NULL;
//	}

//	typedef HRESULT(WINAPI *D3D10CreateDevice)(IDXGIAdapter *pAdapter,D3D10_DRIVER_TYPE DriverType,HMODULE Software,UINT Flags,UINT SDKVersion,ID3D10Device **ppDevice);
//	result=((D3D10CreateDevice)symbol)(NULL,D3D10_DRIVER_TYPE_HARDWARE,NULL,0,D3D10_SDK_VERSION,&mD3DDevice);
//	if(FAILED(result) || mD3DDevice==NULL){
//		Error::unknown(Categories::TOADLET_PEEPER,
//			"D3D10RenderWindow: Error creating D3D10Device object");
//		return false;
//	}

	void *symbol=GetProcAddress(mLibrary,TOADLET_D3D10_CREATE_DEVICE_AND_SWAP_CHAIN_NAME);
	if(symbol==NULL){
		Error::symbolNotFound(Categories::TOADLET_PEEPER,
			String("D3D10RenderWindow: Error finding ")+TOADLET_D3D10_CREATE_DEVICE_AND_SWAP_CHAIN_NAME);
		return NULL;
	}

	int flags=format->flags;

	if(format->debug){
		flags|=D3D10_CREATE_DEVICE_DEBUG;
	}

	if(format->threads<=1){
		flags|=D3D10_CREATE_DEVICE_SINGLETHREADED;
	}

	typedef HRESULT(WINAPI *D3D10CreateDeviceAndSwapChain)(IDXGIAdapter *pAdapter,D3D10_DRIVER_TYPE DriverType,HMODULE Software,UINT Flags,UINT SDKVersion,DXGI_SWAP_CHAIN_DESC *pSwapChainDesc,IDXGISwapChain **ppSwapChain,ID3D10Device **ppDevice);
	result=((D3D10CreateDeviceAndSwapChain)symbol)(NULL,D3D10_DRIVER_TYPE_HARDWARE,NULL,flags,D3D10_SDK_VERSION,&desc,&mSwapChain,&mD3DDevice);
	if(FAILED(result) || mD3DDevice==NULL){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D10RenderWindow: Error creating D3D10Device object");
		return false;
	}


//	UINT adaptor=D3DADAPTER_DEFAULT;
//	D3DADAPTER_IDENTIFIER9 identifier={0};
//	mD3D->GetAdapterIdentifier(adaptor,0,&identifier);
//	Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
//		String("D3D Driver:") + identifier.Driver);
//	Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
//		String("D3D Description:") + identifier.Description);

//	fillPresentParameters(mPresentParameters);

	ID3D10Texture2D *texture;
	mSwapChain->GetBuffer(0,__uuidof(texture),(void**)&texture);
	mD3DDevice->CreateRenderTargetView(texture,NULL,&mRenderTargetView);
//	mD3DDevice->CreateDepthStencilView(texture,NULL,&mDepthStencilView);

//	mD3DDevice->OMGetRenderTargets(1,&mRenderTargetView,&mDepthStencilView);

	return true;
}

bool D3D10WindowRenderTarget::destroyContext(){
	if(mRenderTargetView!=NULL){
		mRenderTargetView->Release();
		mRenderTargetView=NULL;
	}

	if(mDepthStencilView!=NULL){
		mDepthStencilView->Release();
		mDepthStencilView=NULL;
	}

	if(mD3DDevice!=NULL){
		mD3DDevice->Release();
		mD3DDevice=NULL;
	}

	if(mLibrary!=0){
		FreeLibrary(mLibrary);
		mLibrary=0;
	}

	return true;
}
/*
void D3D10WindowRenderTarget::fillPresentParameters(D3DPRESENT_PARAMETERS &presentParameters){
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
*/
}
}
