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
/*
#include "GLRenderer.h"
#include "GLFBORenderTexturePeer.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;

namespace toadlet{
namespace peeper{

bool GLFBORenderTexturePeer_available(GLRenderer *renderer){
	return GLFBORenderTexturePeer::available(renderer);
}

GLTexturePeer *new_GLFBORenderTexturePeer(GLRenderer *renderer,RenderTexture *texture){
	return new GLFBORenderTexturePeer(renderer,texture);
}

bool GLFBORenderTexturePeer::available(GLRenderer *renderer){
#	if defined(TOADLET_HAS_GLEW)
		return GLEW_EXT_framebuffer_object>0;
#	elif defined(TOADLET_HAS_EAGL)
		return true;
#	else
		return false;
#	endif
}

GLFBORenderTexturePeer::GLFBORenderTexturePeer(GLRenderer *renderer,RenderTexture *texture):GLRenderTargetPeer(),GLTexturePeer(renderer,texture),
	renderTexture(NULL),
	width(0),
	height(0),
	framebufferHandle(0),
	depthRenderbufferHandle(0)
{
	const CapabilitySet &capabilitySet=renderer->getCapabilitySet();

	renderTexture=texture;

	initialized=true;

	width=texture->getWidth();
	height=texture->getHeight();

	textureTarget=getGLTarget(texture,capabilitySet);
	GLuint format=getGLFormat(texture->getFormat());
	GLuint internalFormat=format;
	GLuint type=getGLType(texture->getFormat());

	glTexImage2D(textureTarget,0,internalFormat,width,height,0,format,type,NULL);
	glGenFramebuffers(1,&framebufferHandle);
	glBindFramebuffer(GL_FRAMEBUFFER,framebufferHandle);

	if((texture->getFormat()&Texture::Format_BIT_DEPTH)>0){
		// If its a depth texture, we just use the depth texture as the framebuffer target
		glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,textureTarget,textureHandle,0);
		#if !defined(TOADLET_HAS_GLES)
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		#endif
	}
	else{
		// If its a color texture, we use the color texture as the framebuffer target
		glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,textureTarget,textureHandle,0);

		if(renderTexture->hasDepthBuffer()){
			// And then we generate a depth buffer and tell the framebuffer to use it for the depth data
			glGenRenderbuffers(1,&depthRenderbufferHandle);
			glBindRenderbuffer(GL_RENDERBUFFER,depthRenderbufferHandle);
			glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT24,width,height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,depthRenderbufferHandle);
		}
	}

	GLenum status=glCheckFramebufferStatus(GL_FRAMEBUFFER);

	glBindFramebuffer(GL_FRAMEBUFFER,0);

	if(status!=GL_FRAMEBUFFER_COMPLETE){
		String error;
		switch(status){
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				error="GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
			break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				error="GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
			break;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
				error="GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS";
			break;
			case GL_FRAMEBUFFER_INCOMPLETE_FORMATS:
				error="GL_FRAMEBUFFER_INCOMPLETE_FORMATS";
			break;
			#if !defined(TOADLET_HAS_GLES)
				case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
					error="GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
				break;
				case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
					error="GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
				break;
			#endif
		}
		Error::unknown(Categories::TOADLET_PEEPER,error);
		return;
	}

	TOADLET_CHECK_GLERROR("GLFBORenderTexturePeer::GLFBORenderTexturePeer");
}

GLFBORenderTexturePeer::~GLFBORenderTexturePeer(){
	if(framebufferHandle!=0){
		glDeleteFramebuffers(1,&framebufferHandle);
		framebufferHandle=0;
	}

	if(depthRenderbufferHandle!=0){
		glDeleteRenderbuffers(1,&depthRenderbufferHandle);
		depthRenderbufferHandle=0;
	}

	TOADLET_CHECK_GLERROR("GLFBORenderTexturePeer::~GLFBORenderTexturePeer");
}

void GLFBORenderTexturePeer::makeCurrent(){
	glBindFramebuffer(GL_FRAMEBUFFER,framebufferHandle);
}

void GLFBORenderTexturePeer::swap(){
	glBindFramebuffer(GL_FRAMEBUFFER,0);
}

int GLFBORenderTexturePeer::getWidth() const{
	return width;
}

int GLFBORenderTexturePeer::getHeight() const{
	return height;
}

bool GLFBORenderTexturePeer::isValid() const{
	return framebufferHandle!=0;
}

}
}

*/