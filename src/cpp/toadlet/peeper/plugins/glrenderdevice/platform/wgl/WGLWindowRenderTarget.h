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

#ifndef TOADLET_PEEPER_WGLWINDOWRENDERTARGET_H
#define TOADLET_PEEPER_WGLWINDOWRENDERTARGET_H

#include "WGLRenderTarget.h"
#include <toadlet/peeper/WindowRenderTargetFormat.h>

namespace toadlet{
namespace peeper{

class TOADLET_API WGLWindowRenderTarget:public WGLRenderTarget{
public:
	WGLWindowRenderTarget();
	WGLWindowRenderTarget(HWND wnd,WindowRenderTargetFormat *format);
	virtual ~WGLWindowRenderTarget();
	void destroy(){destroyContext();}

	RenderTarget *getRootRenderTarget(){return (GLRenderTarget*)this;}

	bool createContext(HWND wnd,WindowRenderTargetFormat *format,int winPixelFormat=0);
	bool destroyContext();
	bool activateAdditionalContext();
	void deactivateAdditionalContext();

	bool swap();

	bool isPrimary() const{return true;}
	bool isValid() const{return mGLRC!=0;}
	int getWidth() const;
	int getHeight() const;

protected:
	int checkDefaultColorBits(int colorBits) const;

	HWND mWnd;
	PIXELFORMATDESCRIPTOR mPFD;
	Collection<HGLRC> mThreadContexts;
	Collection<int> mThreadIDs;
};

}
}

#endif
