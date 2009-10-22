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

#ifndef TOADLET_PEEPER_BUFFER_H
#define TOADLET_PEEPER_BUFFER_H

#include <toadlet/egg/Resource.h>

namespace toadlet{
namespace peeper{

class Renderer;
class BufferPeer;

class TOADLET_API Buffer:public egg::Resource{
public:
	TOADLET_SHARED_POINTERS(Buffer,egg::Resource);

	enum Type{
		Type_INDEX,
		Type_VERTEX,
	};

	enum UsageType{
		UsageType_STATIC,	// Buffer data is never changed
		UsageType_STREAM,	// Buffer data changes once per frame
		UsageType_DYNAMIC,	// Buffer data changes frequently
	};

	enum AccessType{
		AccessType_READ_ONLY,	// Buffer data is only readable
		AccessType_WRITE_ONLY,	// Buffer data is only writeable
		AccessType_READ_WRITE,	// Buffer data is readable & writeable
		AccessType_NO_ACCESS,	// Buffer data is inaccessable
	};

	enum LockType{
		LockType_READ_ONLY,	// Only read operations are performed on the data
		LockType_WRITE_ONLY,	// Only write operations are performed on the data
		LockType_READ_WRITE,	// Read & write operations are performed on the data
	};

	Buffer();
	virtual ~Buffer();

	Type getType() const{return mType;}
	UsageType getUsageType() const{return mUsageType;}
	AccessType getAccessType() const{return mAccessType;}
	int getBufferSize() const{return mBufferSize;}

	// This will force it to remember the contents of a buffer if it is lost somehow, makes updating the buffer more expensive
	void setRememberContents(bool remember){mRememberContents=remember;}

	uint8 *lock(LockType lockType);
	bool unlock();

	virtual void internal_setBufferPeer(BufferPeer *bufferPeer,bool own);
	inline BufferPeer *internal_getBufferPeer() const{return mBufferPeer;}
	inline bool internal_ownsBufferPeer() const{return mOwnsBufferPeer;}

	// The data variable provides a non-peered method of storing data
	inline uint8 *internal_getData() const{return mData;}

// HACK: Temporary
void setName(const egg::String &name){}
const egg::String &getName() const{static egg::String s;return s;}

protected:
	Type mType;
	UsageType mUsageType;
	AccessType mAccessType;
	int mBufferSize;
	bool mRememberContents;
	short mLockCount;
	uint8 *mLockedData;

	BufferPeer *mBufferPeer;
	bool mOwnsBufferPeer;

	uint8 *mData;
	bool mHasBeenLocked;
};

class BufferPeer{
public:
	virtual ~BufferPeer(){}

	virtual uint8 *lock(Buffer::LockType lockType)=0;
	virtual bool unlock()=0;

	virtual bool supportsRead()=0;
};

}
}

#endif
