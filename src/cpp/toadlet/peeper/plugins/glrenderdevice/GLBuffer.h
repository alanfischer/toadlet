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

#ifndef TOADLET_PEEPER_GLBUFFER_H
#define TOADLET_PEEPER_GLBUFFER_H

#include "GLIncludes.h"
#include "GLPixelBuffer.h"
#include <toadlet/peeper/IndexBuffer.h>
#include <toadlet/peeper/VertexBuffer.h>
#include <toadlet/peeper/ConstantBuffer.h>
#include <toadlet/egg/Collection.h>

namespace toadlet{
namespace peeper{

class GLRenderDevice;

class TOADLET_API GLBuffer:public IndexBuffer,public VertexBuffer,public GLPixelBuffer,public ConstantBuffer{
public:
	GLBuffer(GLRenderDevice *renderDevice);
	virtual ~GLBuffer();

	IndexBuffer *getRootIndexBuffer(){return this;}
	VertexBuffer *getRootVertexBuffer(){return this;}
	PixelBuffer *getRootPixelBuffer(){return this;}
	ConstantBuffer *getRootConstantBuffer(){return this;}

	GLTextureMipPixelBuffer *castToGLTextureMipPixelBuffer(){return NULL;}
	GLFBOPixelBuffer *castToGLFBOPixelBuffer(){return NULL;}
	GLBuffer *castToGLBuffer(){return this;}

	void setBufferDestroyedListener(BufferDestroyedListener *listener){mListener=listener;}

	bool create(int usage,int access,IndexFormat indexFormat,int size);
	bool create(int usage,int access,VertexFormat::ptr vertexFormat,int size);
	bool create(int usage,int access,int pixelFormat,int width,int height,int depth);
	bool create(int usage,int access,int size);
	void destroy();

	void resetCreate(){}
	void resetDestroy(){}

	int getUsage() const{return mUsage;}
	int getAccess() const{return mAccess;}
	int getDataSize() const{return mDataSize;}
	int getSize() const{return mSize;}
	int getWidth() const{return mWidth;}
	int getHeight() const{return mHeight;}
	int getDepth() const{return mDepth;}

	IndexFormat getIndexFormat() const{return mIndexFormat;}
	VertexFormat::ptr getVertexFormat() const{return mVertexFormat;}
	int getPixelFormat() const{return mPixelFormat;}

	uint8 *lock(int lockAccess);
	bool unlock();

	void setConstant(int location,float *data,int size);

protected:
	bool createContext();
	bool destroyContext();

	static GLenum getBufferUsage(int usage,int access);

	GLRenderDevice *mDevice;

	BufferDestroyedListener *mListener;
	int mUsage;
	int mAccess;
	int mDataSize;
	int mSize;
	int mWidth,mHeight,mDepth;

	IndexFormat mIndexFormat;
	VertexFormat::ptr mVertexFormat;
	int mPixelFormat;
	egg::Collection<tbyte*> mElementOffsets;

	GLuint mHandle;
	GLenum mTarget;
	int mLockAccess;
	bool mMapping;
	uint8 *mData;

	egg::Collection<float> mConstants;
	egg::Collection<int> mConstantSizes;

	friend class GLRenderDevice;
};

}
}

#endif
