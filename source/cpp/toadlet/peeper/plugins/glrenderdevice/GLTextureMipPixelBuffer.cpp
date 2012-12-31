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

#include "GLTextureMipPixelBuffer.h"
#include "GLTexture.h"
#include "GLRenderDevice.h"

namespace toadlet{
namespace peeper{

GLTextureMipPixelBuffer::GLTextureMipPixelBuffer(GLTexture *texture,GLuint level,GLuint cubeSide):GLPixelBuffer(),
	mTexture(NULL),
	mLevel(0),
	mCubeSide(0),
	mUsage(0),
	//mFormat,
	mDataSize(0)
{
	mTexture=texture;
	mLevel=level;
	mCubeSide=cubeSide;

	int l=level;
	int w=texture->getFormat()->getWidth(),h=texture->getFormat()->getHeight(),d=texture->getFormat()->getDepth();
	while(l>0){
		w/=2; h/=2; d/=2;
		l--;
	}

	mUsage=texture->getUsage()&(Usage_BIT_STATIC|Usage_BIT_STREAM|Usage_BIT_DYNAMIC|Usage_BIT_STAGING);
	mFormat=new TextureFormat(texture->getFormat());
	mFormat->setSize(w,h,d);
	mDataSize=mFormat->getDataSize();
}

GLuint GLTextureMipPixelBuffer::getHandle() const{
	return mTexture->getHandle();
}

GLuint GLTextureMipPixelBuffer::getTarget() const{
	#if !defined(TOADLET_HAS_GLES)
		if(mTexture->getTarget()==GL_TEXTURE_CUBE_MAP){
			return GLRenderDevice::GLCubeFaces[mCubeSide];
		}
	#endif
	return mTexture->getTarget();
}

}
}
