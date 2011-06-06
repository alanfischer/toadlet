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
	mDXGISwapChain(NULL),
	mDXGIDevice(NULL),
	mDXGIAdapter(NULL),
	mDepthTexture(NULL),
	mWindow(0),
	mWidth(0),
	mHeight(0)
{
	mRenderTargetViews.resize(1,NULL);
}

D3D10WindowRenderTarget::D3D10WindowRenderTarget(HWND wnd,WindowRenderTargetFormat *format):D3D10RenderTarget(),
	mLibrary(0),
	mDXGISwapChain(NULL),
	mDXGIDevice(NULL),
	mDXGIAdapter(NULL),
	mDepthTexture(NULL),
	mWindow(0),
	mWidth(0),
	mHeight(0)
{
	mRenderTargetViews.resize(1,NULL);
	createContext(wnd,format);
}

D3D10WindowRenderTarget::~D3D10WindowRenderTarget(){
	destroyContext();
}

void D3D10WindowRenderTarget::swap(){
	mDXGISwapChain->Present(0,0);
}

void D3D10WindowRenderTarget::reset(){
	if(mRenderTargetViews[0]!=NULL){
		mRenderTargetViews[0]->Release();
		mRenderTargetViews[0]=NULL;
	}

	mD3DDevice->OMGetRenderTargets(1,&mRenderTargetViews[0],NULL);
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
	result=((D3D10CreateDeviceAndSwapChain)symbol)(NULL,D3D10_DRIVER_TYPE_HARDWARE,NULL,flags,D3D10_SDK_VERSION,&desc,&mDXGISwapChain,&mD3DDevice);
	if(FAILED(result) || mD3DDevice==NULL){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D10RenderWindow: Error creating D3D10Device object");
		return false;
	}

	mD3DDevice->QueryInterface(__uuidof(mDXGIDevice),(void**)&mDXGIDevice);
	mDXGIDevice->GetAdapter(&mDXGIAdapter);

	DXGI_ADAPTER_DESC adapterDesc;
	mDXGIAdapter->GetDesc(&adapterDesc);

	Logger::alert(Categories::TOADLET_PEEPER,
		String("D3D Description:") + adapterDesc.Description);

	ID3D10Texture2D *texture;
	mDXGISwapChain->GetBuffer(0,__uuidof(texture),(void**)&texture);
	D3D10_TEXTURE2D_DESC textureDesc;
	texture->GetDesc(&textureDesc);
	mD3DDevice->CreateRenderTargetView(texture,NULL,&mRenderTargetViews[0]);

	mDepthTexture=new D3D10Texture(mD3DDevice);
	mDepthTexture->create(Texture::Usage_BIT_RENDERTARGET,Texture::Dimension_D2,Texture::Format_DEPTH_16,textureDesc.Width,textureDesc.Height,0,1,NULL);
	mD3DDevice->CreateDepthStencilView(mDepthTexture->getD3D10Resource(),NULL,&mDepthStencilView);

	return true;
}

bool D3D10WindowRenderTarget::destroyContext(){
	deactivate();

	if(mRenderTargetViews[0]!=NULL){
		mRenderTargetViews[0]->Release();
		mRenderTargetViews[0]=NULL;
	}

	if(mDepthStencilView!=NULL){
		mDepthStencilView->Release();
		mDepthStencilView=NULL;
	}

	if(mDepthTexture!=NULL){
		delete mDepthTexture;
		mDepthTexture=NULL;
	}

	if(mD3DDevice!=NULL){
		mD3DDevice->Release();
		mD3DDevice=NULL;
	}

	if(mDXGIDevice!=NULL){
		mDXGIDevice->Release();
		mDXGIDevice=NULL;
	}

	if(mDXGIAdapter!=NULL){
		mDXGIAdapter->Release();
		mDXGIAdapter=NULL;
	}

	if(mLibrary!=0){
		FreeLibrary(mLibrary);
		mLibrary=0;
	}

	return true;
}

}
}