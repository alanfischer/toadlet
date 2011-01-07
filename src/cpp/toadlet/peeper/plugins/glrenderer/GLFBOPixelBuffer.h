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

#ifndef TOADLET_PEEPER_GLFBOPIXELBUFFER_H
#define TOADLET_PEEPER_GLFBOPIXELBUFFER_H

#include "GLPixelBuffer.h"

namespace toadlet{
namespace peeper{

class GLFBORenderTarget;

class GLFBOPixelBuffer:public GLPixelBuffer{
public:
	TOADLET_SHARED_POINTERS(GLFBOPixelBuffer);

	GLFBOPixelBuffer(GLFBORenderTarget *target);

	virtual ~GLFBOPixelBuffer();

	virtual PixelBuffer *getRootPixelBuffer(){return this;}
	virtual GLTextureMipPixelBuffer *castToGLTextureMipPixelBuffer(){return NULL;}
	virtual GLFBOPixelBuffer *castToGLFBOPixelBuffer(){return this;}
	virtual GLBuffer *castToGLBuffer(){return NULL;}

	virtual void setBufferDestroyedListener(BufferDestroyedListener *listener){}

	virtual bool create(int usage,int access,int pixelFormat,int width,int height,int depth);
	virtual void destroy();

	virtual void resetCreate(){}
	virtual void resetDestroy(){}

	virtual int getUsage() const{return mUsage;}
	virtual int getAccess() const{return mAccess;}
	virtual int getDataSize() const{return mDataSize;}

	virtual int getPixelFormat() const{return mPixelFormat;}
	virtual int getWidth() const{return mWidth;}
	virtual int getHeight() const{return mHeight;}
	virtual int getDepth() const{return 1;}
	
	/// @todo: Can these be implemented
	virtual uint8 *lock(int access){return NULL;}
	virtual bool unlock(){return false;}

	inline GLuint getHandle() const{return mHandle;}

protected:
	GLFBORenderTarget *mTarget;
	GLuint mHandle;
	int mPixelFormat;
	int mUsage;
	int mAccess;
	int mDataSize;
	int mWidth,mHeight;

	friend class GLFBORenderTarget;
};

}
}

#endif
