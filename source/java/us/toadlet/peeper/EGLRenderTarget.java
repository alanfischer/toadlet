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

public abstract class EGLRenderTarget extends GLRenderTarget{
	final static int EGL_BIND_TO_TEXTURE_RGB=0x3039;
	final static int EGL_CONTEXT_CLIENT_VERSION=0x3098;

	public EGL10 egl;
	public EGL11 egl11;
	public int egl_version=0;

	public EGLRenderTarget(){
		super(0,false);
		egl=(EGL10)EGLContext.getEGL();
		try{egl11=(EGL11)EGLContext.getEGL();}catch(Exception ex){}
	}

	public EGLDisplay getEGLDisplay(){return mDisplay;}
	public EGLSurface getEGLSurface(){return mSurface;}
	public EGLContext getEGLContext(){return mContext;}

	public boolean activate(){
		return egl.eglMakeCurrent(mDisplay,mSurface,mSurface,mContext);
	}
	
	public boolean deactivate(){
		return egl.eglMakeCurrent(mDisplay,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);
	}

	public void reset(){}

	protected EGLConfig chooseEGLConfig(EGLDisplay display,int redBits,int greenBits,int blueBits,int alphaBits,int depthBits,int stencilBits,boolean window,boolean pixmap,boolean pbuffer,int fsaaCount){
		int[] configOptions=new int[32];
		int i=0;

		// Select default configuration 
		configOptions[i++]=EGL_LEVEL;
		configOptions[i++]=0;

		if(redBits>0){
			configOptions[i++]=EGL_RED_SIZE;
			configOptions[i++]=redBits;
		}

		if(greenBits>0){
			configOptions[i++]=EGL_GREEN_SIZE;
			configOptions[i++]=greenBits;
		}

		if(blueBits>0){
			configOptions[i++]=EGL_BLUE_SIZE;
			configOptions[i++]=blueBits;
		}

		if(alphaBits>0){
			configOptions[i++]=EGL_ALPHA_SIZE;
			configOptions[i++]=alphaBits;
		}

		if(depthBits>0){
			configOptions[i++]=EGL_DEPTH_SIZE;
			configOptions[i++]=depthBits;
		}

		if(stencilBits>0){
			configOptions[i++]=EGL_STENCIL_SIZE;
			configOptions[i++]=stencilBits;
		}

		// Use this if you want to only allow configurations with native rendering to a surface
		if(false){
			configOptions[i++]=EGL_NATIVE_RENDERABLE;
			configOptions[i++]=1;
		}

		if(window || pixmap || pbuffer){
			configOptions[i++]=EGL_SURFACE_TYPE;
			configOptions[i]=0;
			if(window){
				configOptions[i]|=EGL_WINDOW_BIT;
			}
			if(pixmap){
				configOptions[i]|=EGL_PIXMAP_BIT;
			}
			if(pbuffer){
				configOptions[i]|=EGL_PBUFFER_BIT;
			}
			i++;
		}

		if(egl_version>=11 && pbuffer){
			configOptions[i++]=EGL_BIND_TO_TEXTURE_RGB;
			configOptions[i++]=1;
		}

		// Append number of number buffers depending on FSAA mode selected
		switch(fsaaCount){
			case 1:
				configOptions[i++]=EGL_SAMPLE_BUFFERS;
				configOptions[i++]=1;
				configOptions[i++]=EGL_SAMPLES;
				configOptions[i++]=2;
			break;
			case 2:
				configOptions[i++]=EGL_SAMPLE_BUFFERS;
				configOptions[i++]=1;
				configOptions[i++]=EGL_SAMPLES;
				configOptions[i++]=4;
			break;
			default:
				configOptions[i++]=EGL_SAMPLE_BUFFERS;
				configOptions[i++]=0;
			break;
		}

		// Terminate the list with EGL_NONE
		while(i<configOptions.length) configOptions[i++]=EGL_NONE;

		return chooseEGLConfigFromConfigOptions(display,configOptions);
	}

