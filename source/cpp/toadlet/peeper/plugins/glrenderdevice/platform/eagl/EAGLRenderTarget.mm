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

#include "EAGLRenderTarget.h"
#include "../../GLTexture.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Log.h>
#include <toadlet/peeper/Texture.h>
#include <toadlet/peeper/TextureFormat.h>
#import <QuartzCore/QuartzCore.h>

namespace toadlet{
namespace peeper{

#if defined(TOADLET_HAS_GLES)
	#if defined(TOADLET_HAS_GL_20)
		TOADLET_C_API RenderTarget *new_EAGL2RenderTarget(void *display,void *layer,WindowRenderTargetFormat *format,RenderTarget *shareTarget){
			format->flags=2;
			return (GLRenderTarget*)(new EAGLRenderTarget((CAEAGLLayer*)layer,format,shareTarget));
		}
	#else
		TOADLET_C_API RenderTarget *new_EAGL1RenderTarget(void *display,void *layer,WindowRenderTargetFormat *format,RenderTarget *shareTarget){
			format->flags=1;
			return (GLRenderTarget*)(new EAGLRenderTarget((CAEAGLLayer*)layer,format,shareTarget));
		}
	#endif
#else
	TOADLET_C_API RenderTarget *new_EAGLRenderTarget(void *display,void *layer,WindowRenderTargetFormat *format,RenderTarget *shareTarget){
		return (GLRenderTarget*)(new EAGLRenderTarget((CAEAGLLayer*)layer,format,shareTarget));
	}
#endif

EAGLRenderTarget::EAGLRenderTarget():GLFBORenderTarget(NULL),
	mDrawable(nil),
	mContext(nil),
	mRenderBufferHandle(0),
	
	mMSAARenderTarget(NULL),
	mMSAARenderBufferHandle(0),
	mMSAADepthBufferHandle(0)
{
}

EAGLRenderTarget::EAGLRenderTarget(CAEAGLLayer *drawable,WindowRenderTargetFormat *format,RenderTarget *shareTarget):GLFBORenderTarget(NULL),
	mDrawable(nil),
	mContext(nil),
	mRenderBufferHandle(0),
	
	mMSAARenderTarget(NULL),
	mMSAARenderBufferHandle(0),
	mMSAADepthBufferHandle(0)
{
	createContext(drawable,format,shareTarget);
}

void EAGLRenderTarget::destroy(){
	destroyContext();
	
	BaseResource::destroy();
}

bool EAGLRenderTarget::createContext(CAEAGLLayer *drawable,WindowRenderTargetFormat *format,RenderTarget *shareTarget){
	mDrawable=drawable;

	mDrawable.drawableProperties=[NSDictionary dictionaryWithObjectsAndKeys:
		[NSNumber numberWithBool:YES],
		kEAGLDrawablePropertyRetainedBacking,
		format->pixelFormat==TextureFormat::Format_RGB_5_6_5?kEAGLColorFormatRGB565:kEAGLColorFormatRGBA8,
		kEAGLDrawablePropertyColorFormat,
		nil
	];

	EAGLRenderingAPI api=kEAGLRenderingAPIOpenGLES1;
	if(format->flags==2){
		api=kEAGLRenderingAPIOpenGLES2;
	}
	
	if(shareTarget!=NULL){
		EAGLShareGroup *shareGroup=((EAGLRenderTarget*)(shareTarget->getRootRenderTarget()))->mContext.sharegroup
		mContext=[[EAGLContext alloc] initWithAPI:api sharegroup:shareGroup];
	}
	else{
		mContext=[[EAGLContext alloc] initWithAPI:api];
	}
	if(mContext==nil){
		Error::unknown(Categories::TOADLET_PEEPER,
			"Failed to create EAGLContext");
		return false;
	}

	if(![EAGLContext setCurrentContext:mContext]){
		destroyContext();
		return false;
	}

	int width=[drawable bounds].size.width;
	int height=[drawable bounds].size.height;

	String extensions=glGetString(GL_EXTENSIONS);

	GLFBORenderTarget::create();
	glBindFramebuffer(GL_FRAMEBUFFER,mHandle);

	// Manually create the Color Renderbuffer for now
	//  We could add another Renderbuffer storage function for EAGL in
	//  GLFBORenderbufferSurface and then we could just make an instance
	//  of that and attach it.
	glGenRenderbuffers(1,&mRenderBufferHandle);
	glBindRenderbuffer(GL_RENDERBUFFER,mRenderBufferHandle);
	if(![mContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:drawable]){
		destroyContext();
		return false;
	}
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,mRenderBufferHandle);

