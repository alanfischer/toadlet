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

#ifndef TOADLET_PEEPER_EGLRENDERTARGETPEER_H
#define TOADLET_PEEPER_EGLRENDERTARGETPEER_H

#include "../../GLIncludes.h"
#include "../../GLRenderTargetPeer.h"

namespace toadlet{
namespace peeper{

class TOADLET_API EGLRenderTargetPeer:public GLRenderTargetPeer{
public:
	int egl_version;

	EGLRenderTargetPeer();
	virtual ~EGLRenderTargetPeer(){}

	inline EGLDisplay getEGLDisplay() const{return mDisplay;}
	inline EGLSurface getEGLSurface() const{return mSurface;}
	inline EGLContext getEGLContext() const{return mContext;}

	void makeCurrent();

protected:
	EGLConfig chooseEGLConfig(EGLDisplay display,int redBits,int greenBits,int blueBits,int alphaBits,int depthBits,int stencilBits,bool window,bool pixmap,bool pbuffer,int fsaaCount);
	EGLConfig chooseEGLConfigFromConfigOptions(EGLDisplay display,const EGLint *configOptions);
	EGLConfig pickEGLConfig(EGLDisplay display,EGLConfig oldConfig,EGLConfig newConfig);

	EGLDisplay mDisplay;
	EGLSurface mSurface;
	EGLContext mContext;
};

}
}

#endif