	protected EGLConfig chooseEGLConfigFromConfigOptions(EGLDisplay display,int[] configOptions){
		if(configOptions==null){
			return null;
		}

		EGLConfig result=null;

		{
			int[] numConfigs=new int[1];
			int i=0;
			EGLConfig[] allConfigs=null;

			egl.eglGetConfigs(display,null,0,numConfigs);
			if(egl.eglGetError()!=EGL_SUCCESS || numConfigs[0]<=0){
				return null;
			}

			allConfigs=new EGLConfig[numConfigs[0]];

			i=numConfigs[0];

			egl.eglChooseConfig(display,configOptions,allConfigs,i,numConfigs);
			if(egl.eglGetError()!=EGL_SUCCESS || numConfigs[0]<=0){
				return null;
			}

			result=allConfigs[0];

			for(i=1;i<numConfigs[0];i++){
				result=pickEGLConfig(display,result,allConfigs[i]);
			}
		}

		return result;
	}

	protected EGLConfig pickEGLConfig(EGLDisplay display,EGLConfig oldConfig,EGLConfig newConfig){
		// Check for better colorbuffer type
		int[] oldBufType=new int[1];
		int[] newBufType=new int[1];

		if(egl_version>=12){
			if(!egl.eglGetConfigAttrib(display,newConfig,EGL_COLOR_BUFFER_TYPE,newBufType)){return oldConfig;}

			if(!egl.eglGetConfigAttrib(display,oldConfig,EGL_COLOR_BUFFER_TYPE,oldBufType)){return newConfig;}

			if(oldBufType[0]==EGL_RGB_BUFFER && newBufType[0]!=oldBufType[0]){return oldConfig;}

			if(newBufType[0]==EGL_RGB_BUFFER && newBufType[0]!=oldBufType[0]){return newConfig;}
		}

		// Check for less colorBits (including alpha)
		{
			int oldColorBits=0;
			int newColorBits=0;
			int[] temp=new int[1];
			int newRGB=0;
			int oldRGB=0;
			
			if(!egl.eglGetConfigAttrib(display,newConfig,EGL_RED_SIZE,temp)){return oldConfig;}
			newColorBits+=temp[0];
			
			if(!egl.eglGetConfigAttrib(display,newConfig,EGL_GREEN_SIZE,temp)){return oldConfig;}
			newColorBits+=temp[0];
			
			if(!egl.eglGetConfigAttrib(display,newConfig,EGL_BLUE_SIZE,temp)){return oldConfig;}
			newColorBits+=temp[0];
			
			if(egl_version>=12){
				if(!egl.eglGetConfigAttrib(display,newConfig,EGL_LUMINANCE_SIZE,temp)){return oldConfig;}
				newColorBits+=temp[0];
			}
			newRGB=newColorBits;

			if(!egl.eglGetConfigAttrib(display,newConfig,EGL_ALPHA_SIZE,temp)){return oldConfig;}
			newColorBits+=temp[0];

			if(!egl.eglGetConfigAttrib(display,oldConfig,EGL_RED_SIZE,temp)){return newConfig;}
			oldColorBits+=temp[0];

			if(!egl.eglGetConfigAttrib(display,oldConfig,EGL_GREEN_SIZE,temp)){return newConfig;}
			oldColorBits+=temp[0];
			
			if(!egl.eglGetConfigAttrib(display,oldConfig,EGL_BLUE_SIZE,temp)){return newConfig;}
			oldColorBits+=temp[0];

			if(egl_version>=12){
				if(!egl.eglGetConfigAttrib(display,oldConfig,EGL_LUMINANCE_SIZE,temp)){return newConfig;}
				oldColorBits+=temp[0];
			}
			oldRGB=oldColorBits;
			
			if(!egl.eglGetConfigAttrib(display,oldConfig,EGL_ALPHA_SIZE,temp)){return newConfig;}
			oldColorBits+=temp[0];

			// First check total rgbal-bits then just rgb - favor infimum
			if(newColorBits<oldColorBits || newRGB<oldRGB){return newConfig;}
		}

		// Rest of the eglChooseConfig sorting is ok, returning the old config
		return oldConfig;
	}

	protected EGLDisplay mDisplay=EGL_NO_DISPLAY;
	protected EGLSurface mSurface=EGL_NO_SURFACE;
	protected EGLContext mContext=EGL_NO_CONTEXT;
}
