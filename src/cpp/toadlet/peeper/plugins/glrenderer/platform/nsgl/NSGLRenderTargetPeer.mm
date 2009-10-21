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

#include "NSGLRenderTargetPeer.h"
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

NSGLRenderTargetPeer::NSGLRenderTargetPeer():
	mView(nil),
	mPixelFormat(nil),
	mContext(nil)
{
}

NSGLRenderTargetPeer::NSGLRenderTargetPeer(NSView *view,const Visual &visual,NSOpenGLPixelFormat *pixelFormat):
	mView(nil),
	mPixelFormat(nil),
	mContext(nil)
{
	createContext(view,visual,pixelFormat);
}

NSGLRenderTargetPeer::NSGLRenderTargetPeer(NSOpenGLContext *context):
	mView(nil),
	mPixelFormat(nil),
	mContext(nil)
{
	mContext=[context retain];
}

NSGLRenderTargetPeer::~NSGLRenderTargetPeer(){
	destroyContext();
}

bool NSGLRenderTargetPeer::createContext(NSView *view,const Visual &visual,NSOpenGLPixelFormat *pixelFormat){
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

void NSGLRenderTargetPeer::destroyContext(){
	if(mContext!=nil){
		[mContext release];
		mContext=nil;
	}
}

void NSGLRenderTargetPeer::makeCurrent(){
	// If setView is called before the view is ready, this will fail
	//  so we check to see if it needs to be done each time.
	if([mContext view]==nil && mView!=nil){
		[mContext setView:mView];
	}

	[mContext makeCurrentContext];
}

int NSGLRenderTargetPeer::getWidth() const{
	return [[mContext view] bounds].size.width;
}

int NSGLRenderTargetPeer::getHeight() const{
	return [[mContext view] bounds].size.height;
}

void NSGLRenderTargetPeer::swap(){
	[mContext flushBuffer];
}

}
}

