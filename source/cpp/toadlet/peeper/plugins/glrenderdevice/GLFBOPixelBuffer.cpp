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

#if defined(TOADLET_HAS_GLFBOS)

namespace toadlet{
namespace peeper{

GLFBOPixelBuffer::GLFBOPixelBuffer(GLFBORenderTarget *target):
	mTarget(NULL),
	mHandle(0),
	mUsage(0),
	mAccess(0),
	//mFormat,
	mDataSize(0)
{
	mTarget=target;
}

GLFBOPixelBuffer::~GLFBOPixelBuffer(){
	destroy();
}

bool GLFBOPixelBuffer::create(int usage,int access,TextureFormat::ptr format){
	TOADLET_CHECK_GLERROR("entering GLFBOPixelBuffer::create");

	mUsage=usage;
	mAccess=access;
	mFormat=format;
	mDataSize=format->getDataSize();

	glGenRenderbuffers(1,&mHandle);
	glBindRenderbuffer(GL_RENDERBUFFER,mHandle);
	glRenderbufferStorage(GL_RENDERBUFFER,GLRenderDevice::getGLFormat(format->getPixelFormat(),true),format->getWidth(),format->getHeight());

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

	BaseResource::destroy();
}

}
}

#endif
