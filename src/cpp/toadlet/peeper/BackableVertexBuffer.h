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

#ifndef TOADLET_PEEPER_BACKABLEVERTEXBUFFER_H
#define TOADLET_PEEPER_BACKABLEVERTEXBUFFER_H

#include <toadlet/peeper/VertexBuffer.h>

namespace toadlet{
namespace peeper{

class TOADLET_API BackableVertexBuffer:public VertexBuffer{
public:
	TOADLET_SHARED_POINTERS(BackableVertexBuffer,egg::Resource);

	BackableVertexBuffer();
	virtual ~BackableVertexBuffer();

	virtual VertexBuffer *getRootVertexBuffer(){return mBack;}

	virtual bool create(int usageFlags,AccessType accessType,VertexFormat::ptr vertexFormat,int size);
	virtual void destroy();

	virtual bool createContext(){return mBack->createContext();}
	virtual void destroyContext(bool backData){mBack->destroyContext(backData);}
	virtual bool contextNeedsReset(){return mBack->contextNeedsReset();}

	virtual int getUsageFlags() const{return mUsageFlags;}
	virtual AccessType getAccessType() const{return mAccessType;}
	virtual int getDataSize() const{return mDataSize;}
	virtual VertexFormat::ptr getVertexFormat(){return mVertexFormat;}
	virtual int getSize() const{return mSize;}

	virtual uint8 *lock(AccessType accessType);
	virtual bool unlock();

	virtual void setBack(VertexBuffer::ptr back,bool initial=false);
	virtual VertexBuffer::ptr getBack(){return mBack;}
	
protected:
	int mUsageFlags;
	AccessType mAccessType;
	int mDataSize;
	VertexFormat::ptr mVertexFormat;
	int mSize;

	uint8 *mData;
	VertexBuffer::ptr mBack;
};

}
}

#endif
