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

#ifndef TOADLET_PEEPER_GLTEXTUREPEER_H
#define TOADLET_PEEPER_GLTEXTUREPEER_H

#include "GLIncludes.h"
#include <toadlet/peeper/Texture.h>
#include <toadlet/peeper/TextureBlend.h>
#include <toadlet/peeper/CapabilitySet.h>

namespace toadlet{
namespace peeper{

class GLRenderer;

class TOADLET_API GLTexturePeer:public TexturePeer{
public:
	GLTexturePeer(GLRenderer *renderer,Texture *texture);

	virtual ~GLTexturePeer();

	void generateMipMaps();

	virtual bool isValid() const{return true;}

	static GLuint getGLTarget(Texture *texture,const CapabilitySet &capabilitySet);
	static GLuint getGLFormat(int textureFormat);
	static GLuint getGLType(int textureFormat);
	static GLuint getGLWrap(Texture::AddressMode addressMode,bool hasClampToEdge=true);
	static GLuint getGLMinFilter(Texture::Filter minFilter,Texture::Filter mipFilter);
	static GLuint getGLMagFilter(Texture::Filter magFilter);
	static GLuint getGLTextureBlendSource(TextureBlend::Source blend);

	GLRenderer *renderer;
	GLuint textureHandle;
	GLenum textureTarget;
	Matrix4x4 textureMatrix;
	bool manuallyBuildMipMaps;
};

}
}

#endif
