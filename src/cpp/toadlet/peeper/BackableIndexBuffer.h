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

#ifndef TOADLET_PEEPER_BACKABLEINDEXBUFFER_H
#define TOADLET_PEEPER_BACKABLEINDEXBUFFER_H

#include <toadlet/peeper/IndexBuffer.h>

namespace toadlet{
namespace peeper{

class TOADLET_API BackableIndexBuffer:public IndexBuffer{
public:
	TOADLET_SHARED_POINTERS(BackableIndexBuffer);

	BackableIndexBuffer();
	virtual ~BackableIndexBuffer();

	virtual IndexBuffer *getRootIndexBuffer(){return mBack;}

	virtual void setBufferDestroyedListener(BufferDestroyedListener *listener){mListener=listener;}

	virtual bool create(int usage,int access,IndexFormat indexFormat,int size);
	virtual void destroy();

	virtual void resetCreate(){}
	virtual void resetDestroy(){}

	virtual int getUsage() const{return mUsage;}
	virtual int getAccess() const{return mAccess;}
	virtual int getDataSize() const{return mDataSize;}
	virtual IndexFormat getIndexFormat(){return mIndexFormat;}
	virtual int getSize() const{return mSize;}

	virtual uint8 *lock(int lockAccess);
	virtual bool unlock();

	virtual void setBack(IndexBuffer::ptr back);
	virtual IndexBuffer::ptr getBack(){return mBack;}
	
protected:
	BufferDestroyedListener *mListener;
	int mUsage;
	int mAccess;
	int mDataSize;
	IndexFormat mIndexFormat;
	int mSize;

	uint8 *mData;
	IndexBuffer::ptr mBack;
};

}
}

#endif
