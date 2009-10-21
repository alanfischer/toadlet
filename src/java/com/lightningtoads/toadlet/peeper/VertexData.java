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

public final class VertexData{
	public VertexData(){
		mVertexBuffers=new VertexBuffer[1];
	}

	public VertexData(int numVertexBuffers){
		mVertexBuffers=new VertexBuffer[numVertexBuffers];
	}

	public VertexData(VertexBuffer vertexBuffer){
		mVertexBuffers=new VertexBuffer[1];
		addVertexBuffer(vertexBuffer);
	}

	public VertexFormat getVertexFormat(){return mVertexFormat;}

	public void addVertexBuffer(VertexBuffer vertexBuffer){
		if(mNumVertexBuffers==0){
			mVertexFormat=vertexBuffer.getVertexFormat();
		}
		else{
			if(mNumVertexBuffers==1){
				mVertexFormat=new VertexFormat();
			}

			VertexFormat vertexFormat=vertexBuffer.getVertexFormat();

			int i;
			for(i=0;i<vertexFormat.getNumVertexElements();++i){
				mVertexFormat.addVertexElement(vertexFormat.getVertexElement(i));
			}
		}

		if(mVertexBuffers.length<mNumVertexBuffers+1){
			VertexBuffer[] vertexBuffers=new VertexBuffer[mVertexBuffers.length+1];
			System.arraycopy(mVertexBuffers,0,vertexBuffers,0,mVertexBuffers.length);
			mVertexBuffers=vertexBuffers;
		}
		mVertexBuffers[mNumVertexBuffers++]=vertexBuffer;
	}

	public int getNumVertexBuffers(){return mNumVertexBuffers;}
	public VertexBuffer getVertexBuffer(int index){return mVertexBuffers[index];}

	protected VertexFormat mVertexFormat=null;
	protected VertexBuffer[] mVertexBuffers=null;
	protected int mNumVertexBuffers=0;
}
