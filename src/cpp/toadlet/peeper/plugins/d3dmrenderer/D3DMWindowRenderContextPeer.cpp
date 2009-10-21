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

#include "D3DMWindowRenderContextPeer.h"
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

TOADLET_C_API RenderTargetPeer *new_D3DMWindowRenderContextPeer(HWND wnd,const Visual &visual){
	return new D3DMWindowRenderContextPeer(wnd,visual);
}

D3DMWindowRenderContextPeer::D3DMWindowRenderContextPeer():D3DMRenderContextPeer(),
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

D3DMWindowRenderContextPeer::D3DMWindowRenderContextPeer(HWND wnd,const Visual &visual):D3DMRenderContextPeer(),
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

D3DMWindowRenderContextPeer::~D3DMWindowRenderContextPeer(){
	destroyContext();
}

void D3DMWindowRenderContextPeer::makeCurrent(IDirect3DMobileDevice *device){
	HRESULT result=device->SetRenderTarget(mColorSurface,mDepthSurface);
	TOADLET_CHECK_D3DMERROR(result,"Error in SetRenderTarget");
}

void D3DMWindowRenderContextPeer::reset(){
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
	TOADLET_CHECK_D3DMERROR(result,"reset");

	mD3DDevice->GetRenderTarget(&mColorSurface);
	mD3DDevice->GetDepthStencilSurface(&mDepthSurface);
}

bool D3DMWindowRenderContextPeer::createContext(HWND wnd,const Visual &visual){
	HRESULT result;

	mLibrary=LoadLibrary(TEXT("D3DM.dll"));
	if(mLibrary==0){
		Error::libraryNotFound(Categories::TOADLET_PEEPER,
			"D3DMRenderWindow: Error loading D3DM.dll");
		return false;
	}

	void *symbol=GetProcAddress(mLibrary,L"Direct3DMobileCreate");
	if(symbol==NULL){
		Error::symbolNotFound(Categories::TOADLET_PEEPER,
			"D3DMRenderWindow: Error finding Direct3DMobileCreate");
		return NULL;
	}

	typedef IDirect3DMobile*(WINAPI *D3DMobileCreate)(UINT);
	mD3D=((D3DMobileCreate)symbol)(D3DM_SDK_VERSION);
	if(mD3D==NULL){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3DMRenderWindow: Error creating Direct3DMobile object");
		return false;
	}

	UINT adaptor=D3DMADAPTER_DEFAULT;
	D3DMADAPTER_IDENTIFIER identifier={0};
	mD3D->GetAdapterIdentifier(adaptor,0,&identifier);
	Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
		String("D3DM Driver:") + identifier.Driver);
	Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
		String("D3DM Description:") + identifier.Description);

	mWindow=wnd;

	fillPresentParameters(mPresentParameters);

	DWORD flags=0;
	result=mD3D->CreateDevice(adaptor,D3DMDEVTYPE_DEFAULT,wnd,flags,&mPresentParameters,&mD3DDevice);
	if(FAILED(result)){
		TOADLET_CHECK_D3DMERROR(result,"Error creating D3D Device");
		return false;
	}

	mD3DDevice->GetRenderTarget(&mColorSurface);
	mD3DDevice->GetDepthStencilSurface(&mDepthSurface);

	return true;
}

bool D3DMWindowRenderContextPeer::destroyContext(){
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


void D3DMWindowRenderContextPeer::fillPresentParameters(D3DMPRESENT_PARAMETERS &presentParameters){
	RECT rect={0};
	GetClientRect(mWindow,&rect);
	mWidth=rect.right-rect.left;
	mHeight=rect.bottom-rect.top;

	memset(&mPresentParameters,0,sizeof(mPresentParameters));
	mPresentParameters.AutoDepthStencilFormat=D3DMFMT_D16;
	mPresentParameters.EnableAutoDepthStencil=TRUE;
	mPresentParameters.Windowed			=TRUE;
	mPresentParameters.SwapEffect		=D3DMSWAPEFFECT_DISCARD;
	mPresentParameters.BackBufferFormat	=D3DMFMT_UNKNOWN;
}

}
}
