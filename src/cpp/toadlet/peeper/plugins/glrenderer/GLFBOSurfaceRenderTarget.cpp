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

#include "GLFBOSurfaceRenderTarget.h"
#include "GLTexture.h"
#include "GLRenderer.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

bool GLFBOSurfaceRenderTarget::available(GLRenderer *renderer){
	#if defined(TOADLET_HAS_GLEW)
		return GLEW_EXT_framebuffer_object>0;
	#elif defined(TOADLET_HAS_EAGL)
		return true;
	#else
		return false;
	#endif
}

GLFBOSurfaceRenderTarget::GLFBOSurfaceRenderTarget(GLRenderer *renderer):GLRenderTarget(),
	mRenderer(NULL),
	mWidth(0),
	mHeight(0),
	mHandle(0),
	mNeedsCompile(false)
	//mSurfaces
{
	mRenderer=renderer;
}

GLFBOSurfaceRenderTarget::~GLFBOSurfaceRenderTarget(){
	destroy();
}

bool GLFBOSurfaceRenderTarget::create(){
	destroy();

	mWidth=0;
	mHeight=0;
	mNeedsCompile=true;

	glGenFramebuffers(1,&mHandle);

	TOADLET_CHECK_GLERROR("GLFBOSurfaceRenderTarget::create");

	return true;
}

bool GLFBOSurfaceRenderTarget::destroy(){
	if(mRenderer==NULL || mRenderer->getRenderTarget()->getRootRenderTarget()==(GLRenderTarget*)this){
		glBindFramebuffer(GL_FRAMEBUFFER,0);
	}

	int i;
	for(i=0;i<mOwnedSurfaces.size();++i){
		mOwnedSurfaces[i]->destroy();
	}
	mOwnedSurfaces.clear();

	if(mHandle!=0){
		glDeleteFramebuffers(1,&mHandle);
		mHandle=0;
	}

	TOADLET_CHECK_GLERROR("GLFBOSurfaceRenderTarget::destroy");

	return true;
}

bool GLFBOSurfaceRenderTarget::makeCurrent(){
	if(mNeedsCompile){
		compile();
	}

	glBindFramebuffer(GL_FRAMEBUFFER,mHandle);

	TOADLET_CHECK_GLERROR("GLFBOSurfaceRenderTarget::makeCurrent");

	return true;
}

bool GLFBOSurfaceRenderTarget::swap(){
	glBindFramebuffer(GL_FRAMEBUFFER,0);

	TOADLET_CHECK_GLERROR("GLFBOSurfaceRenderTarget::swap");

	return true;
}

bool GLFBOSurfaceRenderTarget::attach(Surface::ptr surface,Attachment attachment){
	GLSurface *glsurface=(GLSurface*)surface->getRootSurface();

	GLTextureMipSurface *textureSurface=glsurface->castToGLTextureMipSurface();
	GLFBORenderbufferSurface *renderbufferSurface=glsurface->castToGLFBORenderbufferSurface();

	glBindFramebuffer(GL_FRAMEBUFFER,mHandle);
	if(textureSurface!=NULL){
		GLTexture *texture=textureSurface->getTexture();
		GLuint handle=texture->getHandle();
		GLenum target=texture->getTarget();
		GLuint level=textureSurface->getLevel();
		mWidth=texture->getWidth();
		mHeight=texture->getHeight();

		glFramebufferTexture2D(GL_FRAMEBUFFER,getGLAttachment(attachment),target,handle,level);
		// TODO: Figure out EXACTLY when we need these and when we dont, I think we just need them if its ONLY a SHADOWMAP
		#if !defined(TOADLET_HAS_GLES)
			//glDrawBuffer(GL_NONE);
			//glReadBuffer(GL_NONE);
		#endif

		Matrix4x4 matrix;
		Math::setMatrix4x4FromScale(matrix,Math::ONE,-Math::ONE,Math::ONE);
		Math::setMatrix4x4FromTranslate(matrix,0,Math::ONE,0);
		texture->setMatrix(matrix);
	}
	else if(renderbufferSurface!=NULL){
		GLuint handle=renderbufferSurface->getHandle();

		glFramebufferRenderbuffer(GL_FRAMEBUFFER,getGLAttachment(attachment),GL_RENDERBUFFER,handle);
	}
	glBindFramebuffer(GL_FRAMEBUFFER,0);

	mSurfaces.add(surface);
	mSurfaceAttachments.add(attachment);
	mNeedsCompile=true;

	TOADLET_CHECK_GLERROR("GLFBOSurfaceRenderTarget::attach");

	return true;
}

