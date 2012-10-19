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

public final class VertexElement{
	// Type
	public final static byte TYPE_UNKNOWN=0;

	public final static byte TYPE_POSITION=1;
	public final static byte TYPE_BLEND_WEIGHTS=2;
	public final static byte TYPE_BLEND_INDICES=3;
	public final static byte TYPE_NORMAL=4;
	public final static byte TYPE_COLOR=5;
	public final static byte TYPE_TEX_COORD=6;
	public final static byte TYPE_BINORMAL=7;
	public final static byte TYPE_TANGENT=8;

	public final static byte MAX_TYPE=9;

	// Format
	public final static int FORMAT_UNKNOWN=0;
	
	public final static int FORMAT_BIT_UINT_8=1<<0;
	//public final static int FORMAT_BIT_INT_8=1<<1;
	public final static int FORMAT_BIT_INT_16=1<<2;
	//public final static int FORMAT_BIT_INT_32=1<<3;
	public final static int FORMAT_BIT_FIXED_32=1<<4;
	public final static int FORMAT_BIT_FLOAT_32=1<<5;
	//public final static int FORMAT_BIT_DOUBLE_64=1<<6;

	public final static int FORMAT_BIT_COUNT_1=1<<10;
	public final static int FORMAT_BIT_COUNT_2=1<<11;
	public final static int FORMAT_BIT_COUNT_3=1<<12;
	public final static int FORMAT_BIT_COUNT_4=1<<13;

	public final static int FORMAT_COLOR_RGBA=1<<20;

	public VertexElement(){
		type=0;
		format=0;
		index=0;
		formatOffset=0;
		offsetBuffer=null;
	}

	public VertexElement(byte type,int format){
		this.type=type;
		this.format=format;
		index=0;
		formatOffset=0;
		offsetBuffer=null;
	}

	public VertexElement(byte type,int format,short index){
		this.type=type;
		this.format=format;
		this.index=index;
		formatOffset=0;
		offsetBuffer=null;
	}

	public VertexElement(VertexElement vertexElement){
		type=vertexElement.type;
		format=vertexElement.format;
		index=vertexElement.index;
		formatOffset=vertexElement.formatOffset;
		offsetBuffer=vertexElement.offsetBuffer;
	}

	public void set(VertexElement vertexElement){
		type=vertexElement.type;
		format=vertexElement.format;
		index=vertexElement.index;
		formatOffset=vertexElement.formatOffset;
		offsetBuffer=vertexElement.offsetBuffer;
	}

	public int getSize(){
		int size=0;

		if((format&FORMAT_BIT_UINT_8/*|FORMAT_BIT_INT_8*/)>0){
			size=1;
		}
		else if((format&FORMAT_BIT_INT_16)>0){
			size=2;
		}
		else if((format&(/*FORMAT_BIT_INT_32|*/FORMAT_BIT_FIXED_32|FORMAT_BIT_FLOAT_32))>0){
			size=4;
		}
		//else if((format&FORMAT_BIT_DOUBLE_64)>0){
		//	size=8;
		//}
		else if(format==FORMAT_COLOR_RGBA){
			size=4;
		}

		if((format&FORMAT_BIT_COUNT_2)>0){
			size*=2;
		}
		else if((format&FORMAT_BIT_COUNT_3)>0){
			size*=3;
		}
		else if((format&FORMAT_BIT_COUNT_4)>0){
			size*=4;
		}

		return size;
	}

	public byte type;
	public int format;
	public short index;
	public short formatOffset; // Offset is in formats' data type units, not bytes
	public java.nio.Buffer offsetBuffer; // The nio.Buffer from the Buffer's main buffer that has been 'slice()'d to the formatOffset
}

