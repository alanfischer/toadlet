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
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/System.h>

namespace toadlet{
namespace peeper{

GLXWindowRenderTarget::GLXWindowRenderTarget():GLXRenderTarget(),
	mVisualInfo(NULL)
	//mThreadContexts,
	//mThreadIDs
{}

GLXWindowRenderTarget::GLXWindowRenderTarget(GLXDrawable drawable,Display *display,XVisualInfo *visualInfo):GLXRenderTarget(),
	mVisualInfo(NULL)
	//mThreadContexts,
	//mThreadIDs
{
	createContext(drawable,display,visualInfo);
}

GLXWindowRenderTarget::~GLXWindowRenderTarget(){
	destroy();
}

bool GLXWindowRenderTarget::createContext(GLXDrawable drawable,Display *display,XVisualInfo *visualInfo){
	mDrawable=drawable;
	mDisplay=display;
	mVisualInfo=visualInfo;

	mContext=glXCreateContext(mDisplay,mVisualInfo,NULL,True);
	if(!mContext){
		Error::unknown(Categories::TOADLET_PEEPER,
			"glXCreateContext failed");
		return false;
	}

	glXMakeCurrent(mDisplay,mDrawable,mContext);
	
	/// @todo: Have this accept a WindowRenderTargetFormat, so we can get thread count from there.
	int threads=2;
	int numThreads=threads<=1?0:threads-1;
	mThreadContexts.resize(numThreads,0);
	mThreadIDs.resize(numThreads,0);
	int i;
	for(i=0;i<numThreads;++i){
		GLXContext context=glXCreateContext(mDisplay,mVisualInfo,mContext,True);
		if(context==0){
			destroyContext();
			Error::unknown(Categories::TOADLET_PEEPER,
				"Failed to create threaded GL rendering context");
			return false;
		}
		mThreadContexts[i]=context;
	}

	if(glXIsDirect(mDisplay,mContext)){
		Logger::alert(Categories::TOADLET_PEEPER,
			"Using Direct Rendering");
	}
	else{
		Logger::alert(Categories::TOADLET_PEEPER,
			"No Direct Rendering possible");
	}

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