	if(format->multisamples>0 && extensions.find("GL_APPLE_framebuffer_multisample")>0){
		mMSAARenderTarget=new GLFBORenderTarget(NULL);
		mMSAARenderTarget->create();
		glBindFramebuffer(GL_FRAMEBUFFER,mMSAARenderTarget->getHandle());

		glGenRenderbuffers(1,&mMSAARenderBufferHandle);
		glBindRenderbuffer(GL_RENDERBUFFER,mMSAARenderBufferHandle);
		glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER,format->multisamples,getGLFormatSize(format->pixelFormat),width,height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,mMSAARenderBufferHandle);
		
		glGenRenderbuffers(1,&mMSAADepthBufferHandle);
		glBindRenderbuffer(GL_RENDERBUFFER,mMSAADepthBufferHandle);
		glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER,format->multisamples,GL_DEPTH_COMPONENT16,width,height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,mMSAADepthBufferHandle);
		
		GLenum status=glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if(status!=GL_FRAMEBUFFER_COMPLETE){
			Log::warning(Categories::TOADLET_PEEPER,String("FBO Warning:")+GLFBORenderTarget::getFBOMessage(status));
			mMSAARenderTarget=NULL;
		}
	}
	if(mMSAARenderTarget==NULL){
		/// @todo: Lets change this to not use GLFBOPixelBuffer, since the MSAA stuff doesn't anyways
		if(format->depthBits!=0){
			// No Depth-Stencil buffer, so add one
			GLFBOPixelBuffer::ptr buffer(new GLFBOPixelBuffer(this));
			TextureFormat::ptr bufferFormat(new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_DEPTH_16,width,height,1,1));
			if(buffer->create(Buffer::Usage_NONE,Buffer::Access_NONE,bufferFormat)){
				attach(buffer,Attachment_DEPTH_STENCIL);
				mDepthBuffer=buffer;
			}
		}
	}

	GLFBORenderTarget::activate();

	findAdaptorInfo();
	
	return true;
}

bool EAGLRenderTarget::destroyContext(){
	if(mContext!=nil){
		EAGLContext *oldContext=[EAGLContext currentContext];

		if(oldContext!=mContext){
			[EAGLContext setCurrentContext:mContext];
		}

		GLFBORenderTarget::destroy();
		
		if(mRenderBufferHandle!=0){
			glDeleteRenderbuffers(1,&mRenderBufferHandle);
			mRenderBufferHandle=0;
		}

		if(mMSAARenderTarget!=NULL){
			mMSAARenderTarget->destroy();
			mMSAARenderTarget=NULL;

			if(mMSAARenderBufferHandle!=0){
				glDeleteRenderbuffers(1,&mMSAARenderBufferHandle);
				mMSAARenderBufferHandle=0;
			}
			if(mMSAADepthBufferHandle!=0){
				glDeleteRenderbuffers(1,&mMSAADepthBufferHandle);
				mMSAADepthBufferHandle=0;
			}
		}

		if(oldContext!=mContext){
			[EAGLContext setCurrentContext:oldContext];
		}
		
		[mContext release];
		mContext=nil;
	}
	
	return true;
}

bool EAGLRenderTarget::activate(){
	if(mMSAARenderTarget!=NULL){
		mMSAARenderTarget->activate();
	}
	else{
		GLFBORenderTarget::activate();
	}
	
	return [EAGLContext setCurrentContext:mContext];
}

bool EAGLRenderTarget::deactivate(){
	if(mMSAARenderTarget!=NULL){
		mMSAARenderTarget->deactivate();
	}
	else{
		GLFBORenderTarget::deactivate();
	}

	return [EAGLContext setCurrentContext:nil];
}

bool EAGLRenderTarget::swap(){
	if(mMSAARenderTarget!=NULL){
		GLenum attachments[]={GL_DEPTH_ATTACHMENT};
		glDiscardFramebufferEXT(GL_READ_FRAMEBUFFER_APPLE,1,attachments);
	
		glBindFramebuffer(GL_READ_FRAMEBUFFER_APPLE,mMSAARenderTarget->getHandle());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER_APPLE,mHandle);
	
		glResolveMultisampleFramebufferAPPLE();
	}

	glBindRenderbuffer(GL_RENDERBUFFER,mRenderBufferHandle);
	return [mContext presentRenderbuffer:GL_RENDERBUFFER];
}

int EAGLRenderTarget::getWidth() const{
	return [mDrawable bounds].size.width;
}

int EAGLRenderTarget::getHeight() const{
	return [mDrawable bounds].size.height;
}

GLuint EAGLRenderTarget::getGLFormatSize(int textureFormat){
	switch(textureFormat){
		case TextureFormat::Format_RGB_8:
			return GL_RGB8;
		case TextureFormat::Format_RGB_5_6_5:
			return GL_RGB565;
		case TextureFormat::Format_RGBA_8:
			return GL_RGBA8;
		case TextureFormat::Format_RGBA_5_5_5_1:
			return GL_RGB5_A1;
		case TextureFormat::Format_RGBA_4_4_4_4:
			return GL_RGBA4;
	}
	return 0;		
}
	
}
}

