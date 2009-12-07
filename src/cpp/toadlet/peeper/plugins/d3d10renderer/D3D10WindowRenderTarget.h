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
#include <toadlet/peeper/Visual.h>

namespace toadlet{
namespace peeper{

class TOADLET_API D3D10WindowRenderTarget:public D3D10RenderTarget{
public:
	D3D10WindowRenderTarget();
	D3D10WindowRenderTarget(HWND wnd,const Visual &visual);
	virtual ~D3D10WindowRenderTarget();

	virtual RenderTarget *getRootRenderTarget(){return this;}
	virtual bool isPrimary() const{return true;}
	virtual bool isValid() const{return mD3DDevice!=NULL;}

	virtual bool createContext(HWND wnd,const Visual &visual);

	virtual bool destroyContext();

	virtual int getWidth() const{return mWidth;}

	virtual int getHeight() const{return mHeight;}

	virtual bool makeCurrent(ID3D10Device *device);

	virtual void reset();

	inline ID3D10Device *getD3D10Device() const{return mD3DDevice;}
//	inline const D3DPRESENT_PARAMETERS &getPresentParameters() const{return mPresentParameters;}
	inline ID3D10RenderTargetView *getRenderTargetView() const{return mRenderTargetView;}

protected:
//	void fillPresentParameters(D3DPRESENT_PARAMETERS &presentParameters);

//	D3DPRESENT_PARAMETERS mPresentParameters;
	HINSTANCE mLibrary;
	ID3D10Device *mD3DDevice;
	ID3D10RenderTargetView *mRenderTargetView;
	ID3D10DepthStencilView *mDepthStencilView;
	HWND mWindow;
	int mWidth,mHeight;
};

}
}

#endif
