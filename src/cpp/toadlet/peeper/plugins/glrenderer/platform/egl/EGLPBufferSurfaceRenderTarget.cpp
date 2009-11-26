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

#include "EGLPBufferSurfaceRenderTarget.h"
#include "../../GLRenderer.h"
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;

namespace toadlet{
namespace peeper{

bool GLPBufferSurfaceRenderTarget_available(GLRenderer *renderer){
	return EGLPBufferSurfaceRenderTarget::available(renderer);
}

SurfaceRenderTarget *new_GLPBufferSurfaceRenderTarget(GLRenderer *renderer,bool copy){
	return new EGLPBufferSurfaceRenderTarget(renderer,copy);
}

bool EGLPBufferSurfaceRenderTarget::available(GLRenderer *renderer){
	return ((EGLRenderTarget*)renderer->getPrimaryRenderTarget()->getRootRenderTarget())->egl_version>=11;
}

EGLPBufferSurfaceRenderTarget::EGLPBufferSurfaceRenderTarget(GLRenderer *renderer,bool copy):EGLRenderTarget(),
	mRenderer(NULL),
	mCopy(false),
	mTexture(NULL),
	mWidth(0),
	mHeight(0),
	mBound(false),
	mInitialized(false)
{
	mRenderer=renderer;
	mCopy=copy;
	egl_version=((EGLRenderTarget*)mRenderer->getPrimaryRenderTarget()->getRootRenderTarget())->egl_version;
}

EGLPBufferSurfaceRenderTarget::~EGLPBufferSurfaceRenderTarget(){
	destroy();
}

bool EGLPBufferSurfaceRenderTarget::create(){
	mWidth=0;
	mHeight=0;
	mBound=false;
	mInitialized=false;

	return true;
}

bool EGLPBufferSurfaceRenderTarget::destroy(){
	destroyBuffer();

	return true;
}

bool EGLPBufferSurfaceRenderTarget::makeCurrent(){
	unbind();

	EGLRenderTarget::makeCurrent();

	if(mInitialized==false){
		mRenderer->setDefaultStates();
		mInitialized=true;
	}

	return true;
}

bool EGLPBufferSurfaceRenderTarget::swap(){
	glFlush();

	bind();

	return true;
}

bool EGLPBufferSurfaceRenderTarget::attach(Surface::ptr surface,Attachment attachment){
	GLTextureMipSurface *gltextureSurface=((GLSurface*)surface->getRootSurface())->castToGLTextureMipSurface();
	mTexture=gltextureSurface->getTexture();

	if((mTexture->getFormat()&Texture::Format_BIT_DEPTH)>0){
		Error::invalidParameters(Categories::TOADLET_PEEPER,
			"Format_BIT_DEPTH not available for pbuffers");
		return false;
	}

	compile();

	return true;
}

bool EGLPBufferSurfaceRenderTarget::remove(Surface::ptr surface){
	mTexture=NULL;

	compile();

	return false;
}

bool EGLPBufferSurfaceRenderTarget::compile(){
	if(mTexture!=NULL){
		createBuffer();

		Matrix4x4 matrix;
		Math::setMatrix4x4FromScale(matrix,Math::ONE,-Math::ONE,Math::ONE);
		Math::setMatrix4x4FromTranslate(matrix,0,Math::ONE,0);
		mTexture->setMatrix(matrix);
	}
	else{
		destroyBuffer();
	}

	return true;
}

bool EGLPBufferSurfaceRenderTarget::createBuffer(){
	destroyBuffer();

	int width=mTexture->getWidth();
	int height=mTexture->getHeight();

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
	int depthBits=16;

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
			"width or height not as expected");
		return false;
	}

	return true;
}

bool EGLPBufferSurfaceRenderTarget::destroyBuffer(){
	unbind();

	if(mDisplay!=EGL_NO_DISPLAY){
		if(mContext==eglGetCurrentContext()){
			((EGLRenderTarget*)mRenderer->getPrimaryRenderTarget()->getRootRenderTarget())->makeCurrent();
		}

		if(mContext!=EGL_NO_CONTEXT){
			eglDestroyContext(mDisplay,mContext);
			TOADLET_CHECK_EGLERROR("eglDestroyContext");
			mContext=EGL_NO_CONTEXT;
		}

		if(mSurface!=EGL_NO_SURFACE){
			eglDestroySurface(mDisplay,mSurface);
			TOADLET_CHECK_EGLERROR("eglDestroySurface");
			mSurface=EGL_NO_SURFACE;
		}
	}

	return true;
}

void EGLPBufferSurfaceRenderTarget::bind(){
	if(mBound==false){
		mBound=true;
		glBindTexture(mTexture->getTarget(),mTexture->getHandle());
		TOADLET_CHECK_GLERROR("glBindTexture");
		if(mCopy){
			glCopyTexSubImage2D(mTexture->getTarget(),0,0,0,0,0,mWidth,mHeight);
			TOADLET_CHECK_GLERROR("glBindTexture");
		}
		else{
			eglBindTexImage(mDisplay,mSurface,EGL_BACK_BUFFER);
			TOADLET_CHECK_EGLERROR("eglBindTexImage");
		}
	}
}

void EGLPBufferSurfaceRenderTarget::unbind(){
	if(mBound==true){
		mBound=false;
		if(mCopy==false){
			eglReleaseTexImage(mDisplay,mSurface,EGL_BACK_BUFFER);
			TOADLET_CHECK_EGLERROR("eglReleaseTexImage");
		}
	}
}

}
}

