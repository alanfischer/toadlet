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

#include "GLFBOPixelBuffer.h"
#include "GLFBORenderTarget.h"
#include "GLRenderDevice.h"
#include <toadlet/egg/image/ImageFormatConversion.h>

namespace toadlet{
namespace peeper{

GLFBOPixelBuffer::GLFBOPixelBuffer(GLFBORenderTarget *target):GLPixelBuffer(),
	mListener(NULL),
	mTarget(NULL),
	mHandle(0),
	mPixelFormat(0),
	mUsage(0),
	mAccess(0),
	mDataSize(0),
	mWidth(0),mHeight(0)
{
	mTarget=target;
}

GLFBOPixelBuffer::~GLFBOPixelBuffer(){
	destroy();
}

bool GLFBOPixelBuffer::create(int usage,int access,int pixelFormat,int width,int height,int depth){
	TOADLET_CHECK_GLERROR("entering GLFBOPixelBuffer::create");

	mUsage=usage;
	mAccess=access;
	mPixelFormat=pixelFormat;
	mWidth=width;
	mHeight=height;
	mDataSize=ImageFormatConversion::getRowPitch(mPixelFormat,mWidth)*mHeight;

	glGenRenderbuffers(1,&mHandle);
	glBindRenderbuffer(GL_RENDERBUFFER,mHandle);
	glRenderbufferStorage(GL_RENDERBUFFER,GLRenderDevice::getGLFormat(mPixelFormat,true),mWidth,mHeight);

	TOADLET_CHECK_GLERROR("GLFBOPixelBuffer::create");

	return true;
}

void GLFBOPixelBuffer::destroy(){
	if(mHandle!=0){
		glDeleteRenderbuffers(1,&mHandle);
		mHandle=0;

		// Check this only if we had a handle, to eliminate errors at shutdown
		TOADLET_CHECK_GLERROR("GLFBOPixelBuffer::destroy");
	}

	if(mListener!=NULL){
		mListener->bufferDestroyed(this);
		mListener=NULL;
	}
}

}
}
