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

#include "WGLPBufferSurfaceRenderTarget.h"
#include "../../GLRenderer.h"
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;

namespace toadlet{
namespace peeper{

bool GLPBufferSurfaceRenderTarget_available(GLRenderer *renderer){
	return WGLPBufferSurfaceRenderTarget::available(renderer);
}

SurfaceRenderTarget *new_GLPBufferSurfaceRenderTarget(GLRenderer *renderer){
	return new WGLPBufferSurfaceRenderTarget(renderer);
}

bool WGLPBufferSurfaceRenderTarget::available(GLRenderer *renderer){
	#if defined(TOADLET_HAS_GLEW)
		return WGLEW_ARB_render_texture>0;
	#else
		return false;
	#endif
}

WGLPBufferSurfaceRenderTarget::WGLPBufferSurfaceRenderTarget(GLRenderer *renderer):WGLRenderTarget(),
	mRenderer(NULL),
	mTexture(NULL),
	mPBuffer(NULL),
	mWidth(0),
	mHeight(0),
	mBound(false),
	mInitialized(false)
{
	mRenderer=renderer;
}

WGLPBufferSurfaceRenderTarget::~WGLPBufferSurfaceRenderTarget(){
	destroy();
}

bool WGLPBufferSurfaceRenderTarget::create(){
	// Nothing yet, all done after an attach
	return true;
}

bool WGLPBufferSurfaceRenderTarget::destroy(){
	destroyBuffer();

	if(mBound){
		mBound=false;
		wglReleaseTexImageARB(mPBuffer,WGL_FRONT_LEFT_ARB);
		TOADLET_CHECK_GLERROR("wglReleaseTexImageARB");
	}

	mInitialized=false;

	return true;
}

bool WGLPBufferSurfaceRenderTarget::makeCurrent(){
	if(mBound){
		mBound=false;
		wglReleaseTexImageARB(mPBuffer,WGL_FRONT_LEFT_ARB);
		TOADLET_CHECK_GLERROR("wglReleaseTexImageARB");
	}

	WGLRenderTarget::makeCurrent();

	if(mInitialized==false){
		mRenderer->setDefaultStates();
		mInitialized=true;
	}

	return true;
}

bool WGLPBufferSurfaceRenderTarget::swap(){
	if(mBound==false){
		mBound=true;
		glBindTexture(mTexture->getTarget(),mTexture->getHandle());
		wglBindTexImageARB(mPBuffer,WGL_FRONT_LEFT_ARB);
		TOADLET_CHECK_GLERROR("wglBindTexImageARB");
	}

	return true;
}

bool WGLPBufferSurfaceRenderTarget::remove(Surface::ptr surface){
	// Unimplemented currently
	return false;
}

bool WGLPBufferSurfaceRenderTarget::attach(Surface::ptr surface,Attachment attachment){
	GLTextureMipSurface *gltextureSurface=((GLSurface*)surface->getRootSurface())->castToGLTextureMipSurface();
	mTexture=gltextureSurface->getTexture();

	if((mTexture->getFormat()&Texture::Format_BIT_DEPTH)>0){
		Error::invalidParameters(Categories::TOADLET_PEEPER,
			"Format_BIT_DEPTH not available for pbuffers");
		return false;
	}

	createBuffer();

	return true;
}

bool WGLPBufferSurfaceRenderTarget::createBuffer(){
	destroyBuffer();

	int width=mTexture->getWidth();
	int height=mTexture->getHeight();

	HDC hdc=wglGetCurrentDC();
	HGLRC context=wglGetCurrentContext();

	int bindType=WGL_BIND_TO_TEXTURE_RGB_ARB;
	int pixelType=WGL_TYPE_RGBA_ARB;
	int texFormat=WGL_TEXTURE_RGB_ARB;

	int format=mTexture->getFormat();
	int redBits=ImageFormatConversion::getRedBits(format);
	int greenBits=ImageFormatConversion::getGreenBits(format);
	int blueBits=ImageFormatConversion::getBlueBits(format);
	int depthBits=16;

	int iAttributes[]={
		WGL_COLOR_BITS_ARB,redBits+greenBits+blueBits,
		WGL_ALPHA_BITS_ARB,0,
		WGL_STENCIL_BITS_ARB,0,
		WGL_DEPTH_BITS_ARB,depthBits,
		WGL_DRAW_TO_PBUFFER_ARB,GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB,GL_FALSE,
		WGL_PIXEL_TYPE_ARB,pixelType,
		WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB,
		bindType,true,// bindType must be last, since it could be zero if we were not binding
		0,
	};

	float fAttributes[]={0,0};

	int pAttributes[]={ 
		WGL_TEXTURE_FORMAT_ARB,texFormat,
		WGL_TEXTURE_TARGET_ARB,WGL_TEXTURE_2D_ARB,
		0 
	};

	int wglformat=0;
	unsigned int count=0;

	wglChoosePixelFormatARB(hdc,iAttributes,fAttributes,1,&wglformat,&count);

	if(count==0){
		Error::unknown(Categories::TOADLET_PEEPER,
			"wglChoosePixelFormatARB() failed");
		return false;
	}

	// Analyse pixel format
	const int piAttributes[]={
		WGL_RED_BITS_ARB,WGL_GREEN_BITS_ARB,WGL_BLUE_BITS_ARB,WGL_ALPHA_BITS_ARB,
		WGL_DEPTH_BITS_ARB,WGL_STENCIL_BITS_ARB
	};

	int piValues[sizeof(piAttributes)/sizeof(const int)];
	wglGetPixelFormatAttribivARB(hdc,wglformat,0,sizeof(piAttributes)/sizeof(const int),piAttributes,piValues);

	Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
		String("Format RGBA=")+
		piValues[0] + "," + piValues[1] + "," + piValues[2] + "," + piValues[3] + " Depth=" +
		piValues[4] + " Stencil=" + piValues[5] + " Width=" + width + " Height=" + height);
	
