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

class TOADLET_API D3D10WindowRenderTarget:public D3D10RenderTarget{
public:
	D3D10WindowRenderTarget();
	D3D10WindowRenderTarget(HWND wnd,WindowRenderTargetFormat *format);
	virtual ~D3D10WindowRenderTarget();

	virtual RenderTarget *getRootRenderTarget(){return this;}
	virtual bool isPrimary() const{return true;}
	virtual bool isValid() const{return mD3DDevice!=NULL;}

	virtual bool createContext(HWND wnd,WindowRenderTargetFormat *format);
	virtual bool destroyContext();

	virtual int getWidth() const{return mWidth;}
	virtual int getHeight() const{return mHeight;}

	virtual void swap();

	virtual void reset();

	inline ID3D10RenderTargetView *getRenderTargetView() const{return mRenderTargetViews[0];}

protected:
	HINSTANCE mLibrary;
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
