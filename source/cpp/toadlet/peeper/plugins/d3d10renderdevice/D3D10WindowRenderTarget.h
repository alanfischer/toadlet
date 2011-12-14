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

#ifndef TOADLET_PEEPER_D3D10WINDOWRENDERTARGET_H
#define TOADLET_PEEPER_D3D10WINDOWRENDERTARGET_H

#include "D3D10RenderTarget.h"
#include "D3D10Texture.h"
#include <toadlet/peeper/WindowRenderTargetFormat.h>

namespace toadlet{
namespace peeper{

#if defined(TOADLET_SET_D3D10)
	TOADLET_C_API RenderTarget *new_D3D10WindowRenderTarget(void *display,void *window,WindowRenderTargetFormat *format);
#else
	TOADLET_C_API RenderTarget *new_D3D11WindowRenderTarget(void *display,void *window,WindowRenderTargetFormat *format);
#endif

class TOADLET_API D3D10WindowRenderTarget:public D3D10RenderTarget{
public:
	D3D10WindowRenderTarget();
	D3D10WindowRenderTarget(HWND wnd,WindowRenderTargetFormat *format);
	virtual ~D3D10WindowRenderTarget();
	void destroy(){destroyContext();}

	RenderTarget *getRootRenderTarget(){return this;}
	bool isPrimary() const{return true;}
	bool isValid() const{return mD3DDevice!=NULL;}

	bool createContext(HWND wnd,WindowRenderTargetFormat *format);
	bool destroyContext();

	int getWidth() const{return mWidth;}
	int getHeight() const{return mHeight;}

	void swap();

	void reset();

	inline ID3D10RenderTargetView *getRenderTargetView() const{return mRenderTargetViews[0];}

	typedef HRESULT(WINAPI *D3D10CreateDeviceAndSwapChain)(IDXGIAdapter*,D3D10_DRIVER_TYPE,HMODULE,UINT,UINT,DXGI_SWAP_CHAIN_DESC*,IDXGISwapChain**,ID3D10Device**);
	D3D10CreateDeviceAndSwapChain CreateDeviceAndSwapChainSymbol;

	typedef HRESULT(WINAPI *D3D10ReflectShader)(void*,SIZE_T,ID3D10ShaderReflection**);
	D3D10ReflectShader ReflectShaderSymbol;

	typedef HRESULT(WINAPI *D3DX10CompileFromMemory)(LPCSTR,SIZE_T,LPCSTR,struct _D3D_SHADER_MACRO*,interface ID3DInclude*,LPCSTR,LPCSTR,UINT,UINT,interface ID3DX10ThreadPump*,interface ID3D10Blob**,interface ID3D10Blob**,HRESULT*);
	D3DX10CompileFromMemory CompileFromMemorySymbol;

protected:
	HINSTANCE mD3DLibrary,mD3DXLibrary;
	IDXGISwapChain *mDXGISwapChain;
	IDXGIDevice *mDXGIDevice;
	IDXGIAdapter *mDXGIAdapter;
	D3D10Texture *mDepthTexture;
	HWND mWindow;
	int mWidth,mHeight;
};

}
}

#endif
