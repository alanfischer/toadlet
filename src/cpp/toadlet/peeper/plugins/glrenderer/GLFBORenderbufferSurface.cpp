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

#include "GLFBORenderbufferSurface.h"
#include "GLFBORenderTarget.h"
#include "GLTexture.h"

namespace toadlet{
namespace peeper{

GLFBORenderbufferSurface::GLFBORenderbufferSurface(GLFBORenderTarget *target):GLSurface(),
	mTarget(NULL),
	mHandle(0)
	mFormat(0),
	mWidth(0),
	mHeight(0)
{
	mTarget=target;
}

bool GLFBORenderbufferSurface::create(int format,int width,int height){
	mFormat=format;
	mWidth=width;
	mHeight=height;

	glGenRenderbuffers(1,&mHandle);
	glBindRenderbuffer(GL_RENDERBUFFER,mHandle);
	glRenderbufferStorage(GL_RENDERBUFFER,GLTexture::getGLFormat(mFormat),mWidth,mHeight);

	TOADLET_CHECK_GLERROR("GLFBORenderbufferSurface::create");

	return true;
}

bool GLFBORenderbufferSurface::destroy(){
	if(mHandle!=0){
		glDeleteRenderbuffers(1,&mHandle);
		mHandle=0;

		TOADLET_CHECK_GLERROR("GLFBORenderbufferSurface::destroy");
	}

	return true;
}

}
}
