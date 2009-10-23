/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright (C) 2006, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer
 *
 * All source code for the Toadlet Engine, including
 * this file, is the sole property of Lightning Toads
 * Productions, LLC. It has been developed on our own
 * time, with our own tools, on our own hardware. None
 * of this code may be observed, used or changed
 * without our explicit permission.
 *
 *  - Lightning Toads Productions, LLC
 *
 ********** Copyright header - do not remove **********/

package com.lightningtoads.toadlet.tadpole;

public final class VertexFormat{
	public VertexFormat(){
		mVertexElements=new VertexElement[0];
	}

	public VertexFormat(int numVertexElements){
		mVertexElements=new VertexElement[numVertexElements];
	}

	public VertexFormat(VertexFormat vertexFormat){
		mVertexElements=new VertexElement[vertexFormat.mNumVertexElements];

		int i;
		for(i=0;i<vertexFormat.mNumVertexElements;++i){
			addVertexElement(vertexFormat.mVertexElements[i]);
		}
	}

	public VertexElement addVertexElement(VertexElement element){
		short index=(short)mNumVertexElements;
		if(mVertexElements.length<mNumVertexElements+1){
			VertexElement[] vertexElements=new VertexElement[mVertexElements.length+1];
			System.arraycopy(mVertexElements,0,vertexElements,0,mVertexElements.length);
			mVertexElements=vertexElements;
		}
		
		mVertexElements[index]=new VertexElement(element);
		mNumVertexElements++;

		mVertexElements[index].formatOffset=mVertexFormatSize;

		mVertexSize+=element.getSize();
		if((element.format&(VertexElement.FORMAT_BIT_COUNT_1|VertexElement.FORMAT_COLOR_RGBA))>0){
			mVertexFormatSize+=1;
		}
		else if((element.format&VertexElement.FORMAT_BIT_COUNT_2)>0){
			mVertexFormatSize+=2;
		}
		else if((element.format&VertexElement.FORMAT_BIT_COUNT_3)>0){
			mVertexFormatSize+=3;
		}
		else if((element.format&VertexElement.FORMAT_BIT_COUNT_4)>0){
			mVertexFormatSize+=4;
		}

		if(element.type==VertexElement.TYPE_COLOR){
			if(element.index>=mColorElementsByIndex.length){
				byte[] colorElementsByIndex=new byte[element.index+1];
				System.arraycopy(mColorElementsByIndex,0,colorElementsByIndex,0,mColorElementsByIndex.length);
				mColorElementsByIndex=colorElementsByIndex;
			}
			mColorElementsByIndex[element.index]=(byte)index;
		}
		else if(element.type==VertexElement.TYPE_TEX_COORD){
			if(element.index>=mTexCoordElementsByIndex.length){
				byte[] texCoordElementsByIndex=new byte[element.index+1];
				System.arraycopy(mTexCoordElementsByIndex,0,texCoordElementsByIndex,0,mTexCoordElementsByIndex.length);
				mTexCoordElementsByIndex=texCoordElementsByIndex;
			}
			mTexCoordElementsByIndex[element.index]=(byte)index;
		}
		else{
			if(element.type>=mVertexElementsByType.length){
				byte[] vertexElementsByType=new byte[element.type+1];
				System.arraycopy(mVertexElementsByType,0,vertexElementsByType,0,mVertexElementsByType.length);
				mVertexElementsByType=vertexElementsByType;
			}
			mVertexElementsByType[element.type]=(byte)index;
		}

		return mVertexElements[index];
	}

	public int getNumVertexElements(){return mNumVertexElements;}

	public VertexElement getVertexElement(int index){return mVertexElements[index];}

	public short getVertexSize(){return mVertexSize;}

	public VertexElement getVertexElementFromType(byte type){
		return getVertexElementFromType(type,0);
	}

	public VertexElement getVertexElementFromType(byte type,int index){
		if(type==VertexElement.TYPE_COLOR){
			return mVertexElements[mColorElementsByIndex[index]];
		}
		else if(type==VertexElement.TYPE_TEX_COORD){
			return mVertexElements[mTexCoordElementsByIndex[index]];
		}
		else{
			return mVertexElements[mVertexElementsByType[type]];
		}
	}

	public short getMaxColorIndex(){return (short)(mColorElementsByIndex.length-1);}

	public short getMaxTexCoordIndex(){return (short)(mTexCoordElementsByIndex.length-1);}

	public VertexElement getColorElementByIndex(int colorIndex){return mVertexElements[mColorElementsByIndex[colorIndex]];}

	public VertexElement getTexCoordElementByIndex(int texCoordIndex){return mVertexElements[mTexCoordElementsByIndex[texCoordIndex]];}

	public short getVertexFormatSize(){return mVertexFormatSize;} // The size of the vertex with respect to it's format

	VertexElement[] mVertexElements=null;
	int mNumVertexElements=0;
	short mVertexSize=0;
	short mVertexFormatSize=0; // Not a byte offset, instead an offset of a size of the data type used in this format
	byte[] mVertexElementsByType=new byte[0];

	short mMaxColorIndex=0;
	byte[] mColorElementsByIndex=new byte[0];

	short mMaxTexCoordIndex=0;
	byte[] mTexCoordElementsByIndex=new byte[0];
};