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

#include "AGLPBufferRenderTexturePeer.h"
#include "../../../../RenderContext.h"
#include "../../GLRenderer.h"
#include <toadlet/egg/Error.h>

#warning "The AGLPBufferRenderTexturePeer is currently broken.  It only copies the contents of the buffer clears for some reason"

using namespace toadlet::egg;
using namespace toadlet::egg::image;

namespace toadlet{
namespace peeper{

bool GLPBufferRenderTexturePeer_available(GLRenderer *renderer){
	return AGLPBufferRenderTexturePeer::available(renderer);
}

GLTexturePeer *new_GLPBufferRenderTexturePeer(GLRenderer *renderer,RenderTexture *texture){
	return new AGLPBufferRenderTexturePeer(renderer,texture);
}

bool AGLPBufferRenderTexturePeer::available(GLRenderer *renderer){
	#if defined(TOADLET_HAS_GLEW)
		return GLEW_APPLE_pixel_buffer>0;
	#else
		return false;
	#endif
}

AGLPBufferRenderTexturePeer::AGLPBufferRenderTexturePeer(GLRenderer *renderer,RenderTexture *texture):AGLRenderTargetPeer(),GLTexturePeer(renderer,texture){
	mRenderer=renderer;
	mTexture=texture;
	mScreen=0;
	mPBuffer=NULL;
	mWidth=0;
	mHeight=0;

	if((texture->getFormat()&Image::Format_BIT_DEPTH)>0){
		Error::invalidParameters(Categories::TOADLET_EGG,
			"Format_BIT_DEPTH not available for pbuffers");
		return;
	}

	createBuffer();
}

AGLPBufferRenderTexturePeer::~AGLPBufferRenderTexturePeer(){
	destroyBuffer();
}

void AGLPBufferRenderTexturePeer::makeCurrent(){
	AGLRenderTargetPeer::makeCurrent();

	bool result=aglSetPBuffer(mContext,mPBuffer,0,0,mScreen);
	if(result==false){
		Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ERROR,
			String("Error in aglSetPBuffer:")+(int)aglGetError());
		return;
	}
}
void AGLPBufferRenderTexturePeer::swap(){
	glBindTexture(textureTarget,textureHandle);
	AGLRenderTargetPeer *renderTargetPeer=(AGLRenderTargetPeer*)mRenderer->getRenderContext()->internal_getRenderTargetPeer();
	bool result=aglTexImagePBuffer(renderTargetPeer->getAGLContext(),mPBuffer,GL_FRONT);
	if(result==false){
		Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ERROR,
			String("Error in aglTexImagePBuffer:")+(int)aglGetError());
	}
}

void AGLPBufferRenderTexturePeer::createBuffer(){
	int depthBits=mTexture->hasDepthBuffer()?16:0;

	GLint attrs[]={
		AGL_RGBA,
		AGL_DEPTH_SIZE,depthBits,
		AGL_ACCELERATED,
		AGL_NONE,
	};

	AGLPixelFormat pixelFormat=aglChoosePixelFormat(NULL,0,attrs);
	if(pixelFormat==0){
		destroyBuffer();
		Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ERROR,
			"Error in aglChoosePixelFormat");
		return;
	}

	AGLRenderTargetPeer *renderTargetPeer=(AGLRenderTargetPeer*)mRenderer->getRenderContext()->internal_getRenderTargetPeer();
	mContext=aglCreateContext(pixelFormat,renderTargetPeer->getAGLContext());
	if(mContext==NULL){
		Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ERROR,
			"Error creating context");
		TOADLET_CHECK_AGLERROR("aglCreateContext");
		return;
	}

	mScreen=aglGetVirtualScreen(mContext);

	GLboolean result=aglCreatePBuffer(((Texture*)mTexture)->getWidth(),((Texture*)mTexture)->getHeight(),textureTarget,GL_RGBA,0,&mPBuffer);
	if(result==GL_FALSE){
		Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ERROR,
			"Error in aglCreatePBuffer");
		return;
	}

	GLenum target,internalFormat;
	GLint maxLevel;
	result=aglDescribePBuffer(mPBuffer,(GLint*)&mWidth,(GLint*)&mHeight,&target,&internalFormat,&maxLevel);
	if(result==GL_FALSE){
		Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ERROR,
			"Error in aglDescribePBuffer");
		return;
	}
}

void AGLPBufferRenderTexturePeer::destroyBuffer(){
	if(mContext!=NULL){
		aglDestroyContext(mContext);
		mContext=NULL;
	}
	if(mPBuffer!=NULL){
		aglDestroyPBuffer(mPBuffer);
		mPBuffer=NULL;
	}
}

int AGLPBufferRenderTexturePeer::getWidth() const{
	return mWidth;
}

int AGLPBufferRenderTexturePeer::getHeight() const{
	return mHeight;
}

bool AGLPBufferRenderTexturePeer::isValid() const{
	return mPBuffer!=NULL;
}

}
}

