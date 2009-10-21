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

// Include GLRenderer first due to collisions with the Status enum & X windows
#include "../../GLRenderer.h"
#include "../../../../RenderContext.h"
#include "GLXPBufferRenderTexturePeer.h"

using namespace toadlet::egg;
using namespace toadlet::egg::image;

namespace toadlet{
namespace peeper{

bool GLPBufferRenderTexturePeer_available(GLRenderer *renderer){
	return GLXPBufferRenderTexturePeer::available(renderer);
}

GLTexturePeer *new_GLPBufferRenderTexturePeer(GLRenderer *renderer,RenderTexture *texture){
	return new GLXPBufferRenderTexturePeer(renderer,texture);
}

bool GLXPBufferRenderTexturePeer::available(GLRenderer *renderer){
#	if defined(TOADLET_HAS_GLEW)
		return GLXEW_SGIX_pbuffer>0;
#	else
		return false;
#	endif
}

GLXPBufferRenderTexturePeer::GLXPBufferRenderTexturePeer(GLRenderer *renderer,RenderTexture *texture):GLXRenderTargetPeer(),GLTexturePeer(renderer,texture){
	mTexture=texture;
	mPBuffer=0;
	mWidth=0;
	mHeight=0;
	mBound=false;
	mShare=false;//true;

	if((texture->getFormat()&Texture::Format_BIT_DEPTH)>0){
		Error::invalidParameters(Categories::TOADLET_EGG,
			"Format_BIT_DEPTH not available for pbuffers");
		return;
	}

	glTexImage2D(textureTarget,0,GL_RGB,((Texture*)texture)->getWidth(),((Texture*)texture)->getHeight(),0,GL_RGB,GL_UNSIGNED_BYTE,NULL);

	createBuffer(renderer);
}

GLXPBufferRenderTexturePeer::~GLXPBufferRenderTexturePeer(){
	destroyBuffer();
}

void GLXPBufferRenderTexturePeer::makeCurrent(){
	GLXRenderTargetPeer::makeCurrent();
}

void GLXPBufferRenderTexturePeer::swap(){
	glBindTexture(textureTarget,textureHandle);
	glCopyTexSubImage2D(textureTarget,0,0,0,0,0,mWidth,mHeight);
}

static int xError=0;

static int handleXError(Display *,XErrorEvent *){
	xError=1;
	return 0;
}

void GLXPBufferRenderTexturePeer::createBuffer(GLRenderer *renderer){
	int width=((Texture*)mTexture)->getWidth();
	int height=((Texture*)mTexture)->getHeight();

	GLXRenderTargetPeer *renderContextPeer=(GLXRenderTargetPeer*)renderer->getRenderContext()->internal_getRenderTargetPeer();

	mDisplay=XOpenDisplay(NULL);
	if(mDisplay==None){
		destroyBuffer();
		Error::unknown(Categories::TOADLET_PEEPER,
			"DISPLAY bad value; couldn't open display");
		return;
	}

	int (*oldHandler)(Display*,XErrorEvent*);
	oldHandler=XSetErrorHandler(handleXError);

	int format=mTexture->getFormat();
	int redBits=ImageFormatConversion::getRedBits(format);
	int greenBits=ImageFormatConversion::getGreenBits(format);
	int blueBits=ImageFormatConversion::getBlueBits(format);
	int depthBits=mTexture->hasDepthBuffer()?16:0;

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
		None
	};

	int pbAttribs[]={
		GLX_LARGEST_PBUFFER_SGIX,False,
		GLX_PRESERVED_CONTENTS_SGIX,False,
		None
	};


	int nConfigs=0;
	GLXFBConfigSGIX *fbConfigs=glXChooseFBConfigSGIX(mDisplay,XDefaultScreen(mDisplay),fbAttribs,&nConfigs);
	if(nConfigs==0 || fbConfigs==NULL){
		Error::unknown(Categories::TOADLET_PEEPER,
			"Error getting FBConfig");
		return;
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

	XFree(fbConfigs);

	if(mPBuffer==None){
		destroyBuffer();
		Error::unknown(Categories::TOADLET_PEEPER,
			"Error creating pbuffer");
		return;
	}

	XSetErrorHandler(oldHandler);

	XVisualInfo *visInfo=glXGetVisualFromFBConfigSGIX(mDisplay,fbConfig);
	if(visInfo==NULL){
		destroyBuffer();
		Error::unknown(Categories::TOADLET_PEEPER,
			"Error getting visInfo");
		return;
	}

	mContext=glXCreateContext(mDisplay,visInfo,renderContextPeer->getGLXContext(),True);
	if(mContext==NULL){
		mContext=glXCreateContext(mDisplay,visInfo,renderContextPeer->getGLXContext(),False);
		if(mContext==NULL){
			destroyBuffer();
			Error::unknown(Categories::TOADLET_PEEPER,
				"Error getting visInfo");
			return;
		}
		else{
			Logger::log(Categories::TOADLET_PEEPER,Logger::Level_WARNING,
				"Using indirect GLXContext");
		}
	}

	glXMakeCurrent(mDisplay,mPBuffer,mContext);

	mDrawable=mPBuffer;

	unsigned int uwidth=0,uheight=0;
	glXQueryGLXPbufferSGIX(mDisplay,mPBuffer,GLX_WIDTH_SGIX,&uwidth);
	glXQueryGLXPbufferSGIX(mDisplay,mPBuffer,GLX_HEIGHT_SGIX,&uheight);
	mWidth=uwidth;mHeight=uheight;
	if(mWidth!=width || mHeight!=height){
		Error::unknown(Categories::TOADLET_PEEPER,
			"GLXPBufferRenderTexturePeer::createBuffer: width or height not as expected");
	}
}

void GLXPBufferRenderTexturePeer::destroyBuffer(){
	if(mContext!=0 && mShare==false){
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

int GLXPBufferRenderTexturePeer::getWidth() const{
	return mWidth;
}

int GLXPBufferRenderTexturePeer::getHeight() const{
	return mHeight;
}

bool GLXPBufferRenderTexturePeer::isValid() const{
	return mPBuffer!=0;
}

}
}

