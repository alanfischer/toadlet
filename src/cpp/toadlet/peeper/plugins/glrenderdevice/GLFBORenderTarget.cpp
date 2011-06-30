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

#include "GLFBORenderTarget.h"
#include "GLTexture.h"
#include "GLRenderDevice.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>

namespace toadlet{
namespace peeper{

bool GLFBORenderTarget::available(GLRenderDevice *renderDevice){
	#if defined(TOADLET_HAS_GLEW)
		return GLEW_EXT_framebuffer_object>0;
	#elif defined(TOADLET_HAS_EAGL)
		return true;
	#else
		return false;
	#endif
}

GLFBORenderTarget::GLFBORenderTarget(GLRenderDevice *renderDevice):GLRenderTarget(),
	mDevice(NULL),
	mListener(NULL),
	mWidth(0),
	mHeight(0),
	mHandle(0),
	mNeedsCompile(false)
	//mBuffers,
	//mBufferAttachments,
	//mDepthBuffer
{
	mDevice=renderDevice;
}

GLFBORenderTarget::~GLFBORenderTarget(){
	destroy();
}

bool GLFBORenderTarget::create(){
	mWidth=0;
	mHeight=0;
	mNeedsCompile=true;

	glGenFramebuffers(1,&mHandle);

	TOADLET_CHECK_GLERROR("GLFBORenderTarget::create");

	return true;
}

void GLFBORenderTarget::destroy(){
	// Check Handle here so if we are destroyed multiple times, we won't attempt to reference the RenderDevice
	if(mHandle!=0 && mDevice!=NULL && mDevice->getRenderTarget()->getRootRenderTarget()==(GLRenderTarget*)this){
		glBindFramebuffer(GL_FRAMEBUFFER,0);
	}
 
	if(mDepthBuffer!=NULL){
		mDepthBuffer->destroy();
		mDepthBuffer=NULL;
	}

	if(mHandle!=0){
		glDeleteFramebuffers(1,&mHandle);
		mHandle=0;

		// Check this only if we had a handle, to eliminate errors at shutdown
		TOADLET_CHECK_GLERROR("GLFBORenderTarget::destroy");
	}

	if(mListener!=NULL){
		mListener->renderTargetDestroyed((PixelBufferRenderTarget*)this);
		mListener=NULL;
	}
}

bool GLFBORenderTarget::activate(){
	if(mNeedsCompile){
		compile();
	}

	glBindFramebuffer(GL_FRAMEBUFFER,mHandle);

	TOADLET_CHECK_GLERROR("GLFBORenderTarget::activate");

	return true;
}

bool GLFBORenderTarget::deactivate(){
	glBindFramebuffer(GL_FRAMEBUFFER,0);

	TOADLET_CHECK_GLERROR("GLFBORenderTarget::deactivate");

	return true;
}

bool GLFBORenderTarget::swap(){
	glBindFramebuffer(GL_FRAMEBUFFER,0);

	int i;
	for(i=0;i<mBufferAttachments.size();++i){
		if(mBufferAttachments[i]!=Attachment_DEPTH_STENCIL){
			GLPixelBuffer *glpixelBuffer=(GLPixelBuffer*)mBuffers[i]->getRootPixelBuffer();
			GLTextureMipPixelBuffer *textureBuffer=glpixelBuffer->castToGLTextureMipPixelBuffer();
			if(textureBuffer!=NULL){
				textureBuffer->getTexture()->generateMipLevels();
			}
		}
	}

	TOADLET_CHECK_GLERROR("GLFBORenderTarget::swap");

	return true;
}

bool GLFBORenderTarget::attach(PixelBuffer::ptr buffer,Attachment attachment){
	GLPixelBuffer *glpixelBuffer=(GLPixelBuffer*)buffer->getRootPixelBuffer();

	GLTextureMipPixelBuffer *textureBuffer=glpixelBuffer->castToGLTextureMipPixelBuffer();
	GLFBOPixelBuffer *fboBuffer=glpixelBuffer->castToGLFBOPixelBuffer();
	GLBuffer *pixelBuffer=glpixelBuffer->castToGLBuffer();

	glBindFramebuffer(GL_FRAMEBUFFER,mHandle);
	if(textureBuffer!=NULL){
		GLTexture *texture=textureBuffer->getTexture();
		GLuint handle=textureBuffer->getHandle();
		GLenum target=textureBuffer->getTarget();
		GLuint level=textureBuffer->getLevel();
		mWidth=textureBuffer->getWidth();
		mHeight=textureBuffer->getHeight();

		glFramebufferTexture2D(GL_FRAMEBUFFER,getGLAttachment(attachment),target,handle,level);

		Matrix4x4 matrix;
		Math::setMatrix4x4FromScale(matrix,Math::ONE,-Math::ONE,Math::ONE);
		Math::setMatrix4x4FromTranslate(matrix,0,Math::ONE,0);
		texture->setMatrix(matrix);
	}
	else if(fboBuffer!=NULL){
		GLuint handle=fboBuffer->getHandle();

		glFramebufferRenderbuffer(GL_FRAMEBUFFER,getGLAttachment(attachment),GL_RENDERBUFFER,handle);
	}
	else if(pixelBuffer!=NULL){
		Error::unimplemented("can not bind PixelBuffer to FBO");
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER,0);

	mBuffers.add(buffer);
	mBufferAttachments.add(attachment);
	mNeedsCompile=true;

	TOADLET_CHECK_GLERROR("GLFBORenderTarget::attach");

	return true;
}

bool GLFBORenderTarget::remove(PixelBuffer::ptr buffer){
	int i;
	for(i=0;i<mBuffers.size();++i){
		if(mBuffers[i]==buffer){
			break;
		}
	}
	if(i==mBuffers.size()){
		return false;
	}

	GLPixelBuffer *glbuffer=(GLPixelBuffer*)buffer->getRootPixelBuffer();

	GLTextureMipPixelBuffer *texturebuffer=glbuffer->castToGLTextureMipPixelBuffer();
	GLFBOPixelBuffer *fbobuffer=glbuffer->castToGLFBOPixelBuffer();

	Attachment attachment=mBufferAttachments[i];

	glBindFramebuffer(GL_FRAMEBUFFER,mHandle);
	if(texturebuffer!=NULL){
		glFramebufferTexture2D(GL_FRAMEBUFFER,getGLAttachment(attachment),GL_TEXTURE_2D,0,0);
	}
	else if(fbobuffer!=NULL){
		glFramebufferRenderbuffer(GL_FRAMEBUFFER,getGLAttachment(attachment),GL_RENDERBUFFER,0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER,0);

	mBuffers.removeAt(i);
	mBufferAttachments.removeAt(i);
	mNeedsCompile=true;

	TOADLET_CHECK_GLERROR("GLFBORenderTarget::remove");

	return true;
}

/// @todo: Make it check to see if a pre-existing self-created depth buffer is compatible, and if not, destroy & recreate it
bool GLFBORenderTarget::compile(){
	TOADLET_CHECK_GLERROR("entering GLFBORenderTarget::compile");

	PixelBuffer::ptr depth;
	PixelBuffer::ptr color;

	int i;
	for(i=0;i<mBufferAttachments.size();++i){
		if(mBufferAttachments[i]==Attachment_DEPTH_STENCIL){
			depth=mBuffers[i];
		}
		else{
			color=mBuffers[i];
		}
	}

	if(mDepthBuffer!=NULL){
		remove(mDepthBuffer);
		mDepthBuffer->destroy();
		mDepthBuffer=NULL;
	}

	glBindFramebuffer(GL_FRAMEBUFFER,mHandle);
	if(color!=NULL && depth==NULL){
		// No Depth-Stencil buffer, so add one
		GLFBOPixelBuffer::ptr buffer(new GLFBOPixelBuffer(this));
		if(buffer->create(Buffer::Usage_BIT_STREAM,Buffer::Access_NONE,Texture::Format_DEPTH_24,mWidth,mHeight,1)){
			attach(buffer,Attachment_DEPTH_STENCIL);
			mDepthBuffer=buffer;
		}
	}
	else if(color==NULL && depth!=NULL){
		#if !defined(TOADLET_HAS_GLES)
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		#endif
	}

	// Rebind it since attach will unbind it
	glBindFramebuffer(GL_FRAMEBUFFER,mHandle);
	GLenum status=glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status!=GL_FRAMEBUFFER_COMPLETE){
		Logger::warning(Categories::TOADLET_PEEPER,getFBOMessage(status));
	}
	glBindFramebuffer(GL_FRAMEBUFFER,0);

	TOADLET_CHECK_GLERROR("GLFBORenderTarget::compile");

	mNeedsCompile=false;

	return status==GL_FRAMEBUFFER_COMPLETE;
}

GLenum GLFBORenderTarget::getGLAttachment(Attachment attachment){
	switch(attachment){
		case Attachment_DEPTH_STENCIL:
			return GL_DEPTH_ATTACHMENT;
		case Attachment_COLOR_0:
			return GL_COLOR_ATTACHMENT0;
		#if !defined(TOADLET_HAS_EAGL)
			case Attachment_COLOR_1:
				return GL_COLOR_ATTACHMENT1;
			case Attachment_COLOR_2:
				return GL_COLOR_ATTACHMENT2;
			case Attachment_COLOR_3:
				return GL_COLOR_ATTACHMENT3;
		#endif
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"GLFBORenderTarget::getGLAttachment: Invalid attachment");
			return 0;
	};
}

const char *GLFBORenderTarget::getFBOMessage(GLenum status){
	switch(status){
		case GL_FRAMEBUFFER_COMPLETE:
			return "GL_FRAMEBUFFER_COMPLETE";
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
			return "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS";
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS:
			return "GL_FRAMEBUFFER_INCOMPLETE_FORMATS";
		#if !defined(TOADLET_HAS_GLES)
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
				return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
				return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
		#endif
		default:
			return "UNKNOWN";
	}
}

}
}
