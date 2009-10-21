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

package com.lightningtoads.toadlet.peeper;

#include <com/lightningtoads/toadlet/peeper/Types.h>

import com.lightningtoads.toadlet.egg.Resource;
import com.lightningtoads.toadlet.egg.image.ImageDefinitions;

public class Buffer implements Resource{
	public enum Type{
		INDEX,
		VERTEX,
	}

	public enum UsageType{
		STATIC,	// Buffer data is never changed
		STREAM,	// Buffer data changes once per frame
		DYNAMIC,	// Buffer data changes frequently
	}

	public enum AccessType{
		READ_ONLY,	// Buffer data is only readable
		WRITE_ONLY,	// Buffer data is only writeable
		READ_WRITE,	// Buffer data is readable & writeable
		NO_ACCESS,	// Buffer data is inaccessable
	}

	public enum LockType{
		READ_ONLY,	// Only read operations are performed on the data
		WRITE_ONLY,	// Only write operations are performed on the data
		READ_WRITE,	// Read & write operations are performed on the data
	};

	public interface Peer{
		public void destroy();
	
		public java.nio.ByteBuffer lock(Buffer.LockType lockType);
		public boolean unlock(java.nio.ByteBuffer data);

		public boolean supportsRead();
	}

	public void destroy(){
		if(mBufferPeer!=null){
			mBufferPeer.destroy();
		}
	}
	
	public Type getType(){return mType;}
	public UsageType getUsageType(){return mUsageType;}
	public AccessType getAccessType(){return mAccessType;}
	public int getBufferSize(){return mBufferSize;}

	// This will force it to remember the contents of a buffer if it is lost somehow, makes updating the buffer more expensive
	public void setRememberContents(boolean remember){mRememberContents=remember;}
	
	public java.nio.ByteBuffer lock(LockType lockType){
		if(mBufferPeer!=null){
			if(mLockCount==0){
				mLockedData=mBufferPeer.lock(lockType);
// Temporary workaround to avoid rewriting buffers, until we get the peer system removed
if(mLockedData==null){
	mLockedData=mData;
	mLockedData.rewind();
}
				if(mLockedData!=null){
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
			mData.rewind();
			return mData;
		}
	}

	public boolean unlock(){
		if(mBufferPeer!=null){
			if(mLockCount==1){
// Unnecessary thx to our workaround until we get peers removed
//				if(mRememberContents && mBufferPeer.supportsRead()==false){
//					mLockedData.rewind();
//					mData.rewind();
//					mData.put(mLockedData);
//					mLockedData.rewind();
//					mData.rewind();
//				}
				boolean result=mBufferPeer.unlock(mLockedData);
				if(result){
					mLockCount--;
				}				
				mLockedData=null;
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

	public void internal_setBufferPeer(Buffer.Peer bufferPeer){
		// Make a copy of the data locally, so it can be restored if necessary
		if(bufferPeer==null && mData==null && mBufferPeer!=null && mBufferPeer.supportsRead()){
			java.nio.ByteBuffer data=mBufferPeer.lock(LockType.READ_ONLY);
			mData=java.nio.ByteBuffer.allocateDirect(mBufferSize).order(java.nio.ByteOrder.nativeOrder());
			mData.put(data);
			mData.rewind();
			mBufferPeer.unlock(data);
		}
		
		if(mBufferPeer!=null){
			mBufferPeer.destroy();
		}

		mBufferPeer=bufferPeer;
// moved initial buffer loading to here
if(bufferPeer!=null){bufferPeer.lock(LockType.WRITE_ONLY);bufferPeer.unlock(mData);}
		
		if(mBufferPeer!=null && mBufferPeer.supportsRead()){
			mData=null;
		}
	}

	public Buffer.Peer internal_getBufferPeer(){return mBufferPeer;}

	// The data variable provides a non-peered method of storing data
	public java.nio.ByteBuffer internal_getData(){return mData;}

	protected Type mType=Type.INDEX;
	protected UsageType mUsageType=UsageType.STATIC;
	protected AccessType mAccessType=AccessType.READ_ONLY;
	protected int mBufferSize=0;
	protected boolean mRememberContents=true;
	protected short mLockCount=0;
	protected java.nio.ByteBuffer mLockedData=null;

	protected Buffer.Peer mBufferPeer=null;

	protected java.nio.ByteBuffer mData=null;
}
