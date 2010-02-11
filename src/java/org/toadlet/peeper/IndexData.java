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

public final class IndexData{
	public enum Primitive{
		POINTS,
		LINES,
		LINESTRIP,
		TRIS,
		TRISTRIP,
		TRIFAN,
	}

	public IndexData(Primitive primitive,IndexBuffer indexBuffer,int start,int count){
		mPrimitive=primitive;
		mIndexBuffer=indexBuffer;
		mStart=start;
		mCount=count;
	}

	public IndexData(Primitive primitive,IndexBuffer indexBuffer){
		mPrimitive=primitive;
		mIndexBuffer=indexBuffer;
		mStart=0;
		mCount=indexBuffer.getSize();
	}
	
	public void setPrimitive(Primitive primitive){mPrimitive=primitive;}
	public Primitive getPrimitive(){return mPrimitive;}

	public void setIndexBuffer(IndexBuffer indexBuffer){mIndexBuffer=indexBuffer;}
	public IndexBuffer getIndexBuffer(){return mIndexBuffer;}

	public void setStart(int start){mStart=start;}
	public int getStart(){return mStart;}

	public void setCount(int count){mCount=count;}
	public int getCount(){return mCount;}

	protected Primitive mPrimitive=Primitive.POINTS;
	protected IndexBuffer mIndexBuffer=null;
	protected int mStart=0;
	protected int mCount=0;
}
