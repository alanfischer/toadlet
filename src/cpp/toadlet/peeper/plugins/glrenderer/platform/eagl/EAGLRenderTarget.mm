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
#include <toadlet/egg/Error.h>
#include <toadlet/peeper/Texture.h>

#import <QuartzCore/QuartzCore.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

EAGLRenderTarget::EAGLRenderTarget():GLFBOSurfaceRenderTarget(NULL),
	mDrawable(nil),
	mContext(nil),
	mRenderBufferHandle(0)
{
}

EAGLRenderTarget::EAGLRenderTarget(CAEAGLLayer *drawable,const Visual &visual,NSString *colorFormat):GLFBOSurfaceRenderTarget(NULL),
	mDrawable(nil),
	mContext(nil),
	mRenderBufferHandle(0)
{
	createContext(drawable,visual,colorFormat);
}

EAGLRenderTarget::~EAGLRenderTarget(){
	destroyContext();
}

bool EAGLRenderTarget::createContext(CAEAGLLayer *drawable,const Visual &visual,NSString *colorFormat){
	mDrawable=drawable;

	mDrawable.drawableProperties=[NSDictionary dictionaryWithObjectsAndKeys:
		[NSNumber numberWithBool:YES],
		kEAGLDrawablePropertyRetainedBacking,
		kEAGLColorFormatRGBA8,
		kEAGLDrawablePropertyColorFormat,
		nil
	];

	mContext=[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
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

	GLFBOSurfaceRenderTarget::create();

	glGenRenderbuffers(1,&mRenderBufferHandle);
	glBindRenderbuffer(GL_RENDERBUFFER,mRenderBufferHandle);
	if(![mContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:drawable]){
		destroyContext();
		return false;
	}
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,mRenderBufferHandle);

	if(visual.depthBits){
		attach(createBufferSurface(Texture::Format_DEPTH_16,width,height),Attachment_DEPTH_STENCIL);
	}
	
	compile();

	return true;
}

bool EAGLRenderTarget::destroyContext(){
	if(mContext!=nil){
		EAGLContext *oldContext=[EAGLContext currentContext];

		if(oldContext!=mContext){
			[EAGLContext setCurrentContext:mContext];
		}

		GLFBOSurfaceRenderTarget::destroy();
		
		if(mRenderBufferHandle!=0){
			glDeleteRenderbuffers(1,&mRenderBufferHandle);
			mRenderBufferHandle=0;
		}

		if(oldContext!=mContext){
			[EAGLContext setCurrentContext:oldContext];
		}
		
		[mContext release];
		mContext=nil;
	}
	
	return true;
}

bool EAGLRenderTarget::makeCurrent(){
	return [EAGLContext setCurrentContext:mContext];
}

bool EAGLRenderTarget::swap(){
	glBindRenderbuffer(GL_RENDERBUFFER,mRenderBufferHandle);
	return [mContext presentRenderbuffer:GL_RENDERBUFFER];
}

int EAGLRenderTarget::getWidth() const{
	return [mDrawable bounds].size.width;
}

int EAGLRenderTarget::getHeight() const{
	return [mDrawable bounds].size.height;
}

}
}

