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

#include "GLTextureMipSurface.h"
#include "GLTexture.h"
#include "GLRenderer.h"

namespace toadlet{
namespace peeper{

GLTextureMipSurface::GLTextureMipSurface(GLTexture *texture,GLuint level,GLuint cubeSide):GLSurface(),
	mTexture(NULL),
	mLevel(0),
	mCubeSide(0),
	mWidth(0),
	mHeight(0)
{
	mTexture=texture;
	mLevel=level;
	mCubeSide=cubeSide;

	int l=level;
	int w=texture->getWidth(),h=texture->getHeight();
	while(l>0){
		w/=2; h/=2;
		l--;
	}
	mWidth=w;
	mHeight=h;
}

GLuint GLTextureMipSurface::getHandle() const{
	return mTexture->getHandle();
}

GLuint GLTextureMipSurface::getTarget() const{
	#if !defined(TOADLET_HAS_GLES)
		if(mTexture->getTarget()==GL_TEXTURE_CUBE_MAP){
			return GLRenderer::GLCubeFaces[mCubeSide];
		}
	#endif
	return mTexture->getTarget();
}

}
}
