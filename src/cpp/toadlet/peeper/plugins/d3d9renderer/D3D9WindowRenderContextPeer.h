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

#ifndef TOADLET_PEEPER_D3D9WINDOWRENDERCONTEXTPEER_H
#define TOADLET_PEEPER_D3D9WINDOWRENDERCONTEXTPEER_H

#include "D3D9RenderContextPeer.h"
#include <toadlet/peeper/Visual.h>

namespace toadlet{
namespace peeper{

class TOADLET_API D3D9WindowRenderContextPeer:public D3D9RenderContextPeer{
public:
	D3D9WindowRenderContextPeer();
	D3D9WindowRenderContextPeer(HWND wnd,const Visual &visual);
	virtual ~D3D9WindowRenderContextPeer();

	virtual bool createContext(HWND wnd,const Visual &visual);

	virtual bool destroyContext();

	virtual int getWidth() const{return mWidth;}

	virtual int getHeight() const{return mHeight;}

	virtual void makeCurrent(IDirect3DDevice9 *device);

	virtual void reset();

	inline bool isValid() const{return mD3DDevice!=NULL;}

	inline IDirect3DDevice9 *getDirect3DDevice9() const{return mD3DDevice;}
	inline const D3DPRESENT_PARAMETERS &getPresentParameters() const{return mPresentParameters;}

protected:
	void fillPresentParameters(D3DPRESENT_PARAMETERS &presentParameters);

	D3DPRESENT_PARAMETERS mPresentParameters;
	HINSTANCE mLibrary;
	IDirect3D9 *mD3D;
	IDirect3DDevice9 *mD3DDevice;
	IDirect3DSurface9 *mColorSurface;
	IDirect3DSurface9 *mDepthSurface;	
	HWND mWindow;
	int mWidth,mHeight;
};

}
}

#endif
