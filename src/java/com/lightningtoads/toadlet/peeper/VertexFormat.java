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

public final class VertexFormat{
	public VertexFormat(){
		mVertexElements=new VertexElement[1];
	}

	public VertexFormat(int numVertexElements){
		mVertexElements=new VertexElement[numVertexElements];
	}

	public VertexFormat(VertexFormat vertexFormat){
		mVertexElements=new VertexElement[1];
		int i;
		for(i=0;i<vertexFormat.mNumVertexElements;++i){
			addVertexElement(vertexFormat.mVertexElements[i]);
		}
	}

	public VertexElement addVertexElement(VertexElement element){
		short index=mNumVertexElements;
		if(mVertexElements.length<mNumVertexElements+1){
			VertexElement[] vertexElements=new VertexElement[mNumVertexElements+1];
			System.arraycopy(mVertexElements,0,vertexElements,0,mVertexElements.length);
			mVertexElements=vertexElements;
		}

		mNumVertexElements++;
		mVertexElements[index]=new VertexElement(element);
		mVertexElements[index].offset=mVertexSize;

		mVertexSize+=element.getSize();

		if(element.type==VertexElement.Type.COLOR){
			if(element.index>=mColorElementsByIndex.length){
				short[] colorElementsByIndex=new short[element.index+1];
				System.arraycopy(mColorElementsByIndex,0,colorElementsByIndex,0,mColorElementsByIndex.length);
				mColorElementsByIndex=colorElementsByIndex;
			}
			mColorElementsByIndex[element.index]=index;
		}
		else if(element.type==VertexElement.Type.TEX_COORD){
			if(element.index>=mTexCoordElementsByIndex.length){
				short[] texCoordElementsByIndex=new short[element.index+1];
				System.arraycopy(mTexCoordElementsByIndex,0,texCoordElementsByIndex,0,mTexCoordElementsByIndex.length);
				mTexCoordElementsByIndex=texCoordElementsByIndex;
			}
			mTexCoordElementsByIndex[element.index]=index;
		}
		else{
			if(element.type.ordinal()>=mVertexElementsByType.length){
				short[] vertexElementsByType=new short[element.type.ordinal()+1];
				System.arraycopy(mVertexElementsByType,0,vertexElementsByType,0,mVertexElementsByType.length);
				mVertexElementsByType=vertexElementsByType;
			}
			mVertexElementsByType[element.type.ordinal()]=index;
		}

		return mVertexElements[index];
	}

	public int getNumVertexElements(){return mNumVertexElements;}

	public VertexElement getVertexElement(int index){return mVertexElements[index];}

	public short getVertexSize(){return mVertexSize;}

	public boolean hasVertexElementOfType(VertexElement.Type type){return hasVertexElementOfType(type,0);}
	public boolean hasVertexElementOfType(VertexElement.Type type,int index){
		if(type==VertexElement.Type.COLOR){
			return mColorElementsByIndex.length>index && mColorElementsByIndex[index]!=-1;
		}
		else if(type==VertexElement.Type.TEX_COORD){
			return mTexCoordElementsByIndex.length>index && mTexCoordElementsByIndex[index]!=-1;
		}
		else{
			return mVertexElementsByType.length>type.ordinal() && mVertexElementsByType[type.ordinal()]!=-1;
		}
	}

	public VertexElement getVertexElementOfType(VertexElement.Type type){return getVertexElementOfType(type,0);}
	public VertexElement getVertexElementOfType(VertexElement.Type type,int index){
		if(type==VertexElement.Type.COLOR){
			return mVertexElements[mColorElementsByIndex[index]];
		}
		else if(type==VertexElement.Type.TEX_COORD){
			return mVertexElements[mTexCoordElementsByIndex[index]];
		}
		else{
			return mVertexElements[mVertexElementsByType[type.ordinal()]];
		}
	}

	public int getVertexElementIndexOfType(VertexElement.Type type){return getVertexElementIndexOfType(type,0);}
	public int getVertexElementIndexOfType(VertexElement.Type type,int index){
		if(type==VertexElement.Type.COLOR){
			if(mColorElementsByIndex.length>index){
				return mColorElementsByIndex[index];
			}
			else{
				return -1;
			}
		}
		else if(type==VertexElement.Type.TEX_COORD){
			if(mTexCoordElementsByIndex.length>index){
				return mTexCoordElementsByIndex[index];
			}
			else{
				return -1;
			}
		}
		else{
			if(mVertexElementsByType.length>type.ordinal()){
				return mVertexElementsByType[type.ordinal()];
			}
			else{
				return -1;
			}
		}
	}

	public boolean equals(Object object){
		if(this==object){
			return true;
		}
		if((object==null) || (object.getClass()!=getClass())){
			return false;
		}
		VertexFormat format=(VertexFormat)object;
		if(format.mNumVertexElements!=mNumVertexElements){
			return false;
		}
		int i;
		for(i=0;i<mNumVertexElements;++i){
			if(format.mVertexElements[i].equals(mVertexElements[i])==false){
				return false;
			}
		}
		return true;
	}

	public short getMaxColorIndex(){return (short)(mColorElementsByIndex.length-1);}

	public short getMaxTexCoordIndex(){return (short)(mTexCoordElementsByIndex.length-1);}

	public VertexElement getColorElementByIndex(int colorIndex){return mVertexElements[mColorElementsByIndex[colorIndex]];}

	public VertexElement getTexCoordElementByIndex(int texCoordIndex){return mVertexElements[mTexCoordElementsByIndex[texCoordIndex]];}

	protected VertexElement[] mVertexElements;
	protected short mNumVertexElements;

	protected short mVertexSize;
	protected short[] mVertexElementsByType=new short[0];

	protected short mMaxColorIndex;
	protected short[] mColorElementsByIndex=new short[0];

	protected short mMaxTexCoordIndex;
	protected short[] mTexCoordElementsByIndex=new short[0];
}
