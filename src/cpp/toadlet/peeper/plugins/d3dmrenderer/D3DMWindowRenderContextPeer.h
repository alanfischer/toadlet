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

#ifndef TOADLET_PEEPER_D3DMWINDOWRENDERCONTEXTPEER_H
#define TOADLET_PEEPER_D3DMWINDOWRENDERCONTEXTPEER_H

#include "D3DMRenderContextPeer.h"
#include <toadlet/peeper/Visual.h>

namespace toadlet{
namespace peeper{

class TOADLET_API D3DMWindowRenderContextPeer:public D3DMRenderContextPeer{
public:
	D3DMWindowRenderContextPeer();
	D3DMWindowRenderContextPeer(HWND wnd,const Visual &visual);
	virtual ~D3DMWindowRenderContextPeer();

	virtual bool createContext(HWND wnd,const Visual &visual);

	virtual bool destroyContext();

	virtual int getWidth() const{return mWidth;}

	virtual int getHeight() const{return mHeight;}

	virtual void makeCurrent(IDirect3DMobileDevice *device);

	virtual void reset();

	virtual IDirect3DMobileDevice *getDirect3DMobileDevice() const{return mD3DDevice;}

	inline bool isValid() const{return mD3DDevice!=NULL;}

protected:
	void fillPresentParameters(D3DMPRESENT_PARAMETERS &presentParameters);

	D3DMPRESENT_PARAMETERS mPresentParameters;
	HINSTANCE mLibrary;
	IDirect3DMobile *mD3D;
	IDirect3DMobileDevice *mD3DDevice;
	IDirect3DMobileSurface *mColorSurface;
	IDirect3DMobileSurface *mDepthSurface;
	HWND mWindow;
	int mWidth,mHeight;
};

}
}

#endif
