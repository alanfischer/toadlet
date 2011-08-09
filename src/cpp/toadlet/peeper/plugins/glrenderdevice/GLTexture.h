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
#include "GLTextureMipPixelBuffer.h"
#include <toadlet/egg/BaseResource.h>
#include <toadlet/peeper/Texture.h>

namespace toadlet{
namespace peeper{

class GLRenderDevice;

class TOADLET_API GLTexture:protected BaseResource,public Texture{
	TOADLET_BASERESOURCE_PASSTHROUGH(Texture);
public:
	GLTexture(GLRenderDevice *renderDevice);
	virtual ~GLTexture();

	Texture *getRootTexture(){return this;}

	bool create(int usageFlags,Dimension dimension,int format,int width,int height,int depth,int mipLevels,tbyte *mipDatas[]);
	void destroy();

	void resetCreate(){}
	void resetDestroy(){}

	int getUsage() const{return mUsage;}
	Dimension getDimension() const{return mDimension;}
	int getFormat() const{return mFormat;}
	int getWidth() const{return mWidth;}
	int getHeight() const{return mHeight;}
	int getDepth() const{return mDepth;}
	int getNumMipLevels() const{return mMipLevels;}
	scalar getLength() const{return 0;}

	PixelBuffer::ptr getMipPixelBuffer(int level,int cubeSide);
	bool load(int width,int height,int depth,int mipLevel,tbyte *mipData);
	bool read(int width,int height,int depth,int mipLevel,tbyte *mipData);

	inline GLuint getHandle() const{return mHandle;}
	inline GLenum getTarget() const{return mTarget;}

	inline void setMatrix(const Matrix4x4 &matrix){mMatrix.set(matrix);}
	inline Matrix4x4 getMatrix() const{return mMatrix;}

	bool generateMipLevels();

protected:
	bool createContext(int mipLevels,tbyte *mipDatas[]);
	bool destroyContext();

	GLuint getGLTarget();

	GLRenderDevice *mDevice;

	int mUsage;
	Dimension mDimension;
	int mFormat;
	int mWidth;
	int mHeight;
	int mDepth;
	int mMipLevels;

	GLuint mHandle;
	GLenum mTarget;
	Matrix4x4 mMatrix;
	bool mManuallyGenerateMipLevels;
	Collection<PixelBuffer::ptr> mBuffers;

	friend class GLRenderDevice;
};

}
}

#endif
