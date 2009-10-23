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
	TOADLET_SHARED_POINTERS(GLTextureMipSurface,Surface);

protected:
	GLTextureMipSurface(GLTexture *texture,GLuint level,int width,int height);

public:
	virtual ~GLTextureMipSurface(){}

	virtual Surface *getRootSurface(){return this;}
	virtual GLTextureMipSurface *castToGLTextureMipSurface(){return this;}
	virtual GLFBORenderbufferSurface *castToGLFBORenderbufferSurface(){return NULL;}

	virtual int getWidth() const{return mWidth;}
	virtual int getHeight() const{return mHeight;}

	void resize(GLuint level,int width,int height);

	inline GLTexture *getTexture() const{return mTexture;}
	inline GLuint getLevel() const{return mLevel;}

protected:
	GLTexture *mTexture;
	GLuint mLevel;
	int mWidth;
	int mHeight;

	friend GLTexture;
	friend GLRenderer;
};

}
}

#endif
