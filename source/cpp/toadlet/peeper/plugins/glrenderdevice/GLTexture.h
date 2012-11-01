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
#include <toadlet/egg/Collection.h>
#include <toadlet/peeper/Texture.h>

namespace toadlet{
namespace peeper{

class GLRenderDevice;

class TOADLET_API GLTexture:public BaseResource,public Texture{
public:
	TOADLET_RESOURCE(GLTexture,Texture);

	GLTexture(GLRenderDevice *renderDevice);

	Texture *getRootTexture(){return this;}

	bool create(int usageFlags,TextureFormat::ptr format,tbyte *mipDatas[]);
	void destroy();

	void resetCreate(){}
	void resetDestroy(){}

	int getUsage() const{return mUsage;}
	TextureFormat::ptr getFormat() const{return mFormat;}
	scalar getLength() const{return 0;}

	PixelBuffer::ptr getMipPixelBuffer(int level,int cubeSide);
	bool load(TextureFormat *format,tbyte *data);
	bool read(TextureFormat *format,tbyte *data);

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
	TextureFormat::ptr mFormat;

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
