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

import com.lightningtoads.toadlet.egg.Categories;
import com.lightningtoads.toadlet.egg.Error;

public final class IndexBufferAccessor{
	public IndexBufferAccessor(){}

	public IndexBufferAccessor(IndexBuffer buffer,Buffer.LockType lockType){
		lock(buffer,lockType);
	}

	public void destroy(){
		unlock();
	}

	public void lock(IndexBuffer indexBuffer,Buffer.LockType lockType){
		unlock();

		mIndexBuffer=indexBuffer;
		mIndexFormat=mIndexBuffer.getIndexFormat();
		mData=mIndexBuffer.lock(lockType);
	}

	public void unlock(){
		if(mIndexBuffer!=null){
			mIndexBuffer.unlock();
			mIndexBuffer=null;
			mData=null;
		}
	}

	public int getSize(){return mIndexBuffer.getSize();}

	public java.nio.ByteBuffer getData(){return mData;}

	public void set(int i,int x){
		#if defined(TOADLET_DEBUG)
			if(i<0 || i>=getSize()){Error.unknown(Categories.TOADLET_PEEPER,"index out of bounds");return;}
			if(x>=(1<<(8*mIndexFormat.ordinal()))){Error.unknown(Categories.TOADLET_PEEPER,"out of datatype range");return;}
		#endif
		if(mIndexFormat==IndexBuffer.IndexFormat.UINT_8){
			mData.put(i,(byte)x);
		}
		else if(mIndexFormat==IndexBuffer.IndexFormat.UINT_16){
			mData.putShort(i*2,(short)x);
		}
		else{ // IndexBuffer.IndexFormat.UINT_32
			mData.putInt(i*4,x);
		}
	}

	public int get(int i){
		#if defined(TOADLET_DEBUG)
			if(i<0 || i>=getSize()){Error.unknown(Categories.TOADLET_PEEPER,"index out of bounds");return 0;}
		#endif
		if(mIndexFormat==IndexBuffer.IndexFormat.UINT_8){
			return mData.get(i);
		}
		else if(mIndexFormat==IndexBuffer.IndexFormat.UINT_16){
			return mData.getShort(i*2);
		}
		else{ // IndexBuffer.IndexFormat.UINT_32
			return mData.getInt(i*4);
		}
	}

	protected IndexBuffer mIndexBuffer=null;
	protected IndexBuffer.IndexFormat mIndexFormat=IndexBuffer.IndexFormat.UINT_8;
	java.nio.ByteBuffer mData=null;
}
