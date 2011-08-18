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

#include "WGLPBufferRenderTarget.h"
#include "../../GLRenderDevice.h"
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/image/ImageFormatConversion.h>

namespace toadlet{
namespace peeper{

bool GLPBufferRenderTarget_available(GLRenderDevice *renderDevice){
	return WGLPBufferRenderTarget::available(renderDevice);
}

PixelBufferRenderTarget *new_GLPBufferRenderTarget(GLRenderDevice *renderDevice){
	return new WGLPBufferRenderTarget(renderDevice);
}

bool WGLPBufferRenderTarget::available(GLRenderDevice *renderDevice){
	#if defined(TOADLET_HAS_GLEW)
		return WGLEW_ARB_render_texture>0;
	#else
		return false;
	#endif
}

WGLPBufferRenderTarget::WGLPBufferRenderTarget(GLRenderDevice *renderDevice):WGLRenderTarget(),
	mDevice(NULL),
	mListener(NULL),
	mCopy(false),
	mTexture(NULL),
	mPBuffer(NULL),
	mWidth(0),
	mHeight(0),
	mBound(false),
	mInitialized(false)
{
	mDevice=renderDevice;
}

WGLPBufferRenderTarget::~WGLPBufferRenderTarget(){
	destroy();
}

bool WGLPBufferRenderTarget::create(){
	mCopy=true;
	mWidth=0;
	mHeight=0;
	mBound=false;
	mInitialized=false;

	return true;
}

void WGLPBufferRenderTarget::destroy(){
	destroyBuffer();

	if(mListener!=NULL){
		mListener->renderTargetDestroyed((PixelBufferRenderTarget*)this);
		mListener=NULL;
	}
}

bool WGLPBufferRenderTarget::activate(){
	unbind();

	WGLRenderTarget::activate();

	if(mInitialized==false){
		mDevice->setDefaultState();
		mInitialized=true;
	}

	return true;
}

bool WGLPBufferRenderTarget::swap(){
	glFlush();

	bind();

	return true;
}

bool WGLPBufferRenderTarget::attach(PixelBuffer::ptr buffer,Attachment attachment){
	GLTextureMipPixelBuffer *texturebuffer=((GLPixelBuffer*)buffer->getRootPixelBuffer())->castToGLTextureMipPixelBuffer();
	mTexture=texturebuffer->getTexture();

	if((mTexture->getFormat()->pixelFormat&TextureFormat::Format_SEMANTIC_DEPTH)>0){
		Error::invalidParameters(Categories::TOADLET_PEEPER,
			"Format_SEMANTIC_DEPTH not available for pbuffers");
		return false;
	}

	compile();

	return true;
}

bool WGLPBufferRenderTarget::remove(PixelBuffer::ptr buffer){
	mTexture=NULL;

	compile();

	return false;
}

bool WGLPBufferRenderTarget::compile(){
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

bool WGLPBufferRenderTarget::createBuffer(){
	destroyBuffer();

	WGLRenderTarget *primaryTarget=(WGLRenderTarget*)(mDevice->getPrimaryRenderTarget()->getRootRenderTarget());

	int width=mTexture->getFormat()->width;
	int height=mTexture->getFormat()->height;

	HDC hdc=primaryTarget->getDC();

	int bindType=WGL_BIND_TO_TEXTURE_RGB_ARB;
	int pixelType=WGL_TYPE_RGBA_ARB;
	int texFormat=WGL_TEXTURE_RGB_ARB;

	int pixelFormat=mTexture->getFormat()->pixelFormat;
	int redBits=ImageFormatConversion::getRedBits(pixelFormat);
	int greenBits=ImageFormatConversion::getGreenBits(pixelFormat);
	int blueBits=ImageFormatConversion::getBlueBits(pixelFormat);
	int alphaBits=ImageFormatConversion::getAlphaBits(pixelFormat);
	int depthBits=16;

	int iAttributes[]={
		WGL_COLOR_BITS_ARB,redBits+greenBits+blueBits,
		WGL_ALPHA_BITS_ARB,alphaBits,
		WGL_STENCIL_BITS_ARB,0,
		WGL_DEPTH_BITS_ARB,depthBits,
		WGL_DRAW_TO_PBUFFER_ARB,GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB,GL_FALSE,
		WGL_PIXEL_TYPE_ARB,pixelType,
		WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB,
		mCopy?0:bindType,GL_TRUE,
		0,
	};

	float fAttributes[]={
		0
	};

	int pAttributes[]={
		mCopy?0:WGL_TEXTURE_FORMAT_ARB,texFormat,
		mCopy?0:WGL_TEXTURE_TARGET_ARB,WGL_TEXTURE_2D_ARB,
		0 
	};

	int wglformat=0;
	unsigned int count=0;
	wglChoosePixelFormatARB(hdc,iAttributes,fAttributes,1,&wglformat,&count);
	TOADLET_CHECK_WGLERROR("wglChoosePixelFormatARB");

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
	TOADLET_CHECK_WGLERROR("wglGetPixelFormatAttribivARB");

	Logger::alert(Categories::TOADLET_PEEPER,
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

	if(wglShareLists(((WGLRenderTarget*)primaryTarget)->getGLRC(),mGLRC)==false){
		destroyBuffer();
		Error::unknown(Categories::TOADLET_PEEPER,
			"sharing failed");
		return false;
	}

	wglQueryPbufferARB(mPBuffer,WGL_PBUFFER_WIDTH_ARB,(int*)&mWidth);
	wglQueryPbufferARB(mPBuffer,WGL_PBUFFER_HEIGHT_ARB,(int*)&mHeight);
	if(mWidth!=width || mHeight!=height){
		Error::unknown(Categories::TOADLET_PEEPER,
			"width or height not as expected");
		return false;
	}

	TOADLET_CHECK_WGLERROR("createBuffer");

	return true;
}

bool WGLPBufferRenderTarget::destroyBuffer(){
	unbind();

	if(mGLRC==wglGetCurrentContext()){
		((GLRenderTarget*)mDevice->getPrimaryRenderTarget()->getRootRenderTarget())->activate();
	}

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

void WGLPBufferRenderTarget::bind(){
	if(mBound==false){
		mBound=true;
		glBindTexture(mTexture->getTarget(),mTexture->getHandle());
		TOADLET_CHECK_GLERROR("glBindTexture");
		if(mCopy){
			glCopyTexSubImage2D(mTexture->getTarget(),0,0,0,0,0,mWidth,mHeight);
			TOADLET_CHECK_GLERROR("glBindTexture");
		}
		else{
			wglBindTexImageARB(mPBuffer,WGL_FRONT_LEFT_ARB);
			TOADLET_CHECK_WGLERROR("wglBindTexImageARB");
		}
	}
}

void WGLPBufferRenderTarget::unbind(){
	if(mBound==true){
		mBound=false;
		if(mCopy==false){
			wglReleaseTexImageARB(mPBuffer,WGL_FRONT_LEFT_ARB);
			TOADLET_CHECK_WGLERROR("wglReleaseTexImageARB");
		}
	}
}

}
}
