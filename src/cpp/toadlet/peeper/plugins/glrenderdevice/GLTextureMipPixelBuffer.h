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

#ifndef TOADLET_PEEPER_GLTEXTUREMIPPIXELBUFFER_H
#define TOADLET_PEEPER_GLTEXTUREMIPPIXELBUFFER_H

#include "GLIncludes.h"
#include "GLPixelBuffer.h"

namespace toadlet{
namespace peeper{

class GLTexture;
class GLRenderDevice;

class TOADLET_API GLTextureMipPixelBuffer:public GLPixelBuffer{
public:
	TOADLET_SHARED_POINTERS(GLTextureMipPixelBuffer);

protected:
	GLTextureMipPixelBuffer(GLTexture *texture,GLuint level,GLuint cubeSide);

public:
	virtual ~GLTextureMipPixelBuffer(){}

	void setBufferDestroyedListener(BufferDestroyedListener *listener){}

	PixelBuffer *getRootPixelBuffer(){return this;}

	GLTextureMipPixelBuffer *castToGLTextureMipPixelBuffer(){return this;}
	GLFBOPixelBuffer *castToGLFBOPixelBuffer(){return NULL;}
	GLBuffer *castToGLBuffer(){return NULL;}

	bool create(int usage,int access,int pixelFormat,int width,int height,int depth){return false;}
	void destroy(){}

	void resetCreate(){}
	void resetDestroy(){}

	/// @todo: implement
	int getUsage() const{return 0;}
	int getAccess() const{return 0;}
	int getDataSize() const{return mDataSize;}

	int getPixelFormat() const;
	int getWidth() const{return mWidth;}
	int getHeight() const{return mHeight;}
	int getDepth() const{return mDepth;}

	inline GLTexture *getTexture() const{return mTexture;}
	inline GLuint getLevel() const{return mLevel;}
	inline GLuint getCubeSide() const{return mCubeSide;}
	GLuint getHandle() const;
	GLuint getTarget() const;

	/// @todo: Implement these
	uint8 *lock(int access){return NULL;}
	bool unlock(){return false;}

protected:
	GLTexture *mTexture;
	GLuint mLevel;
	GLuint mCubeSide;
	int mDataSize;
	int mWidth,mHeight,mDepth;

	friend class GLTexture;
	friend class GLRenderDevice;
};

}
}

#endif
