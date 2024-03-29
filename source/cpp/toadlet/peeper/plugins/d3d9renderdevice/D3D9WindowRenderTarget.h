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

#ifndef TOADLET_PEEPER_D3D9WINDOWRENDERTARGET_H
#define TOADLET_PEEPER_D3D9WINDOWRENDERTARGET_H

#include "D3D9RenderTarget.h"
#include <toadlet/egg/BaseResource.h>
#include <toadlet/peeper/WindowRenderTargetFormat.h>

namespace toadlet{
namespace peeper{

#if defined(TOADLET_SET_D3DM)
	TOADLET_C_API RenderTarget *new_D3DMWindowRenderTarget(void *display,void *window,WindowRenderTargetFormat *format);
#else
	TOADLET_C_API RenderTarget *new_D3D9WindowRenderTarget(void *display,void *window,WindowRenderTargetFormat *format);
#endif

class TOADLET_API D3D9WindowRenderTarget:public BaseResource,public D3D9RenderTarget{
public:
	TOADLET_RESOURCE(D3D9WindowRenderTarget,D3D9RenderTarget);

	D3D9WindowRenderTarget();
	D3D9WindowRenderTarget(HWND wnd,WindowRenderTargetFormat *format);
	void destroy();

	RenderTarget *getRootRenderTarget(){return this;}
	bool isPrimary() const{return true;}
	bool isValid() const{return mD3DDevice!=NULL;}

	bool createContext(HWND wnd,WindowRenderTargetFormat *format);
	bool destroyContext();

	int getWidth() const{RECT r;GetClientRect(mWindow,&r);return r.right-r.left;}
	int getHeight() const{RECT r;GetClientRect(mWindow,&r);return r.bottom-r.top;}

	bool activate();
	bool deactivate();

	AdaptorInfo *getAdaptorInfo() const{return mAdaptorInfo;}

	void swap();
	void reset();

	inline IDirect3DDevice9 *getDirect3DDevice9() const{return mD3DDevice;}
	inline IDirect3D9 *getDirect3D9() const{return mD3D;}
	inline const D3DPRESENT_PARAMETERS &getPresentParameters() const{return mPresentParameters;}
	inline IDirect3DSurface9 *getColorSurface() const{return mColorSurface;}
	inline IDirect3DSurface9 *getDepthSurface() const{return mDepthSurface;}

	typedef IDirect3D9*(WINAPI *Direct3DCreate9_)(UINT);
	Direct3DCreate9_ Direct3DCreate9;

	typedef HRESULT(WINAPI *D3DXCompileShader_)(LPCSTR,UINT,struct _D3DXMACRO *,interface ID3DXInclude *,LPCSTR,LPCSTR,DWORD,interface ID3DXBuffer **,interface ID3DXBuffer **,interface ID3DXConstantTable **);
	D3DXCompileShader_ D3DXCompileShader;

protected:
	int getClosestSamples(int samples);

	UINT mAdaptor;
	#if defined(TOADLET_SET_D3DM)
		D3DMDEVTYPE mDevType;
	#else
		D3DDEVTYPE mDevType;
	#endif
	int mSamples;
	D3DPRESENT_PARAMETERS mPresentParameters;
	HINSTANCE mD3DLibrary,mD3DXLibrary;

	IDirect3D9 *mD3D;
	IDirect3DDevice9 *mD3DDevice;
	IDirect3DSurface9 *mColorSurface;
	IDirect3DSurface9 *mDepthSurface;
	HWND mWindow;
	AdaptorInfo::ptr mAdaptorInfo;
};

}
}

#endif
