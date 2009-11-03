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

	virtual bool create(int usageFlags,AccessType accessType,IndexFormat indexFormat,int size);
	virtual bool create(int usageFlags,AccessType accessType,VertexFormat::ptr vertexFormat,int size);
	virtual void destroy();

	virtual bool createContext();
	virtual void destroyContext(bool backData);
	virtual bool contextNeedsReset(){return false;}

	virtual int getUsageFlags() const{return mUsageFlags;}
	virtual AccessType getAccessType() const{return mAccessType;}
	virtual int getDataSize() const{return mDataSize;}
	virtual int getSize() const{return mSize;}

	virtual IndexFormat getIndexFormat(){return mIndexFormat;}
	virtual VertexFormat::ptr getVertexFormat(){return mVertexFormat;}

	virtual uint8 *lock(AccessType accessType);
	virtual bool unlock();

protected:
	static GLenum getBufferUsage(int usageFlags,AccessType accessType);

	GLRenderer *mRenderer;

	int mUsageFlags;
	AccessType mAccessType;
	int mSize;
	int mDataSize;

	IndexFormat mIndexFormat;
	VertexFormat::ptr mVertexFormat;
	short mVertexSize;
	egg::Collection<VertexElement> mColorElements;

	GLuint mHandle;
	GLenum mTarget;
	AccessType mLockType;
	bool mMapping;
	bool mBacking;
	uint8 *mData;

	friend class GLRenderer;
};

}
}

#endif
