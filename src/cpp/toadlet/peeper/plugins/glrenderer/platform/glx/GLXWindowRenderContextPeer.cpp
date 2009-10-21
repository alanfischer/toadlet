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

#include "GLXWindowRenderContextPeer.h"

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

GLXWindowRenderContextPeer::GLXWindowRenderContextPeer():GLXRenderTargetPeer()
	//mVisualInfo
{
}

GLXWindowRenderContextPeer::GLXWindowRenderContextPeer(GLXDrawable drawable,Display *display,XVisualInfo *visualInfo):GLXRenderTargetPeer()
	//mVisualInfo
{
	createContext(drawable,display,visualInfo);
}

GLXWindowRenderContextPeer::~GLXWindowRenderContextPeer(){
	destroyContext();
}

bool GLXWindowRenderContextPeer::createContext(GLXDrawable drawable,Display *display,XVisualInfo *visualInfo){
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
	
	if(glXIsDirect(mDisplay,mContext)){
		Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
			"Using Direct Rendering");
	}
	else{
		Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
			"No Direct Rendering possible");
	}

	return true;
}

bool GLXWindowRenderContextPeer::destroyContext(){
	if(mContext!=NULL){
		glXMakeCurrent(mDisplay,None,NULL);
		glXDestroyContext(mDisplay,mContext);
		mContext=NULL;
	}

	return true;
}

void GLXWindowRenderContextPeer::swap(){
	glXSwapBuffers(mDisplay,mDrawable);
}

int GLXWindowRenderContextPeer::getWidth() const{
	unsigned int width=0;
	glXQueryDrawable(mDisplay,mDrawable,GLX_WIDTH,&width);
	return width;
}

int GLXWindowRenderContextPeer::getHeight() const{
	unsigned int height=0;
	glXQueryDrawable(mDisplay,mDrawable,GLX_HEIGHT,&height);
	return height;
}

}
}
