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

#ifndef TOADLET_PEEPER_GLTEXTURE_H
#define TOADLET_PEEPER_GLTEXTURE_H

#include "GLIncludes.h"
#include <toadlet/peeper/Texture.h>
#include <toadlet/peeper/TextureBlend.h>

namespace toadlet{
namespace peeper{

class GLRenderer;

class TOADLET_API GLTexture:public Texture{
public:
	GLTexture(GLRenderer *renderer);

	virtual ~GLTexture();

	virtual Texture *getRootTexture() const{return this;}

	virtual void create(Dimension dimension,int format,int width,int height,int depth);
	virtual void destroy();

	virtual Dimension getDimension() const{return mDimension;}
	virtual int getFormat() const{return mFormat;}
	virtual int getWidth() const{return mWidth;}
	virtual int getHeight() const{return mHeight;}
	virtual int getDepth() const{return mDepth;}

	virtual void load(int format,int width,int height,int depth,uint8 *data);
	virtual bool read(int format,int width,int height,int depth,uint8 *data);

	virtual void setAutoGenerateMipMaps(bool mipmaps);
	virtual bool getAutoGenerateMipMaps() const{return mAutoGenerateMipMaps;}

	virtual void setName(const egg::String &name){mName=name;}
	virtual const egg::String &getName() const{return mName;}

protected:
	void generateMipMaps();
	GLuint getGLTarget();

	static GLuint getGLFormat(int textureFormat);
	static GLuint getGLType(int textureFormat);
	static GLuint getGLWrap(TextureStage::AddressMode addressMode,bool hasClampToEdge=true);
	static GLuint getGLMinFilter(TextureStage::Filter minFilter,TextureStage::Filter mipFilter);
	static GLuint getGLMagFilter(TextureStage::Filter magFilter);
	static GLuint getGLTextureBlendSource(TextureBlend::Source blend);

	GLRenderer *mRenderer;

	Dimension mDimension;
	int mFormat;
	int mWidth;
	int mHeight;
	int mDepth;

	egg::String mName;

	GLuint mHandle;
	GLenum mTarget;
	Matrix4x4 mMatrix;
	bool mAutoGenerateMipMaps;
	bool mManuallyGenerateMipMaps;

	friend GLRenderer;
};

}
}

#endif
