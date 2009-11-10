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

#ifndef TOADLET_PEEPER_GLFBORENDERBUFFERSURFACE_H
#define TOADLET_PEEPER_GLFBORENDERBUFFERSURFACE_H

#include "GLSurface.h"

namespace toadlet{
namespace peeper{

class GLFBOSurfaceRenderTarget;

class GLFBORenderbufferSurface:public GLSurface{
public:
	TOADLET_SHARED_POINTERS(GLFBORenderbufferSurface);

protected:
	GLFBORenderbufferSurface(GLFBOSurfaceRenderTarget *target);

	virtual bool create(int format,int width,int height);
	virtual bool destroy();

public:
	virtual ~GLFBORenderbufferSurface();

	virtual Surface *getRootSurface(){return this;}
	virtual GLTextureMipSurface *castToGLTextureMipSurface(){return NULL;}
	virtual GLFBORenderbufferSurface *castToGLFBORenderbufferSurface(){return this;}

	virtual int getWidth() const{return mWidth;}
	virtual int getHeight() const{return mHeight;}
	inline GLuint getHandle() const{return mHandle;}

	// TODO: Implement these
	virtual int getUsageFlags() const{return 0;}
	virtual AccessType getAccessType() const{return AccessType_NO_ACCESS;}
	virtual int getDataSize() const{return 0;}
	virtual uint8 *lock(AccessType accessType){return NULL;}
	virtual bool unlock(){return false;}

protected:
	GLFBOSurfaceRenderTarget *mTarget;
	GLuint mHandle;
	int mFormat;
	int mWidth;
	int mHeight;

	friend class GLFBOSurfaceRenderTarget;
};

}
}

#endif
