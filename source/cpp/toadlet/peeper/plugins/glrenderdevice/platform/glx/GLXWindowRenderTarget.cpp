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

#include "GLXWindowRenderTarget.h"
#include <toadlet/egg/System.h>
#include <toadlet/peeper/TextureFormat.h>

namespace toadlet{
namespace peeper{

TOADLET_C_API RenderTarget *new_GLXWindowRenderTarget(void *display,void *window,WindowRenderTargetFormat *format){
	return new GLXWindowRenderTarget((Display*)display,(Window)window,format);
}

GLXWindowRenderTarget::GLXWindowRenderTarget()
	//mThreadContexts,
	//mThreadIDs
{}

GLXWindowRenderTarget::GLXWindowRenderTarget(Display *display,Window window,WindowRenderTargetFormat *format)
	//mThreadContexts,
	//mThreadIDs
{
	createContext(display,window,format);
}

void GLXWindowRenderTarget::destroy(){
	destroyContext();
	
	BaseResource::destroy();
}

bool GLXWindowRenderTarget::createContext(Display *display,Window window,WindowRenderTargetFormat *format){
	mDrawable=window;
	mDisplay=display;

	XWindowAttributes attributes;
	XGetWindowAttributes(display,window,&attributes);
	int screen=XScreenNumberOfScreen(attributes.screen);
	
	int attribList[]={
		GLX_RGBA,
		GLX_DOUBLEBUFFER,
		GLX_RED_SIZE,TextureFormat::getRedBits(format->pixelFormat),
		GLX_GREEN_SIZE,TextureFormat::getGreenBits(format->pixelFormat),
		GLX_BLUE_SIZE,TextureFormat::getBlueBits(format->pixelFormat),
		GLX_DEPTH_SIZE,format->depthBits,
		GLX_STENCIL_SIZE,format->stencilBits,
		None
	};
	XVisualInfo *visualInfo=glXChooseVisual(display,screen,attribList);
	if(!visualInfo){
		Error::unknown(Categories::TOADLET_PEEPER,
			"glXChooseVisual failed");
		return false;
	}

	mContext=glXCreateContext(mDisplay,visualInfo,NULL,True);
	if(!mContext){
		Error::unknown(Categories::TOADLET_PEEPER,
			"glXCreateContext failed");
		return false;
	}

	glXMakeCurrent(mDisplay,mDrawable,mContext);
	
	int numThreads=format->threads<=1?0:format->threads-1;
	mThreadContexts.resize(numThreads,0);
	mThreadIDs.resize(numThreads,0);
	int i;
	for(i=0;i<numThreads;++i){
		GLXContext context=glXCreateContext(mDisplay,visualInfo,mContext,True);
		if(context==0){
			destroyContext();
			Error::unknown(Categories::TOADLET_PEEPER,
				"Failed to create threaded GL rendering context");
			return false;
		}
		mThreadContexts[i]=context;
	}

	if(glXIsDirect(mDisplay,mContext)){
		Log::alert(Categories::TOADLET_PEEPER,
			"Using Direct Rendering");
	}
	else{
		Log::alert(Categories::TOADLET_PEEPER,
			"No Direct Rendering possible");
	}

	findAdaptorInfo();

	return true;
}

bool GLXWindowRenderTarget::destroyContext(){
	if(mContext!=NULL){
		glXMakeCurrent(mDisplay,None,NULL);
		glXDestroyContext(mDisplay,mContext);
		mContext=NULL;
	}

	return true;
}

bool GLXWindowRenderTarget::activateAdditionalContext(){
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

	Bool result=glXMakeCurrent(mDisplay,mDrawable,mThreadContexts[i]);
	if(result==True){
		mThreadIDs[i]=threadID;
	}
	
	return result==True;
}

void GLXWindowRenderTarget::deactivateAdditionalContext(){
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
	
	glXMakeCurrent(mDisplay,None,NULL);
	mThreadIDs[i]=0;
}

bool GLXWindowRenderTarget::swap(){
	glXSwapBuffers(mDisplay,mDrawable);
	return true;
}

int GLXWindowRenderTarget::getWidth() const{
	unsigned int width=0;
	glXQueryDrawable(mDisplay,mDrawable,GLX_WIDTH,&width);
	return width;
}

int GLXWindowRenderTarget::getHeight() const{
	unsigned int height=0;
	glXQueryDrawable(mDisplay,mDrawable,GLX_HEIGHT,&height);
	return height;
}

}
}
