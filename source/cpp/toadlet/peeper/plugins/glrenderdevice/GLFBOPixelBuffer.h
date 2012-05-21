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

#ifndef TOADLET_PEEPER_GLFBOPIXELBUFFER_H
#define TOADLET_PEEPER_GLFBOPIXELBUFFER_H

#include "GLPixelBuffer.h"
#include <toadlet/egg/BaseResource.h>

#if defined(TOADLET_HAS_GLFBOS)

namespace toadlet{
namespace peeper{

class GLFBORenderTarget;

class GLFBOPixelBuffer:public BaseResource,public GLPixelBuffer{
public:
	TOADLET_RESOURCE(GLFBOPixelBuffer,GLPixelBuffer);

	GLFBOPixelBuffer(GLFBORenderTarget *target);

	virtual ~GLFBOPixelBuffer();

	PixelBuffer *getRootPixelBuffer(){return this;}
	GLTextureMipPixelBuffer *castToGLTextureMipPixelBuffer(){return NULL;}
	GLFBOPixelBuffer *castToGLFBOPixelBuffer(){return this;}
	GLBuffer *castToGLBuffer(){return NULL;}

	bool create(int usage,int access,TextureFormat::ptr format);
	void destroy();

	void resetCreate(){}
	void resetDestroy(){}

	int getUsage() const{return mUsage;}
	int getAccess() const{return mAccess;}
	int getDataSize() const{return mDataSize;}

	TextureFormat::ptr getTextureFormat() const{return mFormat;}
	
	/// @todo: Can these be implemented
	tbyte *lock(int access){return NULL;}
	bool unlock(){return false;}
	bool update(tbyte *data,int start,int size){return false;}

	inline GLuint getHandle() const{return mHandle;}

protected:
	GLFBORenderTarget *mTarget;
	GLuint mHandle;
	int mUsage;
	int mAccess;
	TextureFormat::ptr mFormat;
	int mDataSize;

	friend class GLFBORenderTarget;
};

}
}

#endif

#endif
