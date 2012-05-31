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

package us.toadlet.peeper;

import javax.microedition.khronos.opengles.*;
import static javax.microedition.khronos.opengles.GL11.*;
import javax.microedition.khronos.egl.*;
import static javax.microedition.khronos.egl.EGL11.*;

public class EGLWindowRenderTarget extends EGLRenderTarget{
	public EGLWindowRenderTarget(){super();}
	public EGLWindowRenderTarget(Object display,Object window,WindowRenderTargetFormat format){createContext(display,window,format,false);}
	public EGLWindowRenderTarget(Object display,Object window,WindowRenderTargetFormat format,boolean pixmap){createContext(display,window,format,pixmap);}
	public void destroy(){destroyContext();}

	public boolean createContext(Object display,Object window,WindowRenderTargetFormat format){return createContext(display,window,format,false);}
	public boolean createContext(Object display,Object window,WindowRenderTargetFormat format,boolean pixmap){
		if(mContext!=EGL_NO_CONTEXT){
			return true;
		}

		mPixmap=pixmap;

		if(display==null){
			display=EGL_DEFAULT_DISPLAY;
		}
		mDisplay=egl.eglGetDisplay(EGL_DEFAULT_DISPLAY);
		if(mDisplay==EGL_NO_DISPLAY){
			System.out.println(
				"error getting display");
			return false;
		}

		int[] version=mSizeArray;
		if(egl.eglInitialize(mDisplay,version)==false){
			System.out.println(
				"eglInitialize error");
			return false;
		}
		egl_version=version[0]*10+version[1];

		System.out.println(
			"CALCULATED EGL VERSION:"+(egl_version/10)+"."+(egl_version%10));

		System.out.println(
			"EGL_VENDOR:"+egl.eglQueryString(mDisplay,EGL_VENDOR));
		System.out.println(
			"EGL_VERSION:"+egl.eglQueryString(mDisplay,EGL_VERSION));
		System.out.println(
			"EGL_EXTENSIONS:"+egl.eglQueryString(mDisplay,EGL_EXTENSIONS));
			
		int pixelFormat=0;
		int redBits=5;
		int greenBits=6;
		int blueBits=5;
		int alphaBits=0;
		int depthBits=16;
		int stencilBits=0;
		int multisamples=0;
		int contextVersion=0;
		if(format!=null){
			pixelFormat=format.getPixelFormat();
			/// @todo: Expose the TextureFormat and use that to get the bits
			//redBits=TextureFormat.getRedBits(pixelFormat);
			//greenBits=TextureFormat.getGreenBits(pixelFormat);
			//blueBits=TextureFormat.getBlueBits(pixelFormat);
			//alphaBits=TextureFormat.getAlphaBits(pixelFormat);
			depthBits=format.getDepthBits();
			stencilBits=format.getStencilBits();
			multisamples=format.getMultisamples();
			contextVersion=format.getFlags();
		}

		if(pixelFormat!=0){
			int Format_SHIFT_TYPES=			8;
			int Format_MASK_TYPES=			0xFF00;
			int Format_TYPE_UINT_5_6_5=		7<<Format_SHIFT_TYPES;
			int Format_TYPE_UINT_5_5_5_1=	8<<Format_SHIFT_TYPES;
			int Format_TYPE_UINT_4_4_4_4=	9<<Format_SHIFT_TYPES;
			if((pixelFormat&Format_MASK_TYPES)==Format_TYPE_UINT_5_6_5){
				redBits=5;greenBits=6;blueBits=5;alphaBits=0;
			}
			else if((pixelFormat&Format_MASK_TYPES)==Format_TYPE_UINT_5_5_5_1){
				redBits=5;greenBits=5;blueBits=5;alphaBits=1;
			}
			else if((pixelFormat&Format_MASK_TYPES)==Format_TYPE_UINT_4_4_4_4){
				redBits=4;greenBits=4;blueBits=4;alphaBits=4;
			}
			else{
				redBits=8;greenBits=8;blueBits=8;alphaBits=0;
			}
		}

		mConfig=chooseEGLConfig(mDisplay,redBits,greenBits,blueBits,alphaBits,depthBits,stencilBits,!pixmap,pixmap,false,multisamples);
		TOADLET_CHECK_EGLERROR("chooseEGLConfig");
		if(mConfig==null){
			System.out.println(
				"error choosing config");
			return false;
		}

		System.out.println(
			"chooseEGLConfig config:"+mConfig);

		if(!pixmap){
			mSurface=egl.eglCreateWindowSurface(mDisplay,mConfig,window,null);
			TOADLET_CHECK_EGLERROR("eglCreateWindowSurface");
		}
		else{
			mSurface=egl.eglCreatePixmapSurface(mDisplay,mConfig,window,null);
			TOADLET_CHECK_EGLERROR("eglCreatePixmapSurface");
		}
		if(mSurface==EGL_NO_SURFACE){
			System.out.println(
				"error creating surface");
			return false;
		}

		int[] configOptions=new int[32];
		int i=0;

		if(contextVersion>0){
			configOptions[i++]=EGL_CONTEXT_CLIENT_VERSION;
			configOptions[i++]=contextVersion;
		}
		
		// Terminate the list with EGL_NONE
		while(i<configOptions.length) configOptions[i++]=EGL_NONE;

		mContext=egl.eglCreateContext(mDisplay,mConfig,EGL_NO_CONTEXT,configOptions);
		TOADLET_CHECK_EGLERROR("eglCreateContext");
		if(mContext==EGL_NO_CONTEXT){
			System.out.println(
				"error creating context");
			return false;
		}

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

		return true;
	}

	public boolean activateAdditionalContext(){return false;}
	public void deactivateAdditionalContext(){}
	
	public boolean swap(){
		if(mPixmap){
			egl.eglWaitGL();
		}
		else{
			egl.eglSwapBuffers(mDisplay,mSurface);
		}
		
		return true;
	}

	public boolean isPrimary(){return true;}
	public boolean isValid(){return mContext!=EGL_NO_CONTEXT;}

	public int getWidth(){
		egl.eglQuerySurface(mDisplay,mSurface,EGL_WIDTH,mSizeArray);
		return mSizeArray[0];
	}
	
	public int getHeight(){
		egl.eglQuerySurface(mDisplay,mSurface,EGL_HEIGHT,mSizeArray);
		return mSizeArray[0];
	}
	
	protected EGLConfig mConfig=null;
	protected boolean mPixmap=false;
	protected int[] mSizeArray=new int[2];
	
	public void TOADLET_CHECK_EGLERROR(String function){
		int error=egl.eglGetError();
		if(error!=EGL_SUCCESS){
			System.out.println(
				("EGL Error in ") + function + ": error=" + error);
		}
	}
}
