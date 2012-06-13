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
#include <toadlet/egg/BaseResource.h>

namespace toadlet{
namespace peeper{

class GLTexture;
class GLRenderDevice;

class TOADLET_API GLTextureMipPixelBuffer:public BaseResource,public GLPixelBuffer{
protected:
	GLTextureMipPixelBuffer(GLTexture *texture,GLuint level,GLuint cubeSide);

public:
	TOADLET_RESOURCE(GLTextureMipPixelBuffer,GLPixelBuffer);

	virtual ~GLTextureMipPixelBuffer(){}

	PixelBuffer *getRootPixelBuffer(){return this;}

	GLTextureMipPixelBuffer *castToGLTextureMipPixelBuffer(){return this;}
	GLFBOPixelBuffer *castToGLFBOPixelBuffer(){return NULL;}
	GLBuffer *castToGLBuffer(){return NULL;}

	bool create(int usage,int access,TextureFormat::ptr format){return false;}
	void destroy(){BaseResource::destroy();}

	void resetCreate(){}
	void resetDestroy(){}

	/// @todo: implement
	int getUsage() const{return 0;}
	int getAccess() const{return 0;}
	TextureFormat::ptr getTextureFormat() const{return mFormat;}
	int getDataSize() const{return mDataSize;}
	int getSize() const{return mFormat->getSize();}

	inline GLTexture *getTexture() const{return mTexture;}
	inline GLuint getLevel() const{return mLevel;}
	inline GLuint getCubeSide() const{return mCubeSide;}
	GLuint getHandle() const;
	GLuint getTarget() const;

	/// @todo: implement
	tbyte *lock(int access){return NULL;}
	bool unlock(){return false;}
	bool update(tbyte *data,int start,int size){return false;}

protected:
	GLTexture *mTexture;
	GLuint mLevel;
	GLuint mCubeSide;
	TextureFormat::ptr mFormat;
	int mDataSize;

	friend class GLTexture;
	friend class GLRenderDevice;
};

}
}

#endif
