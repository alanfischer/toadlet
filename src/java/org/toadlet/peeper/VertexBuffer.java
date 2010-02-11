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

public final class VertexBuffer extends Buffer{
	public VertexBuffer(UsageType usageType,AccessType accessType,VertexFormat vertexFormat,int size){
		mType=Type.VERTEX;
		mUsageType=usageType;
		mAccessType=accessType;
		mVertexFormat=vertexFormat;
		mSize=size;
		mBufferSize=mVertexFormat.getVertexSize()*mSize;

		mData=java.nio.ByteBuffer.allocateDirect(mBufferSize).order(java.nio.ByteOrder.nativeOrder());
	}

	public VertexBuffer(Renderer renderer,UsageType usageType,AccessType accessType,VertexFormat vertexFormat,int size){
		mType=Type.VERTEX;
		mUsageType=usageType;
		mAccessType=accessType;
		mVertexFormat=vertexFormat;
		mSize=size;
		mBufferSize=mVertexFormat.getVertexSize()*mSize;

		mBufferPeer=renderer.createBufferPeer(this);
		if(mBufferPeer==null || mBufferPeer.supportsRead()==false){
			mData=java.nio.ByteBuffer.allocateDirect(mBufferSize).order(java.nio.ByteOrder.nativeOrder());
		}
	}

	public VertexFormat getVertexFormat(){return mVertexFormat;}

	public int getSize(){return mSize;}

	public VertexBuffer clone(){
		VertexBuffer buffer=new VertexBuffer(mUsageType,mAccessType,mVertexFormat,mSize);

		java.nio.ByteBuffer srcData=lock(LockType.READ_ONLY);
		java.nio.ByteBuffer dstData=buffer.lock(LockType.WRITE_ONLY);

		dstData.put(srcData);
		srcData.rewind();
		dstData.rewind();

		buffer.unlock();
		unlock();

		return buffer;
	}

	public VertexBuffer cloneWithNewParameters(UsageType usageType,AccessType accessType,VertexFormat vertexFormat,int size){
		#if defined(TOADLET_DEBUG)
			{
				int i;
				for(i=0;i<vertexFormat.getNumVertexElements();++i){
					VertexElement dstElement=vertexFormat.getVertexElement(i);
					int dstElementSize=dstElement.getSize();
					if(mVertexFormat.hasVertexElementOfType(dstElement.type,dstElement.index)){
						VertexElement srcElement=mVertexFormat.getVertexElementOfType(dstElement.type,dstElement.index);
						int srcElementSize=srcElement.getSize();
						if(dstElementSize!=srcElementSize){
							Error.invalidParameters(Categories.TOADLET_PEEPER,
								"cloneWithNewParameters does not support changing size of elements");
							return null;
						}
					}
				}
			}
		#endif

		VertexBuffer buffer=new VertexBuffer(usageType,accessType,vertexFormat,size);

		java.nio.ByteBuffer srcData=lock(LockType.READ_ONLY);
		java.nio.ByteBuffer dstData=buffer.lock(LockType.WRITE_ONLY);
		byte[] srcDataArray=srcData.array();
		byte[] dstDataArray=dstData.array();
		
		int numVerts=mSize<size?mSize:size;
		int srcVertSize=mVertexFormat.getVertexSize();
		int dstVertSize=vertexFormat.getVertexSize();
		int i,j;
		for(i=0;i<vertexFormat.getNumVertexElements();++i){
			VertexElement dstElement=vertexFormat.getVertexElement(i);
			int elementSize=dstElement.getSize();
			if(mVertexFormat.hasVertexElementOfType(dstElement.type,dstElement.index)){
				VertexElement srcElement=mVertexFormat.getVertexElementOfType(dstElement.type,dstElement.index);
				for(j=0;j<numVerts;++j){
					System.arraycopy(srcDataArray,srcVertSize*j+srcElement.offset,dstDataArray,dstVertSize*j+dstElement.offset,elementSize);
				}
			}
		}

		buffer.unlock();
		unlock();

		return buffer;
	}
	
	protected VertexFormat mVertexFormat=null;
	protected int mSize=0;
}

