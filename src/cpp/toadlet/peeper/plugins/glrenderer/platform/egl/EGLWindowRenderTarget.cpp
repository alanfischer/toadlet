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

#include "EGLWIndowRenderTarget.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/image/ImageFormatConversion.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;

namespace toadlet{
namespace peeper{

TOADLET_C_API RenderTarget *new_EGLWindowRenderTarget(void *nativeSurface,const Visual &visual){
	return new EGLWindowRenderTarget(nativeSurface,visual);
}

EGLWindowRenderTarget::EGLWindowRenderTarget():EGLRenderTarget(),
	mConfig(0),
	mPixmap(false)
{}

EGLWindowRenderTarget::EGLWindowRenderTarget(void *nativeSurface,const Visual &visual,bool pixmap):EGLRenderTarget(),
	mConfig(0),
	mPixmap(false)
{
	createContext(nativeSurface,visual,pixmap);
}

EGLWindowRenderTarget::~EGLWindowRenderTarget(){
	destroyContext();
}

bool EGLWindowRenderTarget::createContext(void *nativeSurface,const Visual &visual,bool pixmap){
	if(mContext!=EGL_NO_CONTEXT){
		return true;
	}

	mPixmap=pixmap;

	#if defined(TOADLET_HAS_GLESEM)
		if(!glesem_getInitialized()){
			int glesemInitResult=glesem_init(GLESEM_ACCELERATED_IF_AVAILABLE);
			if(glesemInitResult<1){
				Error::unknown(Categories::TOADLET_PEEPER,
					"glesem_init failed");
				return false;
			}
		}
	#endif

	mDisplay=eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if(mDisplay==EGL_NO_DISPLAY){
		Error::unknown(Categories::TOADLET_PEEPER,
			"error getting display");
		return false;
	}

	#if defined(TOADLET_HAS_GLESEM)
		if(glesem_eglInitialize(mDisplay)==false){
			Error::unknown(Categories::TOADLET_PEEPER,
				"glesem_eglInitialize error");
			return false;
		}
		egl_version=glesem_egl_majorVersion*10+glesem_egl_minorVersion;
	#else
		int version[2];
		if(eglInitialize(mDisplay,&version[0],&version[1])==false){
			Error::unknown(Logger.TOADLET_PEEPER,
				"eglInitialize error");
			return false;
		}
		egl_version=version[0]*10+version[1];
	#endif
	Logger::alert(Categories::TOADLET_PEEPER,
		String("CALCULATED EGL VERSION:")+(egl_version/10)+"."+(egl_version%10));

	Logger::alert(Categories::TOADLET_PEEPER,
		String("EGL_VENDOR:")+eglQueryString(mDisplay,EGL_VENDOR));
	Logger::alert(Categories::TOADLET_PEEPER,
		String("EGL_VERSION:")+eglQueryString(mDisplay,EGL_VERSION));
	Logger::alert(Categories::TOADLET_PEEPER,
		String("EGL_EXTENSIONS:")+eglQueryString(mDisplay,EGL_EXTENSIONS));

	int format=visual.pixelFormat;
	int redBits=ImageFormatConversion::getRedBits(format);
	int greenBits=ImageFormatConversion::getGreenBits(format);
	int blueBits=ImageFormatConversion::getBlueBits(format);
	int alphaBits=ImageFormatConversion::getAlphaBits(format);
	int depthBits=visual.depthBits;
	int stencilBits=visual.stencilBits;
	int multisamples=visual.multisamples;

	Logger::alert(Categories::TOADLET_PEEPER,
		String("Choosing config for color:")+redBits+","+greenBits+","+blueBits+","+alphaBits+" depth:"+depthBits+" stencil:"+stencilBits);

	mConfig=chooseEGLConfig(mDisplay,redBits,greenBits,blueBits,alphaBits,depthBits,stencilBits,!pixmap,pixmap,false,visual.multisamples);
	TOADLET_CHECK_EGLERROR("chooseEGLConfig");

	Logger::debug(Categories::TOADLET_PEEPER,
		String("chooseEGLConfig config:")+(int)mConfig);

	if(!pixmap){
		mSurface=eglCreateWindowSurface(mDisplay,mConfig,nativeSurface,NULL);
		TOADLET_CHECK_EGLERROR("eglCreateWindowSurface");
	}
	else{
		mSurface=eglCreatePixmapSurface(mDisplay,mConfig,nativeSurface,NULL);
		TOADLET_CHECK_EGLERROR("eglCreatePixmapSurface");
	}

	if(mSurface==EGL_NO_SURFACE){
		Error::unknown(Categories::TOADLET_PEEPER,
			"error creating surface");
		return false;
	}

	mContext=eglCreateContext(mDisplay,mConfig,EGL_NO_CONTEXT,NULL);
	TOADLET_CHECK_EGLERROR("eglCreateContext");

	if(mContext==EGL_NO_CONTEXT){
		Error::unknown(Categories::TOADLET_PEEPER,
			"error creating context");
		return false;
	}

	#if defined(TOADLET_HAS_EGL_12)
	if(egl_version>=12){
		eglBindAPI(EGL_OPENGL_ES_API);
		TOADLET_CHECK_EGLERROR("eglBindAPI");
	}
	#endif

	eglMakeCurrent(mDisplay,mSurface,mSurface,mContext);
	TOADLET_CHECK_EGLERROR("eglMakeCurrent");

	#if defined(TOADLET_HAS_GLESEM)
		if(glesem_glInitialize()==false){
			Error::unknown(Categories::TOADLET_PEEPER,
				"glesem_glInitialize error");
			return false;
		}
	#endif

	return true;
}

bool EGLWindowRenderTarget::destroyContext(){
	if(mDisplay!=EGL_NO_DISPLAY){
		eglMakeCurrent(mDisplay,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);

		if(mContext!=EGL_NO_CONTEXT){
			// This causes a crash in certain situations on the HTC Touch Pro
			//  such as in a WM_CLOSE message, or in some WM_SIZE cases, it seems the context is already destroyed
			// However it appears to also lets us lose the ability to create contexts unless we do this, so its in.
			//  If the crash pops up again, we could try wrapping it with a try/catch(...)
			eglDestroyContext(mDisplay,mContext);
			mContext=EGL_NO_CONTEXT;
		}

		if(mSurface!=EGL_NO_SURFACE){
			eglDestroySurface(mDisplay,mSurface);
			mSurface=EGL_NO_SURFACE;
		}

		eglTerminate(mDisplay);
		mDisplay=EGL_NO_DISPLAY;
	}

	#if defined(TOADLET_HAS_EGL_12)
	if(egl_version>=12){
		eglReleaseThread();
	}
	#endif

	return true;
}

bool EGLWindowRenderTarget::swap(){
	if(mPixmap){
		eglWaitGL();
	}
	else{
		eglSwapBuffers(mDisplay,mSurface);
	}

	return true;
}

int EGLWindowRenderTarget::getWidth() const{
	int width=0;
	eglQuerySurface(mDisplay,mSurface,EGL_WIDTH,&width);
	return width;
}

int EGLWindowRenderTarget::getHeight() const{
	int height=0;
	eglQuerySurface(mDisplay,mSurface,EGL_HEIGHT,&height);
	return height;
}

}
}
