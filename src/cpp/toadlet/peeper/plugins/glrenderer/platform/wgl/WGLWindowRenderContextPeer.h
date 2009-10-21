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

#ifndef TOADLET_PEEPER_WGLWINDOWRENDERCONTEXTPEER_H
#define TOADLET_PEEPER_WGLWINDOWRENDERCONTEXTPEER_H

#include "WGLRenderTargetPeer.h"
#include <toadlet/peeper/Visual.h>

namespace toadlet{
namespace peeper{

class TOADLET_API WGLWindowRenderContextPeer:public WGLRenderTargetPeer{
public:
	WGLWindowRenderContextPeer();
	WGLWindowRenderContextPeer(HWND wnd,const Visual &visual,int pixelFormat=0);
	virtual ~WGLWindowRenderContextPeer();

	virtual bool createContext(HWND wnd,const Visual &visual,int pixelFormat=0);

	virtual bool destroyContext();

	virtual int getWidth() const;

	virtual int getHeight() const;

	virtual void swap();

	inline bool isValid() const{return mGLRC!=0;}

protected:
	int checkDefaultColorBits(int colorBits) const;

	HWND mWnd;
	PIXELFORMATDESCRIPTOR mPFD;
};

}
}

#endif
