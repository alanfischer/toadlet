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

#include "EGLPBufferRenderTexturePeer.h"
#include "EGLRenderContextPeer.h"
#include "../../GLRenderer.h"
#include "../../../../RenderContext.h"
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;

namespace toadlet{
namespace peeper{

bool GLPBufferRenderTexturePeer_available(GLRenderer *renderer){
	return EGLPBufferRenderTexturePeer::available(renderer);
}

GLTexturePeer *new_GLPBufferRenderTexturePeer(GLRenderer *renderer,RenderTexture *texture){
	return new EGLPBufferRenderTexturePeer(renderer,texture);
}

bool EGLPBufferRenderTexturePeer::available(GLRenderer *renderer){
	return ((EGLRenderContextPeer*)renderer->getRenderContext()->internal_getRenderTargetPeer())->egl_version>=11;
}

EGLPBufferRenderTexturePeer::EGLPBufferRenderTexturePeer(GLRenderer *renderer,RenderTexture *texture):EGLRenderTargetPeer(),GLTexturePeer(renderer,texture){
	egl_version=((EGLRenderContextPeer*)renderer->getRenderContext()->internal_getRenderTargetPeer())->egl_version;
	mTexture=texture;
	mWidth=0;
	mHeight=0;
	mBound=false;

	if((texture->getFormat()&Image::Format_BIT_DEPTH)>0){
		Error::invalidParameters(Categories::TOADLET_PEEPER,
			"Format_BIT_DEPTH not available for pbuffers");
		return;
	}

	createBuffer();

	Math::setMatrix4x4FromScale(textureMatrix,Vector3(Math::ONE,-Math::ONE,Math::ONE));
	Math::setMatrix4x4FromTranslate(textureMatrix,Vector3(0,Math::ONE,0));
}

EGLPBufferRenderTexturePeer::~EGLPBufferRenderTexturePeer(){
	if(mBound){
		eglReleaseTexImage(mDisplay,mSurface,EGL_BACK_BUFFER);
		TOADLET_CHECK_EGLERROR("eglReleaseTexImage");
	}

	destroyBuffer();
}

void EGLPBufferRenderTexturePeer::makeCurrent(){
	if(mBound){
		mBound=false;
		eglReleaseTexImage(mDisplay,mSurface,EGL_BACK_BUFFER);
		TOADLET_CHECK_EGLERROR("eglReleaseTexImage");
	}

	EGLRenderTargetPeer::makeCurrent();

	if(mInitialized==false){
		mRenderer->setDefaultStates();
		mInitialized=true;
	}
}

void EGLPBufferRenderTexturePeer::swap(){
	if(mBound==false){
		mBound=true;
		glBindTexture(textureTarget,textureHandle);
		eglBindTexImage(mDisplay,mSurface,EGL_BACK_BUFFER);
		TOADLET_CHECK_EGLERROR("eglBindTexImage");
	}
}

bool EGLPBufferRenderTexturePeer::createBuffer(){
	int width=((Texture*)mTexture)->getWidth();
	int height=((Texture*)mTexture)->getHeight();

	mDisplay=eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if(mDisplay==EGL_NO_DISPLAY){
		Error::unknown(Categories::TOADLET_PEEPER,
			"Error getting display from eglGetDisplay");
		return false;
	}

	int format=mTexture->getFormat();
	int redBits=ImageFormatConversion::getRedBits(format);
	int greenBits=ImageFormatConversion::getGreenBits(format);
	int blueBits=ImageFormatConversion::getBlueBits(format);
	int alphaBits=ImageFormatConversion::getAlphaBits(format);
	int depthBits=mTexture->hasDepthBuffer()?16:0;

	EGLConfig config=chooseEGLConfig(mDisplay,redBits,greenBits,blueBits,alphaBits,depthBits,0,false,false,true,false);
	TOADLET_CHECK_EGLERROR("chooseEGLConfig");

	int attribList[]={
		EGL_WIDTH,width,
		EGL_HEIGHT,height,
		EGL_TEXTURE_TARGET,EGL_TEXTURE_2D,
		EGL_TEXTURE_FORMAT,EGL_TEXTURE_RGB,
		EGL_NONE
	};

	mSurface=eglCreatePbufferSurface(mDisplay,config,attribList);
	TOADLET_CHECK_EGLERROR("eglCreatePbufferSurface");

	if(mSurface==EGL_NO_SURFACE){
		destroyBuffer();

		Error::unknown(Categories::TOADLET_PEEPER,
			"Error in eglCreatePbufferSurface");
		return false;
	}

	EGLContext context=eglGetCurrentContext();

	mContext=eglCreateContext(mDisplay,config,context,NULL);
	TOADLET_CHECK_EGLERROR("eglCreateContext");

	if(mContext==EGL_NO_CONTEXT){
		destroyBuffer();

		Error::unknown(Categories::TOADLET_PEEPER,
			"Error in eglCreateContext");
		return false;
	}

	if(eglBindAPI!=NULL){
		eglBindAPI(EGL_OPENGL_ES_API);
		TOADLET_CHECK_EGLERROR("eglBindAPI");
	}

	eglQuerySurface(mDisplay,mSurface,EGL_WIDTH,(int*)&mWidth);
	eglQuerySurface(mDisplay,mSurface,EGL_HEIGHT,(int*)&mHeight);
	if(mWidth!=width || mHeight!=height){
		Error::unknown(Categories::TOADLET_PEEPER,
			"EGLPBufferRenderTexturePeer::createBuffer: width or height not as expected");
		return false;
	}

	return true;
}

bool EGLPBufferRenderTexturePeer::destroyBuffer(){
	Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
		"Destroying context");

	if(eglMakeCurrent==NULL){
		return true;
	}

	if(mDisplay!=EGL_NO_DISPLAY){
		eglMakeCurrent(mDisplay,EGL_NO_SURFACE,EGL_NO_SURFACE,EGL_NO_CONTEXT);

		if(mContext!=EGL_NO_CONTEXT){
			eglDestroyContext(mDisplay,mContext);
			mContext=EGL_NO_CONTEXT;
		}

		if(mSurface!=EGL_NO_SURFACE){
			eglDestroySurface(mDisplay,mSurface);
			mSurface=EGL_NO_SURFACE;
		}
	}

	return true;
}

int EGLPBufferRenderTexturePeer::getWidth() const{
	return mWidth;
}

int EGLPBufferRenderTexturePeer::getHeight() const{
	return mHeight;
}

}
}

