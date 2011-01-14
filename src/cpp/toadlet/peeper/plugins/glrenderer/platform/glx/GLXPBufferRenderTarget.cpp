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

/// @todo: This needs to be run through and tested thoroughly, plus remove the SGIX stuff in favor of EXT
 
#include "GLXPBufferRenderTarget.h"
#include "../../GLRenderer.h"
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;

namespace toadlet{
namespace peeper{

bool GLPBufferRenderTarget_available(GLRenderer *renderer){
	return GLXPBufferRenderTarget::available(renderer);
}

PixelBufferRenderTarget *new_GLPBufferRenderTarget(GLRenderer *renderer){
	return new GLXPBufferRenderTarget(renderer);
}

bool GLXPBufferRenderTarget::available(GLRenderer *renderer){
	#if defined(TOADLET_HAS_GLEW)
		return GLXEW_SGIX_pbuffer>0;
	#else
		return false;
	#endif
}

GLXPBufferRenderTarget::GLXPBufferRenderTarget(GLRenderer *renderer):GLXRenderTarget(),
	mRenderer(NULL),
	mTexture(NULL),
	mPBuffer(0),
	mWidth(0),
	mHeight(0),
	mBound(false),
	mInitialized(false)
{
	mRenderer=renderer;
}

GLXPBufferRenderTarget::~GLXPBufferRenderTarget(){
	destroy();
}

bool GLXPBufferRenderTarget::create(){
	mWidth=0;
	mHeight=0;
	mBound=false;
	mInitialized=false;

	return true;
}

void GLXPBufferRenderTarget::destroy(){
	destroyBuffer();
}

bool GLXPBufferRenderTarget::activate(){
	unbind();

	GLXRenderTarget::activate();

	if(mInitialized==false){
		mRenderer->setDefaultStates();
		mInitialized=true;
	}

	return true;
}

bool GLXPBufferRenderTarget::swap(){
	glFlush();

	bind();
	
	return true;
}

bool GLXPBufferRenderTarget::attach(PixelBuffer::ptr buffer,Attachment attachment){
	GLTextureMipPixelBuffer *gltextureBuffer=((GLPixelBuffer*)buffer->getRootPixelBuffer())->castToGLTextureMipPixelBuffer();
	mTexture=gltextureBuffer->getTexture();

	if((mTexture->getFormat()&Texture::Format_BIT_DEPTH)>0){
		Error::invalidParameters(Categories::TOADLET_PEEPER,
			"Format_BIT_DEPTH not available for pbuffers");
		return false;
	}

	compile();

	return true;
}

bool GLXPBufferRenderTarget::remove(PixelBuffer::ptr buffer){
	mTexture=NULL;

	compile();

	return false;
}

bool GLXPBufferRenderTarget::compile(){
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

static int xError=0;

static int handleXError(Display *,XErrorEvent *){
	xError=1;
	return 0;
}

bool GLXPBufferRenderTarget::createBuffer(){
	destroyBuffer();

	int width=mTexture->getWidth();
	int height=mTexture->getHeight();

	GLXRenderTarget *renderTarget=(GLXRenderTarget*)(mRenderer->getPrimaryRenderTarget()->getRootRenderTarget());

	mDisplay=XOpenDisplay(NULL);
	if(mDisplay==None){
		destroyBuffer();
		Error::unknown(Categories::TOADLET_PEEPER,
			"DISPLAY bad value; couldn't open display");
		return false;
	}

	int (*oldHandler)(Display*,XErrorEvent*);
	oldHandler=XSetErrorHandler(handleXError);

	int format=mTexture->getFormat();
	int redBits=ImageFormatConversion::getRedBits(format);
	int greenBits=ImageFormatConversion::getGreenBits(format);
	int blueBits=ImageFormatConversion::getBlueBits(format);
	int depthBits=16;

	int fbAttribs[]={
		/* Single buffered, with depth buffer */
		GLX_RENDER_TYPE_SGIX,GLX_RGBA_BIT_SGIX,
		GLX_DRAWABLE_TYPE_SGIX,GLX_PIXMAP_BIT_SGIX,
		GLX_RED_SIZE,redBits,
		GLX_GREEN_SIZE,greenBits,
		GLX_BLUE_SIZE,blueBits,
		GLX_DEPTH_SIZE,depthBits,
		GLX_DOUBLEBUFFER,0,
		GLX_STENCIL_SIZE,0,
		GLX_BIND_TO_TEXTURE_RGB_EXT,True,
		None
	};

	int pbAttribs[]={
		GLX_LARGEST_PBUFFER_SGIX,False,
		GLX_PRESERVED_CONTENTS_SGIX,False,
		GLX_TEXTURE_FORMAT_EXT,GLX_TEXTURE_FORMAT_RGB_EXT,
		None
	};

	int nConfigs=0;
	GLXFBConfigSGIX *fbConfigs=glXChooseFBConfigSGIX(mDisplay,XDefaultScreen(mDisplay),fbAttribs,&nConfigs);
	if(nConfigs==0 || fbConfigs==NULL){
		Error::unknown(Categories::TOADLET_PEEPER,
			"Error getting FBConfig");
		return false;
	}

	GLXFBConfigSGIX fbConfig=0;
	int i;
	for(i=0;i<nConfigs;++i){
		xError=0;
		mPBuffer=glXCreateGLXPbufferSGIX(mDisplay,fbConfigs[i],width,height,pbAttribs);
		if(mPBuffer!=None && xError==0){
			fbConfig=fbConfigs[i];
			break;
		}
	}

	if(mPBuffer==None){
		destroyBuffer();
		Error::unknown(Categories::TOADLET_PEEPER,
			"Error creating pbuffer");
		return false;
	}
	
	XSetErrorHandler(oldHandler);
/*
	XVisualInfo *visInfo=glXGetVisualFromFBConfigSGIX(mDisplay,fbConfig);
	if(visInfo==NULL){
		destroyBuffer();
		Error::unknown(Categories::TOADLET_PEEPER,
			"Error getting visInfo");
		return false;
	}
*/
//	mContext=glXCreateContext(mDisplay,visInfo,renderTarget->getGLXContext(),True);
mContext=glXCreateNewContext(mDisplay,fbConfig,GLX_RGBA_BIT,renderTarget->getGLXContext(),True);
	if(mContext==NULL){
//		mContext=glXCreateContext(mDisplay,visInfo,renderTarget->getGLXContext(),False);
mContext=glXCreateNewContext(mDisplay,fbConfig,GLX_RGBA_BIT,renderTarget->getGLXContext(),False);
		if(mContext==NULL){
			destroyBuffer();
			Error::unknown(Categories::TOADLET_PEEPER,
				"Error getting visInfo");
			return false;
		}
		else{
			Logger::warning(Categories::TOADLET_PEEPER,
				"Using indirect GLXContext");
		}
	}

	XFree(fbConfigs);
//	XFree(visInfo);

	mDrawable=mPBuffer;

	unsigned int uwidth=width,uheight=height;
	glXQueryGLXPbufferSGIX(mDisplay,mPBuffer,GLX_WIDTH_SGIX,&uwidth);
	glXQueryGLXPbufferSGIX(mDisplay,mPBuffer,GLX_HEIGHT_SGIX,&uheight);
	mWidth=uwidth;mHeight=uheight;
	if(mWidth!=width || mHeight!=height){
		Error::unknown(Categories::TOADLET_PEEPER,
			"width or height not as expected");
		return false;
	}

	return true;
}

bool GLXPBufferRenderTarget::destroyBuffer(){
	if(mDisplay!=0){
		if(mContext==glXGetCurrentContext()){
			((GLRenderTarget*)mRenderer->getPrimaryRenderTarget()->getRootRenderTarget())->activate();
		}

		if(mContext!=0){
			glXDestroyContext(mDisplay,mContext);
			mContext=0;
		}
		if(mPBuffer!=0){
			glXDestroyGLXPbufferSGIX(mDisplay,mPBuffer);
			mPBuffer=0;
		}
		
		if(mDisplay!=None){
			XCloseDisplay(mDisplay);
			mDisplay=None;
		}
	}
	return true;
}

void GLXPBufferRenderTarget::bind(){
	glBindTexture(mTexture->getTarget(),mTexture->getHandle());
	glCopyTexSubImage2D(mTexture->getTarget(),0,0,0,0,0,mWidth,mHeight);
}

void GLXPBufferRenderTarget::unbind(){
	// Nothing yet...
}

}
}

