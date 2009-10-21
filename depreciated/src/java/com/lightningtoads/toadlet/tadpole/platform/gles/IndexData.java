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

import javax.microedition.khronos.opengles.*;

public final class IndexData{
	// Primitives
	public final static byte PRIMITIVE_POINTS=0;
	public final static byte PRIMITIVE_LINES=1;
	public final static byte PRIMITIVE_LINESTRIP=2;
	public final static byte PRIMITIVE_TRIS=3;
	public final static byte PRIMITIVE_TRISTRIP=4;
	public final static byte PRIMITIVE_TRIFAN=5;

	public IndexData(byte primitive,Buffer indexBuffer,int start,int count){
		mPrimitive=primitive;
		mIndexBuffer=indexBuffer;
		mStart=start;
		mCount=count;

		if(mIndexBuffer!=null){
			switch(mIndexBuffer.getIndexDataType()){
				case(Buffer.IDT_UINT_8):
					mDrawBuffer=((java.nio.ByteBuffer)mIndexBuffer.mBuffer.position(mStart)).slice();
				break;
				case(Buffer.IDT_UINT_16):
					mDrawBuffer=((java.nio.ShortBuffer)mIndexBuffer.mBuffer.position(mStart)).slice();
				break;
				default:
					mDrawBuffer=mIndexBuffer.mBuffer;
			}
		}
	}

	public byte getPrimitive(){return mPrimitive;}
	public Buffer getIndexBuffer(){return mIndexBuffer;}
	public int getStart(){return mStart;}
	public int getCount(){return mCount;}

	public void bind(GL10 gl){
		if(mIndexBuffer!=null){
			mIndexBuffer.bind(gl);
		}
	}

	public void draw(GL10 gl){
		int type=0;
		switch(mPrimitive){
			case(PRIMITIVE_POINTS):
				type=gl.GL_POINTS;
			break;
			case(PRIMITIVE_LINES):
				type=gl.GL_LINES;
			break;
			case(PRIMITIVE_LINESTRIP):
				type=gl.GL_LINE_STRIP;
			break;
			case(PRIMITIVE_TRIS):
				type=gl.GL_TRIANGLES;
			break;
			case(PRIMITIVE_TRISTRIP):
				type=gl.GL_TRIANGLE_STRIP;
			break;
			case(PRIMITIVE_TRIFAN):
				type=gl.GL_TRIANGLE_FAN;
			break;
		}

		if(mIndexBuffer!=null){
			int dataType=0;
			switch(mIndexBuffer.mIndexDataType){
				case(Buffer.IDT_UINT_8):
					dataType=gl.GL_UNSIGNED_BYTE;
				break;
				case(Buffer.IDT_UINT_16):
					dataType=gl.GL_UNSIGNED_SHORT;
				break;
			}

			gl.glDrawElements(type,mCount,dataType,mDrawBuffer);
		}
		else{
			gl.glDrawArrays(type,mStart,mCount);
		}
	}

	byte mPrimitive;
	Buffer mIndexBuffer;
	java.nio.Buffer mDrawBuffer;
	int mStart;
	int mCount;
}

