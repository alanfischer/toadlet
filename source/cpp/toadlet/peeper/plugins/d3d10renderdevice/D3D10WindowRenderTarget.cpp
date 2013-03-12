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
#include <toadlet/egg/Log.h>

namespace toadlet{
namespace peeper{

#if defined(TOADLET_SET_D3D10)
	TOADLET_C_API RenderTarget *new_D3D10WindowRenderTarget(void *display,void *window,WindowRenderTargetFormat *format){
		return new D3D10WindowRenderTarget((HWND)window,format);
	}
#else
	TOADLET_C_API RenderTarget *new_D3D11WindowRenderTarget(void *display,void *window,WindowRenderTargetFormat *format){
		return new D3D10WindowRenderTarget((HWND)window,format);
	}
#endif

D3D10WindowRenderTarget::D3D10WindowRenderTarget():D3D10RenderTarget(),
	mD3DLibrary(0),mD3DXLibrary(0),
	mDXGISwapChain(NULL),
	mDXGIDevice(NULL),
	mDXGIAdapter(NULL),
	mDepthTexture(NULL),
	mWindow(0)
{
	mRenderTargetViews.resize(1,NULL);
}

D3D10WindowRenderTarget::D3D10WindowRenderTarget(HWND wnd,WindowRenderTargetFormat *format):D3D10RenderTarget(),
	mD3DLibrary(0),mD3DXLibrary(0),
	mDXGISwapChain(NULL),
	mDXGIDevice(NULL),
	mDXGIAdapter(NULL),
	mDepthTexture(NULL),
	mWindow(0)
{
	mRenderTargetViews.resize(1,NULL);
	createContext(wnd,format);
}

void D3D10WindowRenderTarget::destroy(){
	destroyContext();
	
	BaseResource::destroy();
}

void D3D10WindowRenderTarget::swap(){
	mDXGISwapChain->Present(0,0);
}

void D3D10WindowRenderTarget::reset(){
	if(mRenderTargetViews[0]!=NULL){
		mRenderTargetViews[0]->Release();
		mRenderTargetViews[0]=NULL;
	}

	mD3DDevice->ClearState();

	HRESULT result=mDXGISwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

	ID3D10Texture2D *texture;
	mDXGISwapChain->GetBuffer(0,__uuidof(texture),(void**)&texture);
	D3D10_TEXTURE2D_DESC textureDesc;
	texture->GetDesc(&textureDesc);
	mD3DDevice->CreateRenderTargetView(texture,NULL,&mRenderTargetViews[0]);

//	mD3DDevice->OMGetRenderTargets(1,&mRenderTargetViews[0],NULL);
}

bool D3D10WindowRenderTarget::createContext(HWND wnd,WindowRenderTargetFormat *format){
	HRESULT result;

	mD3DLibrary=LoadLibrary(TOADLET_D3D10_DLL_NAME);
	if(mD3DLibrary==0){
		Error::libraryNotFound(Categories::TOADLET_PEEPER,
			String("D3D10RenderWindow: Error loading ")+TOADLET_D3D10_DLL_NAME);
		return false;
	}

	CreateDeviceAndSwapChainSymbol=(D3D10CreateDeviceAndSwapChain)GetProcAddress(mD3DLibrary,TOADLET_D3D10_CREATE_DEVICE_AND_SWAP_CHAIN_NAME);
	if(CreateDeviceAndSwapChainSymbol==NULL){
		Error::symbolNotFound(Categories::TOADLET_PEEPER,
			String("D3D10RenderWindow: Error finding ")+TOADLET_D3D10_CREATE_DEVICE_AND_SWAP_CHAIN_NAME);
		return NULL;
	}

	ReflectShaderSymbol=(D3D10ReflectShader)GetProcAddress(mD3DLibrary,TOADLET_D3D10_REFLECT_SHADER_NAME);
	if(ReflectShaderSymbol==NULL){
		Error::symbolNotFound(Categories::TOADLET_PEEPER,
			String("D3D10RenderWindow: Error finding ")+TOADLET_D3D10_REFLECT_SHADER_NAME);
		return NULL;
	}

	mD3DXLibrary=LoadLibrary(TOADLET_D3DX10_DLL_NAME);
	if(mD3DXLibrary==0){
		Error::libraryNotFound(Categories::TOADLET_PEEPER,
			String("D3D10RenderWindow: Error loading ")+TOADLET_D3DX10_DLL_NAME);
		return false;
	}

	CompileFromMemorySymbol=(D3DX10CompileFromMemory)GetProcAddress(mD3DXLibrary,TOADLET_D3DX10_COMPILE_FROM_MEMORY_NAME);
	if(CompileFromMemorySymbol==NULL){
		Error::symbolNotFound(Categories::TOADLET_PEEPER,
			String("D3D10RenderWindow: Error finding ")+TOADLET_D3DX10_COMPILE_FROM_MEMORY_NAME);
		return NULL;
	}

	mWindow=wnd;
	RECT rect={0};
	GetClientRect(mWindow,&rect);

	DXGI_SWAP_CHAIN_DESC desc={0};
	desc.BufferCount=1;
	desc.BufferDesc.Width=rect.right-rect.left;
	desc.BufferDesc.Height=rect.bottom-rect.top;
	desc.BufferDesc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.OutputWindow=mWindow;
	desc.SampleDesc.Count=1;
	desc.SampleDesc.Quality=0;
	desc.Windowed=TRUE;

	int flags=format->flags;

	if(format->debug){
		flags|=D3D10_CREATE_DEVICE_DEBUG;
	}

	if(format->threads<=1){
		flags|=D3D10_CREATE_DEVICE_SINGLETHREADED;
	}

	result=CreateDeviceAndSwapChainSymbol(NULL,D3D10_DRIVER_TYPE_HARDWARE,NULL,flags,D3D10_SDK_VERSION,&desc,&mDXGISwapChain,&mD3DDevice);
	if(result==E_FAIL && (flags&D3D10_CREATE_DEVICE_DEBUG)!=0){
		flags&=~D3D10_CREATE_DEVICE_DEBUG;
		result=CreateDeviceAndSwapChainSymbol(NULL,D3D10_DRIVER_TYPE_HARDWARE,NULL,flags,D3D10_SDK_VERSION,&desc,&mDXGISwapChain,&mD3DDevice);
	}
	if(FAILED(result) || mD3DDevice==NULL){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D10RenderWindow: Error creating D3D10Device object");
		return false;
	}

	mD3DDevice->QueryInterface(__uuidof(mDXGIDevice),(void**)&mDXGIDevice);
	mDXGIDevice->GetAdapter(&mDXGIAdapter);

	DXGI_ADAPTER_DESC adapterDesc;
	mDXGIAdapter->GetDesc(&adapterDesc);

	mAdaptorInfo=new AdaptorInfo();
	mAdaptorInfo->setDescription(adapterDesc.Description);

	Log::alert(Categories::TOADLET_PEEPER,
		String("D3D Description:") + adapterDesc.Description);

	ID3D10Texture2D *texture;
	mDXGISwapChain->GetBuffer(0,__uuidof(texture),(void**)&texture);
	D3D10_TEXTURE2D_DESC textureDesc;
	texture->GetDesc(&textureDesc);
	mD3DDevice->CreateRenderTargetView(texture,NULL,&mRenderTargetViews[0]);

	mDepthTexture=new D3D10Texture(mD3DDevice);
	TextureFormat::ptr textureFormat=new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_DEPTH_16,textureDesc.Width,textureDesc.Height,1,1);
	mDepthTexture->create(Texture::Usage_BIT_RENDERTARGET,textureFormat,NULL);
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

	if(mD3DLibrary!=0){
		FreeLibrary(mD3DLibrary);
		mD3DLibrary=0;
	}

	if(mD3DXLibrary!=0){
		FreeLibrary(mD3DXLibrary);
		mD3DXLibrary=0;
	}

	return true;
}

}
}
