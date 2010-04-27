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

#ifndef TOADLET_PEEPER_GLTEXTUREMIPSURFACE_H
#define TOADLET_PEEPER_GLTEXTUREMIPSURFACE_H

#include "GLIncludes.h"
#include "GLSurface.h"

namespace toadlet{
namespace peeper{

class GLTexture;
class GLRenderer;

class TOADLET_API GLTextureMipSurface:public GLSurface{
public:
	TOADLET_SHARED_POINTERS(GLTextureMipSurface);

protected:
	GLTextureMipSurface(GLTexture *texture,GLuint level,GLuint cubeSide);

public:
	virtual ~GLTextureMipSurface(){}

	virtual Surface *getRootSurface(){return this;}
	virtual GLTextureMipSurface *castToGLTextureMipSurface(){return this;}
	virtual GLFBORenderbufferSurface *castToGLFBORenderbufferSurface(){return NULL;}

	virtual int getWidth() const{return mWidth;}
	virtual int getHeight() const{return mHeight;}

	inline GLTexture *getTexture() const{return mTexture;}
	inline GLuint getLevel() const{return mLevel;}
	inline GLuint getCubeSide() const{return mCubeSide;}
	GLuint getHandle() const;
	GLuint getTarget() const;

	/// @todo: Implement these
	virtual int getUsage() const{return 0;}
	virtual int getAccess() const{return 0;}
	virtual int getDataSize() const{return 0;}
	virtual uint8 *lock(int access){return NULL;}
	virtual bool unlock(){return false;}

protected:
	GLTexture *mTexture;
	GLuint mLevel;
	GLuint mCubeSide;
	int mWidth;
	int mHeight;

	friend class GLTexture;
	friend class GLRenderer;
};

}
}

#endif
