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
#include <toadlet/egg/System.h>

namespace toadlet{
namespace peeper{

TOADLET_C_API RenderTarget *new_NSGLRenderTarget(void *display,void *view,WindowRenderTargetFormat *format){
	return new NSGLRenderTarget((NSView*)view,format);
}
	
NSGLRenderTarget::NSGLRenderTarget():
	mView(nil),
	mPixelFormat(nil),
	mContext(nil)
{}

NSGLRenderTarget::NSGLRenderTarget(NSView *view,WindowRenderTargetFormat *format,NSOpenGLPixelFormat *pixelFormat):GLRenderTarget(),
	mView(nil),
	mPixelFormat(nil),
	mContext(nil)
{
	createContext(view,format,pixelFormat);
}

NSGLRenderTarget::NSGLRenderTarget(NSOpenGLContext *context):GLRenderTarget(),
	mView(nil),
	mPixelFormat(nil),
	mContext(nil)
{
	mContext=[context retain];
}

NSGLRenderTarget::~NSGLRenderTarget(){
	destroy();
}

void NSGLRenderTarget::destroy(){
	destroyContext();
}

bool NSGLRenderTarget::createContext(NSView *view,WindowRenderTargetFormat *format,NSOpenGLPixelFormat *pixelFormat){
	if(pixelFormat==nil){
		
		
		NSOpenGLPixelFormatAttribute attrs[]={
			NSOpenGLPFADoubleBuffer,
			NSOpenGLPFADepthSize,32,
			(NSOpenGLPixelFormatAttribute)nil
		};
		

		
		mPixelFormat=[[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];

	}
	else{
		mPixelFormat=pixelFormat;
		[mPixelFormat retain];
	}

	mContext=[[NSOpenGLContext alloc] initWithFormat:mPixelFormat shareContext:nil];
	if(mContext==nil){
		destroyContext();
		Error::unknown(Categories::TOADLET_PEEPER,
			"Failed to create GL context");
		return false;
	}

	mView=view;

	int numThreads=format->threads<=1?0:format->threads-1;
	mThreadContexts.resize(numThreads,nil);
	mThreadIDs.resize(numThreads,0);
	int i;
	for(i=0;i<numThreads;++i){
		NSOpenGLContext *context=[[NSOpenGLContext alloc] initWithFormat:mPixelFormat shareContext:mContext];
		if(context==nil){
			destroyContext();
			Error::unknown(Categories::TOADLET_PEEPER,
				"Failed to create threaded GL context");
			return false;
		}
		mThreadContexts[i]=context;
	}

	activate();

	return true;
}

bool NSGLRenderTarget::destroyContext(){
	int i;
	for(i=0;i<mThreadContexts.size();++i){
		if(mThreadContexts[i]!=0){
			[mThreadContexts[i] release];
		}
	}
	mThreadContexts.clear();
	mThreadIDs.clear();

	if(mContext!=nil){
		[mContext release];
		mContext=nil;
	}

	if(mPixelFormat!=nil){
		[mPixelFormat release];
		mPixelFormat=nil;
	}

	return true;
}

bool NSGLRenderTarget::activate(){
	// If setView is called before the view is ready, this will fail
	//  so we check to see if it needs to be done each time.
	if([mContext view]==nil && mView!=nil){
		[mContext setView:mView];
	}

	[mContext makeCurrentContext];
	
	return true;
}

bool NSGLRenderTarget::deactivate(){
	// If setView is called before the view is ready, this will fail
	//  so we check to see if it needs to be done each time.
	if([mContext view]==nil && mView!=nil){
		[mContext setView:mView];
	}

	[NSOpenGLContext clearCurrentContext];
	
	return true;
}

bool NSGLRenderTarget::activateAdditionalContext(){
	int threadID=System::threadID();
	int i;
	for(i=0;i<mThreadIDs.size();++i){
		if(mThreadIDs[i]==threadID){
			return false;
		}
	}
	
	for(i=0;i<mThreadIDs.size();++i){
		if(mThreadIDs[i]==0){
			break;
		}
	}
	if(i==mThreadIDs.size()){
		return false;
	}
	
	[mThreadContexts[i] makeCurrentContext];
	mThreadIDs[i]=threadID;

	return true;
}

void NSGLRenderTarget::deactivateAdditionalContext(){
	int threadID=System::threadID();
	int i;
	for(i=0;i<mThreadIDs.size();++i){
		if(mThreadIDs[i]==threadID){
			break;
		}
	}
	if(i==mThreadIDs.size()){
		return;
	}
	
	[NSOpenGLContext clearCurrentContext];
	mThreadIDs[i]=0;
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

void NSGLRenderTarget::reset(){
	[mContext update];
}

}
}

