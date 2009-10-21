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

#include <toadlet/peeper/Buffer.h>
#include <toadlet/peeper/Renderer.h>
#include <string.h> // memcpy

namespace toadlet{
namespace peeper{

Buffer::Buffer():
	mType(Type_INDEX),
	mUsageType(UsageType_STATIC),
	mBufferSize(0),
	mRememberContents(true),
	mLockCount(0),
	mLockedData(NULL),

	mBufferPeer(NULL),
	mOwnsBufferPeer(false),

	mData(NULL),
	mHasBeenLocked(false)
{}

Buffer::~Buffer(){
	mLockedData=NULL;

	if(mOwnsBufferPeer && mBufferPeer!=NULL){
		delete mBufferPeer;
	}

	if(mData!=NULL){
		delete[] mData;
		mData=NULL;
	}
}

uint8 *Buffer::lock(LockType lockType){
	// mHasBeenLocked is a Software GLBuffer Readback problem hack.
	if(mBufferPeer!=NULL && (mBufferPeer->supportsRead() || mHasBeenLocked)){
		if(mLockCount==0){
			mLockedData=mBufferPeer->lock(lockType);
			if(mLockedData!=NULL){
				mLockCount++;
			}
			return mLockedData;
		}
		else{
			mLockCount++;
			return mLockedData;
		}
	}
	else{
		return mData;
	}
}

bool Buffer::unlock(){
	if(mBufferPeer!=NULL){
		if(mBufferPeer->supportsRead()==false && mHasBeenLocked==false){
			mHasBeenLocked=true;
			uint8 *data=mBufferPeer->lock(Buffer::LockType_WRITE_ONLY);
			memcpy(data,mData,mBufferSize);
			mBufferPeer->unlock();
			return true;
		}

		if(mLockCount==1){
			if(mRememberContents && mBufferPeer->supportsRead()==false){
				memcpy(mData,mLockedData,mBufferSize);
			}
			bool result=mBufferPeer->unlock();
			if(result){
				mLockCount--;
			}
			mLockedData=NULL;
			return result;
		}
		else{
			mLockCount--;
			return true;
		}
	}
	else{
		return true;
	}
}

void Buffer::internal_setBufferPeer(BufferPeer *bufferPeer,bool own){
	// Make a copy of the data locally, so it can be restored if necessary
	if(bufferPeer==NULL && mData==NULL && mBufferPeer!=NULL && mBufferPeer->supportsRead()){
		uint8 *data=mBufferPeer->lock(LockType_READ_ONLY);
		mData=new uint8[mBufferSize];
		memcpy(mData,data,mBufferSize);
		mBufferPeer->unlock();
	}

	if(mOwnsBufferPeer && mBufferPeer!=NULL){
		delete mBufferPeer;
	}
	mBufferPeer=bufferPeer;
	mOwnsBufferPeer=own;

	if(mBufferPeer!=NULL && mBufferPeer->supportsRead()){
		delete[] mData;
		mData=NULL;
	}
}

}
}
