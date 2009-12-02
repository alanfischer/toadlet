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

#import <QuartzCore/QuartzCore.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

EAGLRenderContext::EAGLRenderContext():
	mDrawable(nil),
	mContext(nil),

	mRenderBuffer(0),
	mFrameBuffer(0),
	mDepthBuffer(0)
{
}

EAGLRenderContext::EAGLRenderContext(CAEAGLLayer *drawable,const Visual &visual,NSString *colorFormat):
	mDrawable(nil),
	mContext(nil),

	mRenderBuffer(0),
	mFrameBuffer(0),
	mDepthBuffer(0)
{
	createContext(drawable,visual,colorFormat);
}

EAGLRenderContext::~EAGLRenderContext(){
	destroyContext();
}

bool EAGLRenderContext::createContext(CAEAGLLayer *drawable,const Visual &visual,NSString *colorFormat){
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
	int depthFormat=visual.depthBits!=0?GL_DEPTH_COMPONENT16:0;
	
	glGenRenderbuffers(1,&mRenderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER,mRenderBuffer);
	if(![mContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:drawable]){
		destroyContext();
		return false;
	}

	glGenFramebuffers(1,&mFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER,mFrameBuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,mRenderBuffer);
	if(depthFormat!=0){
		glGenRenderbuffers(1,&mDepthBuffer);
		glBindRenderbuffer(GL_RENDERBUFFER,mDepthBuffer);
		glRenderbufferStorage(GL_RENDERBUFFER,depthFormat,width,height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,mDepthBuffer);
	}

	return true;
}

bool EAGLRenderContext::destroyContext(){
	if(mContext!=nil){
		EAGLContext *oldContext=[EAGLContext currentContext];

		if(oldContext!=mContext){
			[EAGLContext setCurrentContext:mContext];
		}

		if(mDepthBuffer!=0){
			glDeleteRenderbuffers(1,&mDepthBuffer);
			mDepthBuffer=0;
		}

		if(mRenderBuffer!=0){
			glDeleteRenderbuffers(1,&mRenderBuffer);
			mRenderBuffer=0;
		}

		if(mFrameBuffer!=0){
			glDeleteFramebuffers(1,&mFrameBuffer);
			mFrameBuffer=0;
		}

		if(oldContext!=mContext){
			[EAGLContext setCurrentContext:oldContext];
		}
		
		[mContext release];
		mContext=nil;
	}
	
	return true;
}

bool EAGLRenderContext::makeCurrent(){
	return [EAGLContext setCurrentContext:mContext];
}

bool EAGLRenderContextPeer::swap(){
	glBindRenderbuffer(GL_RENDERBUFFER,mRenderBuffer);
	return [mContext presentRenderbuffer:GL_RENDERBUFFER];
}

int EAGLRenderContextPeer::getWidth() const{
	return [mDrawable bounds].size.width;
}

int EAGLRenderContextPeer::getHeight() const{
	return [mDrawable bounds].size.height;
}

}
}

