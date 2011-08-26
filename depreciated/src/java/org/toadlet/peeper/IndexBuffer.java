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

package org.toadlet.peeper;

#include <org/toadlet/peeper/Types.h>

import org.toadlet.egg.Categories;
import org.toadlet.egg.Error;

public final class IndexBuffer extends Buffer{
	// Convinently equals size of index
	public enum IndexFormat{
		empty_for_ordinal0,
		UINT_8, // 1
		UINT_16, // 2
		empty_for_ordinal3,
		UINT_32, // 4
	}

	public IndexBuffer(UsageType usageType,AccessType accessType,IndexFormat indexFormat,int size){
		mType=Type.INDEX;
		mUsageType=usageType;
		mAccessType=accessType;
		mIndexFormat=indexFormat;
		mSize=size;
		mBufferSize=mIndexFormat.ordinal()*mSize;

		mData=java.nio.ByteBuffer.allocateDirect(mBufferSize);
		mData.order(java.nio.ByteOrder.nativeOrder());
	}

	public IndexBuffer(Renderer renderer,UsageType usageType,AccessType accessType,IndexFormat indexFormat,int size){
		mType=Type.INDEX;
		mUsageType=usageType;
		mAccessType=accessType;
		mIndexFormat=indexFormat;
		mSize=size;
		mBufferSize=mIndexFormat.ordinal()*mSize;

		mBufferPeer=renderer.createBufferPeer(this);
		if(mBufferPeer==null || mBufferPeer.supportsRead()==false){
			mData=java.nio.ByteBuffer.allocateDirect(mBufferSize);
			mData.order(java.nio.ByteOrder.nativeOrder());
		}
	}

	public IndexFormat getIndexFormat(){return mIndexFormat;}

	public int getSize(){return mSize;}

	public IndexBuffer clone(){
		IndexBuffer buffer=new IndexBuffer(mUsageType,mAccessType,mIndexFormat,mSize);

		java.nio.ByteBuffer srcData=lock(LockType.READ_ONLY);
		java.nio.ByteBuffer dstData=buffer.lock(LockType.WRITE_ONLY);

		dstData.put(srcData);
		srcData.rewind();
		dstData.rewind();

		buffer.unlock();
		unlock();

		return buffer;
	}

	public IndexBuffer cloneWithNewParameters(UsageType usageType,AccessType accessType,IndexFormat indexFormat,int size){
		#if defined(TOADLET_DEBUG)
			{
				if(indexFormat!=mIndexFormat){
					Error.invalidParameters(Categories.TOADLET_PEEPER,
						"cloneWithNewParameters does not support changing size of elements");
					return null;
				}
			}
		#endif

		IndexBuffer buffer=new IndexBuffer(usageType,accessType,indexFormat,size);

		int numElements=mSize<size?mSize:size;
		java.nio.ByteBuffer srcData=lock(LockType.READ_ONLY);
		java.nio.ByteBuffer dstData=buffer.lock(LockType.WRITE_ONLY);

		dstData.put(srcData);
		srcData.rewind();
		dstData.rewind();

		buffer.unlock();
		unlock();

		return buffer;
	}

	protected IndexFormat mIndexFormat=IndexFormat.UINT_8;
	protected int mSize=0;	
}

