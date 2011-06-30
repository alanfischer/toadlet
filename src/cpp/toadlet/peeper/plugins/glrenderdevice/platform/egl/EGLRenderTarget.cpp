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
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>

namespace toadlet{
namespace peeper{

EGLRenderTarget::EGLRenderTarget():GLRenderTarget(),
	egl_version(0),

	mDisplay(EGL_NO_DISPLAY),
	mSurface(EGL_NO_SURFACE),
	mContext(EGL_NO_CONTEXT)
{}

bool EGLRenderTarget::activate(){
	return eglMakeCurrent(mDisplay,mSurface,mSurface,mContext)==EGL_TRUE;
}

bool EGLRenderTarget::deactivate(){
	return eglMakeCurrent(mDisplay,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT)==EGL_TRUE;
}

EGLConfig EGLRenderTarget::chooseEGLConfig(EGLDisplay display,int redBits,int greenBits,int blueBits,int alphaBits,int depthBits,int stencilBits,bool window,bool pixmap,bool pbuffer,int fsaaCount){
    EGLint configOptions[32];
	int i=0;

	Logger::debug(Categories::TOADLET_PEEPER,
		String("Choosing config for color:")+redBits+","+greenBits+","+blueBits+","+alphaBits+" depth:"+depthBits+" stencil:"+stencilBits);

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

	#if defined(TOADLET_HAS_EGL_11)
	if(egl_version>=11 && pbuffer){
		configOptions[i++]=EGL_BIND_TO_TEXTURE_RGB;
		configOptions[i++]=EGL_TRUE;
	}
	#endif

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
	configOptions[i++]=EGL_NONE;

	EGLConfig config=chooseEGLConfigFromConfigOptions(display,configOptions);

	return config;
}

EGLConfig EGLRenderTarget::chooseEGLConfigFromConfigOptions(EGLDisplay display,const EGLint *configOptions){
	if(configOptions==NULL){
		return NULL;
	}

	EGLConfig result=0;

	{
		EGLint numConfigs=0;
		EGLint i=0;
		EGLConfig *allConfigs=NULL;

		eglGetConfigs(display,NULL,0,&numConfigs);
		if(eglGetError()!=EGL_SUCCESS || numConfigs<=0){
			return NULL;
		}

		allConfigs=new EGLConfig[numConfigs];

		i=numConfigs;

		eglChooseConfig(display,configOptions,allConfigs,i,&numConfigs);
		if(eglGetError()!=EGL_SUCCESS || numConfigs<=0){
			Logger::debug(String("no configs from eglChooseConfig:")+numConfigs);
			delete[] allConfigs;
			return NULL;
		}

		result=allConfigs[0];

		for(i=1;i<numConfigs;i++){
			result=pickEGLConfig(display,result,allConfigs[i]);
		}

		delete[] allConfigs;
	}

	return result;
}

EGLConfig EGLRenderTarget::pickEGLConfig(EGLDisplay display,EGLConfig oldConfig,EGLConfig newConfig){
	// Check for better colorbuffer type
	EGLint oldBufType;
	EGLint newBufType;

	#if defined(TOADLET_HAS_EGL_12)
	if(egl_version>=12){
		if(!eglGetConfigAttrib(display,newConfig,EGL_COLOR_BUFFER_TYPE,&newBufType)){return oldConfig;}

		if(!eglGetConfigAttrib(display,oldConfig,EGL_COLOR_BUFFER_TYPE,&oldBufType)){return newConfig;}

		if(oldBufType==EGL_RGB_BUFFER && newBufType!=oldBufType){return oldConfig;}

		if(newBufType==EGL_RGB_BUFFER && newBufType!=oldBufType){return newConfig;}
	}
	#endif

	// Check for less colorBits (including alpha)
	{
		EGLint oldColorBits = 0;
		EGLint newColorBits = 0;
		EGLint temp			= 0;
		EGLint newRGB		= 0;
		EGLint oldRGB		= 0;
		
		if(!eglGetConfigAttrib(display,newConfig,EGL_RED_SIZE,&temp)){return oldConfig;}
		newColorBits+=temp;
		
		if(!eglGetConfigAttrib(display,newConfig,EGL_GREEN_SIZE,&temp)){return oldConfig;}
		newColorBits+=temp;
		
		if(!eglGetConfigAttrib(display,newConfig,EGL_BLUE_SIZE,&temp)){return oldConfig;}
		newColorBits+=temp;

		#if defined(TOADLET_HAS_EGL_12)
		if(egl_version>=12){
			if(!eglGetConfigAttrib(display,newConfig,EGL_LUMINANCE_SIZE,&temp)){return oldConfig;}
			newColorBits+=temp;
		}
		#endif
		newRGB=newColorBits;

		if(!eglGetConfigAttrib(display,newConfig,EGL_ALPHA_SIZE,&temp)){return oldConfig;}
		newColorBits+=temp;

		if(!eglGetConfigAttrib(display,oldConfig,EGL_RED_SIZE,&temp)){return newConfig;}
		oldColorBits+=temp;

		if(!eglGetConfigAttrib(display,oldConfig,EGL_GREEN_SIZE,&temp)){return newConfig;}
		oldColorBits+=temp;
		
		if(!eglGetConfigAttrib(display,oldConfig,EGL_BLUE_SIZE,&temp)){return newConfig;}
		oldColorBits+=temp;

		#if defined(TOADLET_HAS_EGL_12)
		if(egl_version>=12){
			if(!eglGetConfigAttrib(display,oldConfig,EGL_LUMINANCE_SIZE,&temp)){return newConfig;}
			oldColorBits+=temp;
		}
		#endif
		oldRGB=oldColorBits;
		
		if(!eglGetConfigAttrib(display,oldConfig,EGL_ALPHA_SIZE,&temp)){return newConfig;}
		oldColorBits+=temp;
		
		// First check total rgbal-bits then just rgb - favor infimum
		if(newColorBits<oldColorBits || newRGB<oldRGB){return newConfig;}
	}

	// Rest of the eglChooseConfig sorting is ok, returning the old config
	return oldConfig;
}

}
}
