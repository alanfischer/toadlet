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

#include "EGLWindowRenderTarget.h"
#include <toadlet/peeper/TextureFormat.h>

namespace toadlet{
namespace peeper{

TOADLET_C_API RenderTarget *new_EGLWindowRenderTarget(void *display,void *window,WindowRenderTargetFormat *format){
	return new EGLWindowRenderTarget(display,window,format);
}

EGLWindowRenderTarget::EGLWindowRenderTarget():
	mConfig(0),
	mPixmap(false)
{}

EGLWindowRenderTarget::EGLWindowRenderTarget(void *display,void *window,WindowRenderTargetFormat *format,bool pixmap):
	mConfig(0),
	mPixmap(false)
{
	#if defined(TOADLET_HAS_GLESEM)
		int initialized=glesem_getInitialized();
		if(!initialized){
			Log::alert("initializing glesem");

			int glesemInitResult=glesem_init(GLESEM_ACCELERATED_IF_AVAILABLE);
			if(glesemInitResult<1){
				Error::unknown(Categories::TOADLET_PEEPER,
					"initial glesem_init failed");
				return;
			}
		}
	#endif

	bool result=createContext(display,window,format,pixmap);

	#if defined(TOADLET_HAS_GLESEM)
		if(result==false && !initialized){
			Log::alert("trying software glesem");

			int glesemInitResult=glesem_init(GLESEM_ACCELERATED_NO);
			if(glesemInitResult<1){
				Error::unknown(Categories::TOADLET_PEEPER,
					"software glesem_init failed");
				return;
			}
		}

		result=createContext(display,window,format,pixmap);
	#endif
}

void EGLWindowRenderTarget::destroy(){
	destroyContext();
	
	BaseResource::destroy();
}

bool EGLWindowRenderTarget::createContext(void *display,void *window,WindowRenderTargetFormat *format,bool pixmap){
	#if defined(TOADLET_PLATFORM_WIN32)
		if(display==NULL){
		if(display==NULL){
			display=GetDC((HWND)window);
		}
	#else
		if(display==NULL){
			display=(EGLDisplay)EGL_DEFAULT_DISPLAY;
		}
	#endif

	if(mContext!=EGL_NO_CONTEXT){
		return true;
	}
	
	// Seems like software egl doesn't want a display
	#if defined(TOADLET_HAS_GLESEM)
		if(glesem_getAccelerated()==false){
			display=NULL;
		}
	#endif

	Log::debug(String("creating EGL context for ")+(pixmap?"pixmap":"window"));

	mPixmap=pixmap;

	mDisplay=eglGetDisplay((NativeDisplayType)display);
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
			Error::unknown(Categories::TOADLET_PEEPER,
				"eglInitialize error");
			return false;
		}
		egl_version=version[0]*10+version[1];
	#endif
	Log::alert(Categories::TOADLET_PEEPER,
		String("CALCULATED EGL VERSION:")+(egl_version/10)+"."+(egl_version%10));

	Log::alert(Categories::TOADLET_PEEPER,
		String("EGL_VENDOR:")+eglQueryString(mDisplay,EGL_VENDOR));
	Log::alert(Categories::TOADLET_PEEPER,
		String("EGL_VERSION:")+eglQueryString(mDisplay,EGL_VERSION));
	Log::alert(Categories::TOADLET_PEEPER,
		String("EGL_EXTENSIONS:")+eglQueryString(mDisplay,EGL_EXTENSIONS));

	int pixelFormat=format->pixelFormat;
	int redBits=TextureFormat::getRedBits(pixelFormat);
	int greenBits=TextureFormat::getGreenBits(pixelFormat);
	int blueBits=TextureFormat::getBlueBits(pixelFormat);
	int alphaBits=TextureFormat::getAlphaBits(pixelFormat);
	int depthBits=format->depthBits;
	int stencilBits=format->stencilBits;
	int multisamples=format->multisamples;
	int contextVersion=format->flags;

	mConfig=chooseEGLConfig(mDisplay,redBits,greenBits,blueBits,alphaBits,depthBits,stencilBits,!pixmap,pixmap,false,multisamples);
	TOADLET_CHECK_EGLERROR("chooseEGLConfig");
	if(mConfig==0){
		Error::unknown(Categories::TOADLET_PEEPER,
			"error choosing config");
		return false;
	}

	Log::debug(Categories::TOADLET_PEEPER,
		String("chooseEGLConfig config:")+(int)mConfig);
		
	if(!pixmap){
		mSurface=eglCreateWindowSurface(mDisplay,mConfig,(NativeWindowType)window,NULL);
		TOADLET_CHECK_EGLERROR("eglCreateWindowSurface");
	}
	else{
		mSurface=eglCreatePixmapSurface(mDisplay,mConfig,(EGLNativePixmapType)window,NULL);
		TOADLET_CHECK_EGLERROR("eglCreatePixmapSurface");
	}
	if(mSurface==EGL_NO_SURFACE){
		Error::unknown(Categories::TOADLET_PEEPER,
			"error creating surface");
		return false;
	}

    EGLint configOptions[32];
	int i=0;

	if(contextVersion>0){
		configOptions[i++]=EGL_CONTEXT_CLIENT_VERSION;
		configOptions[i++]=contextVersion;
	}
	
	// Terminate the list with EGL_NONE
	configOptions[i++]=EGL_NONE;

	mContext=eglCreateContext(mDisplay,mConfig,EGL_NO_CONTEXT,configOptions);
	TOADLET_CHECK_EGLERROR("eglCreateContext");
	
	if(mContext==EGL_NO_CONTEXT){
		Error::unknown(Categories::TOADLET_PEEPER,
			"error creating context");
		return false;
	}

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
		Log::debug("destroying EGL context");

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
