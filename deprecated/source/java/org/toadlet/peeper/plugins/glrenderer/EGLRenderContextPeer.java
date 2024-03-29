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

package org.toadlet.peeper.plugins.glrenderer;

#include <org/toadlet/peeper/Types.h>
#include <org/toadlet/peeper/plugins/glrenderer/GLIncludes.h>

import org.toadlet.egg.Categories;
import org.toadlet.egg.Error;
import org.toadlet.egg.Logger;
import org.toadlet.egg.image.ImageFormatConversion;
import org.toadlet.peeper.*;
import javax.microedition.khronos.opengles.*;
import static javax.microedition.khronos.opengles.GL11.*;
import javax.microedition.khronos.egl.*;
import static javax.microedition.khronos.egl.EGL11.*;

public class EGLRenderContextPeer extends EGLRenderTargetPeer{
	public EGLRenderContextPeer(){super();}
	public EGLRenderContextPeer(Object nativeSurface,Visual visual){createContext(nativeSurface,visual,false);}
	public EGLRenderContextPeer(Object nativeSurface,Visual visual,boolean pixmap){createContext(nativeSurface,visual,pixmap);}
	
	public void destroy(){
		destroyContext();
	}

	public boolean createContext(Object nativeSurface,Visual visual){return createContext(nativeSurface,visual,false);}
	public boolean createContext(Object nativeSurface,Visual visual,boolean pixmap){
		if(mContext!=EGL_NO_CONTEXT){
			return true;
		}

		mPixmap=pixmap;

		#if defined(TOADLET_HAS_GLESEM)
			if(!glesem_getInitialized()){
				glesem_init(GLESEM_ACCELERATED_IF_AVAILABLE);
			}
		#endif

		mDisplay=egl.eglGetDisplay(EGL_DEFAULT_DISPLAY);
		if(mDisplay==EGL_NO_DISPLAY){
			Error.unknown(Categories.TOADLET_PEEPER,
				"error getting display");
			return false;
		}

		#if defined(TOADLET_HAS_GLESEM)
			if(glesem_eglInitialize(mDisplay)==false){
				Error.unknown(Categories::TOADLET_PEEPER,
					"glesem_eglInitialize error");
				return false;
			}
			egl_version=glesem_egl_majorVersion*10+glesem_egl_minorVersion;
		#else
			int[] version=new int[2];
			if(egl.eglInitialize(mDisplay,version)==false){
				Error.unknown(Categories.TOADLET_PEEPER,
					"eglInitialize error");
				return false;
			}
			egl_version=version[0]*10+version[1];
		#endif
		Logger.alert(Categories.TOADLET_PEEPER,
			"CALCULATED EGL VERSION:"+(egl_version/10)+"."+(egl_version%10));

		// Crashes on Android 1.1
		#if !defined(TOADLET_PLATFORM_ANDROID)
			Logger.alert(Categories.TOADLET_PEEPER,
				"EGL_VENDOR:"+egl.eglQueryString(mDisplay,EGL_VENDOR));
			Logger.alert(Categories.TOADLET_PEEPER,
				"EGL_VERSION:"+egl.eglQueryString(mDisplay,EGL_VERSION));
			Logger.alert(Categories.TOADLET_PEEPER,
				"EGL_EXTENSIONS:"+egl.eglQueryString(mDisplay,EGL_EXTENSIONS));
		#endif

		int format=visual.pixelFormat;
		int redBits=ImageFormatConversion.getRedBits(format);
		int greenBits=ImageFormatConversion.getGreenBits(format);
		int blueBits=ImageFormatConversion.getBlueBits(format);
		int alphaBits=ImageFormatConversion.getAlphaBits(format);
		int depthBits=visual.depthBits;
		int stencilBits=visual.stencilBits;
		int multisamples=visual.multisamples;

		mConfig=chooseEGLConfig(mDisplay,redBits,greenBits,blueBits,alphaBits,depthBits,stencilBits,!pixmap,pixmap,false,visual.multisamples);
		TOADLET_CHECK_EGLERROR("chooseEGLConfig");

		Logger.debug(Categories.TOADLET_PEEPER,
			"chooseEGLConfig config:"+mConfig);

		if(!pixmap){
			mSurface=egl.eglCreateWindowSurface(mDisplay,mConfig,nativeSurface,null);
			TOADLET_CHECK_EGLERROR("eglCreateWindowSurface");
		}
		else{
			mSurface=egl.eglCreatePixmapSurface(mDisplay,mConfig,nativeSurface,null);
			TOADLET_CHECK_EGLERROR("eglCreatePixmapSurface");
		}

		if(mSurface==EGL_NO_SURFACE){
			Error.unknown(Categories.TOADLET_PEEPER,
				"error creating surface");
			return false;
		}

		mContext=egl.eglCreateContext(mDisplay,mConfig,EGL_NO_CONTEXT,null);
		TOADLET_CHECK_EGLERROR("eglCreateContext");

		if(mContext==EGL_NO_CONTEXT){
			Error.unknown(Categories.TOADLET_PEEPER,
				"error creating context");
			return false;
		}

		#if defined(TOADLET_HAS_EGL_12)
		if(egl_version>=12){
			egl.eglBindAPI(EGL_OPENGL_ES_API);
			TOADLET_CHECK_EGLERROR("eglBindAPI");
		}
		#endif

		#if defined(TOADLET_HAS_GLESEM)
			if(glesem_glInitialize()==false){
				Error::unknown(Categories::TOADLET_PEEPER,
					"glesem_glInitialize error");
				return false;
			}
		#endif

		egl.eglMakeCurrent(mDisplay,mSurface,mSurface,mContext);
		TOADLET_CHECK_EGLERROR("eglMakeCurrent");

		return true;
	}

	public boolean destroyContext(){
		if(mDisplay!=EGL_NO_DISPLAY){
			egl.eglMakeCurrent(mDisplay,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);

			if(mContext!=EGL_NO_CONTEXT){
				egl.eglDestroyContext(mDisplay,mContext);
				mContext=EGL_NO_CONTEXT;
			}

			if(mSurface!=EGL_NO_SURFACE){
				egl.eglDestroySurface(mDisplay,mSurface);
				mSurface=EGL_NO_SURFACE;
			}

			egl.eglTerminate(mDisplay);
			mDisplay=EGL_NO_DISPLAY;
		}

		#if defined(TOADLET_HAS_EGL_12)
		if(egl_version>=12){
			egl.eglReleaseThread();
		}
		#endif

		return true;
	}

	public int getWidth(){
		int[] width=new int[1];
		egl.eglQuerySurface(mDisplay,mSurface,EGL_WIDTH,width);
		return width[0];
	}
	
	public int getHeight(){
		int[] height=new int[1];
		egl.eglQuerySurface(mDisplay,mSurface,EGL_HEIGHT,height);
		return height[0];
	}
	
	public void swap(){
		if(mPixmap){
			egl.eglWaitGL();
		}
		else{
			egl.eglSwapBuffers(mDisplay,mSurface);
		}
	}

	public Texture.Peer castToTexturePeer(){return null;}
	
	public boolean isValid(){return mContext!=EGL_NO_CONTEXT;}

	protected EGLConfig mConfig=null;
	protected boolean mPixmap=false;
}
