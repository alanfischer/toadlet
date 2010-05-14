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

#include "NSGLRenderTarget.h"
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

TOADLET_C_API RenderTarget *new_NSGLRenderTarget(NSView *view,const Visual &visual){
	return new NSGLRenderTarget(view,visual);
}
	
NSGLRenderTarget::NSGLRenderTarget():
	mView(nil),
	mPixelFormat(nil),
	mContext(nil)
{}

NSGLRenderTarget::NSGLRenderTarget(NSView *view,const Visual &visual,NSOpenGLPixelFormat *pixelFormat):GLRenderTarget(),
	mView(nil),
	mPixelFormat(nil),
	mContext(nil)
{
	createContext(view,visual,pixelFormat);
}

NSGLRenderTarget::NSGLRenderTarget(NSOpenGLContext *context):GLRenderTarget(),
	mView(nil),
	mPixelFormat(nil),
	mContext(nil)
{
	mContext=[context retain];
}

NSGLRenderTarget::~NSGLRenderTarget(){
	destroyContext();
}

bool NSGLRenderTarget::createContext(NSView *view,const Visual &visual,NSOpenGLPixelFormat *pixelFormat){
	if(pixelFormat==nil){
		NSOpenGLPixelFormatAttribute attrs[]={
			NSOpenGLPFADoubleBuffer,
			NSOpenGLPFADepthSize,visual.depthBits,
			0
		};

		pixelFormat=[[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
	}
	else{
		[pixelFormat retain];
	}

	mContext=[[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];

	mView=view;

	[pixelFormat release];

	makeCurrent();
	
	return mContext!=nil;
}

bool NSGLRenderTarget::destroyContext(){
	if(mContext!=nil){
		[mContext release];
		mContext=nil;
	}
	
	return true;
}

bool NSGLRenderTarget::makeCurrent(){
	// If setView is called before the view is ready, this will fail
	//  so we check to see if it needs to be done each time.
	if([mContext view]==nil && mView!=nil){
		[mContext setView:mView];
	}

	[mContext makeCurrentContext];
	
	return true;
}

bool NSGLRenderTarget::swap(){
	[mContext flushBuffer];
	
	return true;
}

int NSGLRenderTarget::getWidth() const{
	return [[mContext view] bounds].size.width;
}

int NSGLRenderTarget::getHeight() const{
	return [[mContext view] bounds].size.height;
}

}
}