	mPBuffer=wglCreatePbufferARB(hdc,wglformat,width,height,pAttributes);
	if(!mPBuffer){
		Error::unknown(Categories::TOADLET_PEEPER,
			"wglCreatePbufferARB() failed");
		return false;
	}

	mDC=wglGetPbufferDCARB(mPBuffer);
	if(!mDC){
		destroyBuffer();
		Error::unknown(Categories::TOADLET_PEEPER,
			"wglGetPbufferDCARB() failed");
		return false;
	}
		
	mGLRC=wglCreateContext(mDC);
	if(!mGLRC){
		destroyBuffer();
		Error::unknown(Categories::TOADLET_PEEPER,
			"wglCreateContext() failed");
		return false;
	}

	if(!wglShareLists(context,mGLRC)){
		destroyBuffer();
		Error::unknown(Categories::TOADLET_PEEPER,
			"wglShareLists() failed");
		return false;
	}

	wglQueryPbufferARB(mPBuffer,WGL_PBUFFER_WIDTH_ARB,(int*)&mWidth);
	wglQueryPbufferARB(mPBuffer,WGL_PBUFFER_HEIGHT_ARB,(int*)&mHeight);
	if(mWidth!=width || mHeight!=height){
		Error::unknown(Categories::TOADLET_PEEPER,
			"width or height not as expected");
		return false;
	}

	return true;
}

bool WGLPBufferSurfaceRenderTarget::destroyBuffer(){
	if(mGLRC!=0){
		wglDeleteContext(mGLRC);
		mGLRC=0;
	}
	if(mDC!=0){
		wglReleasePbufferDCARB(mPBuffer,mDC);
		mDC=0;
	}
	if(mPBuffer!=0){
		wglDestroyPbufferARB(mPBuffer);
		mPBuffer=0;
	}
	return true;
}

}
}
