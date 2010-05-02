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
#include <toadlet/peeper/IndexBuffer.h>
#include <toadlet/peeper/VertexBuffer.h>
#include <toadlet/egg/Collection.h>

namespace toadlet{
namespace peeper{

class GLRenderer;

// Currently this class inherits all possible types, but perhaps it should just inherit Buffer, and then there would exist subclasses for each type of Buffer
class TOADLET_API GLBuffer:public IndexBuffer,public VertexBuffer{
public:
	GLBuffer(GLRenderer *renderer);
	virtual ~GLBuffer();

	IndexBuffer *getRootIndexBuffer(){return this;}
	VertexBuffer *getRootVertexBuffer(){return this;}

	virtual void setBufferDestroyedListener(BufferDestroyedListener *listener){mListener=listener;}

	virtual bool create(int usage,int access,IndexFormat indexFormat,int size);
	virtual bool create(int usage,int access,VertexFormat::ptr vertexFormat,int size);
	virtual void destroy();

	virtual void resetCreate(){}
	virtual void resetDestroy(){}

	virtual int getUsage() const{return mUsage;}
	virtual int getAccess() const{return mAccess;}
	virtual int getDataSize() const{return mDataSize;}
	virtual int getSize() const{return mSize;}

	virtual IndexFormat getIndexFormat(){return mIndexFormat;}
	virtual VertexFormat::ptr getVertexFormat(){return mVertexFormat;}

	virtual uint8 *lock(int lockAccess);
	virtual bool unlock();

protected:
	bool createContext();
	bool destroyContext();

	static GLenum getBufferUsage(int usage,int access);

	GLRenderer *mRenderer;

	BufferDestroyedListener *mListener;
	int mUsage;
	int mAccess;
	int mSize;
	int mDataSize;

	IndexFormat mIndexFormat;
	VertexFormat::ptr mVertexFormat;
	egg::Collection<byte*> mElementOffsets;

	GLuint mHandle;
	GLenum mTarget;
	int mLockAccess;
	bool mMapping;
	uint8 *mData;

	friend class GLRenderer;
};

}
}

#endif