bool GLFBOSurfaceRenderTarget::remove(Surface::ptr surface){
	int i;
	for(i=0;i<mSurfaces.size();++i){
		if(mSurfaces[i]==surface){
			break;
		}
	}
	if(i==mSurfaces.size()){
		return false;
	}

	GLSurface *glsurface=(GLSurface*)surface->getRootSurface();

	GLTextureMipSurface *textureSurface=glsurface->castToGLTextureMipSurface();
	GLFBORenderbufferSurface *renderbufferSurface=glsurface->castToGLFBORenderbufferSurface();

	Attachment attachment=mSurfaceAttachments[i];

	glBindFramebuffer(GL_FRAMEBUFFER,mHandle);
	if(textureSurface!=NULL){
		glFramebufferTexture2D(GL_FRAMEBUFFER,getGLAttachment(attachment),GL_TEXTURE_2D,0,0);
	}
	else if(renderbufferSurface!=NULL){
		glFramebufferRenderbuffer(GL_FRAMEBUFFER,getGLAttachment(attachment),GL_RENDERBUFFER,0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER,0);

	mSurfaces.removeAt(i);
	mSurfaceAttachments.removeAt(i);
	mNeedsCompile=true;

	TOADLET_CHECK_GLERROR("GLFBOSurfaceRenderTarget::remove");

	return true;
}

// TODO: Make it check to see if a pre-existing self-created depth surface is compatible, and if not, destroy & recreate it
bool GLFBOSurfaceRenderTarget::compile(){
	Surface::ptr depth;
	Surface::ptr color;

	int i;
	for(i=0;i<mSurfaceAttachments.size();++i){
		if(mSurfaceAttachments[i]==Attachment_DEPTH_STENCIL){
			depth=mSurfaces[i];
		}
		else{
			color=mSurfaces[i];
		}
	}

	if(color!=NULL && depth==NULL){
		// No Depth-Stencil surface, so add one
		attach(createBufferSurface(Texture::Format_DEPTH_16,mWidth,mHeight),Attachment_DEPTH_STENCIL);
	}

	glBindFramebuffer(GL_FRAMEBUFFER,mHandle);
	GLenum status=glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status!=GL_FRAMEBUFFER_COMPLETE){
		Logger::log(Categories::TOADLET_PEEPER,Logger::Level_WARNING,getFBOMessage(status));
	}
	glBindFramebuffer(GL_FRAMEBUFFER,0);

	TOADLET_CHECK_GLERROR("GLFBOSurfaceRenderTarget::compile");

	mNeedsCompile=false;

	return status==GL_FRAMEBUFFER_COMPLETE;
}

Surface::ptr GLFBOSurfaceRenderTarget::createBufferSurface(int format,int width,int height){
	GLFBORenderbufferSurface::ptr surface(new GLFBORenderbufferSurface(this));
	if(surface->create(format,width,height)==false){
		return NULL;
	}
	mOwnedSurfaces.add(surface);
	return surface;
}

GLenum GLFBOSurfaceRenderTarget::getGLAttachment(Attachment attachment){
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
				"GLFBOSurfaceRenderTarget::getGLAttachment: Invalid attachment");
			return 0;
	};
}

const char *GLFBOSurfaceRenderTarget::getFBOMessage(GLenum status){
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
