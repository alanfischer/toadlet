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

#include "EGLPBufferRenderTarget.h"
#include "../../GLRenderDevice.h"
#include <toadlet/peeper/TextureFormat.h>

namespace toadlet{
namespace peeper{

bool GLPBufferRenderTarget_available(GLRenderDevice *renderDevice){
	return EGLPBufferRenderTarget::available(renderDevice);
}

PixelBufferRenderTarget *new_GLPBufferRenderTarget(GLRenderDevice *renderDevice){
	return new EGLPBufferRenderTarget(renderDevice);
}

bool EGLPBufferRenderTarget::available(GLRenderDevice *renderDevice){
	return true;
}

EGLPBufferRenderTarget::EGLPBufferRenderTarget(GLRenderDevice *renderDevice):
	mDevice(NULL),
	mCopy(false),
	mSeparateContext(false),
	mTexture(NULL),
	mWidth(0),
	mHeight(0),
	mBound(false),
	mInitialized(false)
{
	mDevice=renderDevice;
	egl_version=((EGLRenderTarget*)mDevice->getPrimaryRenderTarget()->getRootRenderTarget())->egl_version;
}

bool EGLPBufferRenderTarget::create(){
	mCopy=true;
	mSeparateContext=false;
	mWidth=0;
	mHeight=0;
	mBound=false;
	mInitialized=false;

	return true;
}

void EGLPBufferRenderTarget::destroy(){
	destroyBuffer();

	BaseResource::destroy();
}

bool EGLPBufferRenderTarget::activate(){
	unbind();

	EGLRenderTarget::activate();

	if(mInitialized==false){
		mDevice->setDefaultState();
		mInitialized=true;
	}

	return true;
}

bool EGLPBufferRenderTarget::deactivate(){
	glFlush();

	bind();
	
	EGLRenderTarget::deactivate();

	return true;
}

bool EGLPBufferRenderTarget::attach(PixelBuffer::ptr buffer,Attachment attachment){
	GLPixelBuffer *glbuffer=(GLPixelBuffer*)buffer->getRootPixelBuffer();
	if(glbuffer==NULL){
		Error::nullPointer(Categories::TOADLET_PEEPER,
			"null PixelBuffer");
		return false;
	}
	
	GLTextureMipPixelBuffer *glmipbuffer=glbuffer->castToGLTextureMipPixelBuffer();
	if(glmipbuffer==NULL){
		Error::nullPointer(Categories::TOADLET_PEEPER,
			"PixelBuffer is not a TextureMipPixelBuffer");
		return false;
	}
		
	mTexture=glmipbuffer->getTexture();
	if(mTexture==NULL){
		Error::invalidParameters(Categories::TOADLET_PEEPER,
			"texture is null");
		return false;
	}

	if((mTexture->getFormat()->getPixelFormat()&TextureFormat::Format_MASK_SEMANTICS)==TextureFormat::Format_SEMANTIC_DEPTH){
		Error::invalidParameters(Categories::TOADLET_PEEPER,
			"Format_SEMANTIC_DEPTH not available for pbuffers");
		return false;
	}

	compile();

	return true;
}

bool EGLPBufferRenderTarget::remove(PixelBuffer::ptr buffer){
	mTexture=NULL;

	compile();

	return false;
}

bool EGLPBufferRenderTarget::compile(){
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

bool EGLPBufferRenderTarget::createBuffer(){
	destroyBuffer();

	Log::debug("creating EGL context for pbuffer");

	mDisplay=((EGLRenderTarget*)mDevice->getPrimaryRenderTarget()->getRootRenderTarget())->getEGLDisplay();

	int width=mTexture->getFormat()->getWidth();
	int height=mTexture->getFormat()->getHeight();
	int pixelFormat=mTexture->getFormat()->getPixelFormat();
	int redBits=TextureFormat::getRedBits(pixelFormat);
	int greenBits=TextureFormat::getGreenBits(pixelFormat);
	int blueBits=TextureFormat::getBlueBits(pixelFormat);
	int alphaBits=TextureFormat::getAlphaBits(pixelFormat);
	int depthBits=16;

	EGLConfig config=chooseEGLConfig(mDisplay,redBits,greenBits,blueBits,alphaBits,depthBits,0,false,false,true,false);
	TOADLET_CHECK_EGLERROR("chooseEGLConfig");

	int attribList[5]={0};
	int i=0;
	attribList[i++]=EGL_WIDTH; attribList[i++]=width;
	attribList[i++]=EGL_HEIGHT; attribList[i++]=height;
	if(mCopy==false){
		attribList[i++]=EGL_TEXTURE_TARGET; attribList[i++]=EGL_TEXTURE_2D;
		attribList[i++]=EGL_TEXTURE_FORMAT; attribList[i++]=EGL_TEXTURE_RGB;
	}
	attribList[i++]=EGL_NONE;

	#if !defined(TOADLET_PLATFORM_EMSCRIPTEN)
		mSurface=eglCreatePbufferSurface(mDisplay,config,attribList);
		TOADLET_CHECK_EGLERROR("eglCreatePbufferSurface");
	#endif
	if(mSurface==EGL_NO_SURFACE){
		destroyBuffer();

		Error::unknown(Categories::TOADLET_PEEPER,
			"Error in eglCreatePbufferSurface");
		return false;
	}

	EGLContext context=((EGLRenderTarget*)mDevice->getPrimaryRenderTarget()->getRootRenderTarget())->getEGLContext();
	if(mSeparateContext){
		mContext=eglCreateContext(mDisplay,config,context,NULL);
		TOADLET_CHECK_EGLERROR("eglCreateContext");

		if(mContext==EGL_NO_CONTEXT){
			destroyBuffer();

			Error::unknown(Categories::TOADLET_PEEPER,
				"Error in eglCreateContext");
			return false;
		}
	}
	else{
		mContext=context;
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

bool EGLPBufferRenderTarget::destroyBuffer(){
	unbind();

	if(mDisplay!=EGL_NO_DISPLAY){
		if(mContext==eglGetCurrentContext()){
			((GLRenderTarget*)mDevice->getPrimaryRenderTarget()->getRootRenderTarget())->activate();
		}

		if(mSeparateContext && mContext!=EGL_NO_CONTEXT){
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

void EGLPBufferRenderTarget::bind(){
	if(mBound==false){
		mBound=true;
		glBindTexture(mTexture->getTarget(),mTexture->getHandle());
		TOADLET_CHECK_GLERROR("glBindTexture");
		if(mCopy){
			glCopyTexSubImage2D(mTexture->getTarget(),0,0,0,0,0,mWidth,mHeight);
			TOADLET_CHECK_GLERROR("glCopyTexSubImage2D");
		}
		else{
			#if !defined(TOADLET_PLATFORM_EMSCRIPTEN)
				eglBindTexImage(mDisplay,mSurface,EGL_BACK_BUFFER);
				TOADLET_CHECK_EGLERROR("eglBindTexImage");
			#endif
		}

		mTexture->generateMipLevels();
	}
}

void EGLPBufferRenderTarget::unbind(){
	if(mBound==true){
		mBound=false;
		if(mCopy==false){
			#if !defined(TOADLET_PLATFORM_EMSCRIPTEN)
				eglReleaseTexImage(mDisplay,mSurface,EGL_BACK_BUFFER);
				TOADLET_CHECK_EGLERROR("eglReleaseTexImage");
			#endif
		}
	}
}

}
}

