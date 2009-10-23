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

import java.nio.*;
import javax.microedition.khronos.opengles.*;

public final class Buffer{
	// BufferTypes
	public final static byte BT_INDEX=0;
	public final static byte BT_VERTEX=1;

	// UsageTypes
	public final static byte UT_STATIC=0;	// Buffer data is never changed
	public final static byte UT_STREAM=1;	// Buffer data changes once per frame
	public final static byte UT_DYNAMIC=2;	// Buffer data changes frequently

	// AccessTypes
	public final static byte AT_READ_ONLY=0;	// Buffer data is only readable
	public final static byte AT_WRITE_ONLY=1;	// Buffer data is only writeable
	public final static byte AT_READ_WRITE=2;	// Buffer data is readable & writeable
	public final static byte AT_NO_ACCESS=3;	// Buffer data is inaccessable

	// LockTypes
	public final static byte LT_READ_ONLY=0;	// Only read operations are performed on the data
	public final static byte LT_WRITE_ONLY=1;	// Only write operations are performed on the data
	public final static byte LT_READ_WRITE=2;	// Read & write operations are performed on the data

	// IndexDataTypes
	public final static byte IDT_UINT_8=1;
	public final static byte IDT_UINT_16=2;
	//public final static byte IDT_UINT_32=4;

	public Buffer(byte usageType,byte accessType,byte indexDataType,int size){
		mBufferType=BT_INDEX;
		mUsageType=usageType;
		mAccessType=accessType;
		mSize=size;

		mIndexDataType=indexDataType;

		if(mIndexDataType==IDT_UINT_8){
			mBuffer=ByteBuffer.allocateDirect(size);
		}
		else if(mIndexDataType==IDT_UINT_16){
			ByteBuffer buffer=ByteBuffer.allocateDirect(size*2);
			buffer.order(ByteOrder.nativeOrder());
			mBuffer=buffer.asShortBuffer();
		}
		//else if(mIndexDataType==IDT_UINT_32){
		//	ByteBuffer buffer=ByteBuffer.allocateDirect(size*4);
		//	buffer.order(ByteOrder.nativeOrder());
		//	mBuffer=buffer.asIntBuffer();
		//}
		else{
			throw new RuntimeException("Unknown indexDataType");
		}

		//#ifdef USE_HARDWAREBUFFERS
			target=GL11.GL_ELEMENT_ARRAY_BUFFER;
		//#endif
		mElementSize=mIndexDataType;
	}

	public Buffer(byte usageType,byte accessType,VertexFormat vertexFormat,int size){
		mBufferType=BT_VERTEX;
		mUsageType=usageType;
		mAccessType=accessType;
		mSize=size;

		mVertexFormat=new VertexFormat(vertexFormat);
		
		int numVertexElements=mVertexFormat.getNumVertexElements();
		if(numVertexElements>0){
			byte bufferDataType=getBufferDataType(mVertexFormat.getVertexElement(0).format);
			int i;
			for(i=1;i<numVertexElements;++i){
				if(bufferDataType!=getBufferDataType(mVertexFormat.getVertexElement(i).format)){
					throw new RuntimeException("Cannot combine two vertex elements of different data types");
				}
			}

			int count=0;
			for(i=0;i<numVertexElements;++i){
				int format=mVertexFormat.getVertexElement(i).format;

				if((format&VertexElement.FORMAT_BIT_COUNT_1)>0){
					count+=1;
				}
				else if((format&VertexElement.FORMAT_BIT_COUNT_2)>0){
					count+=2;
				}
				else if((format&VertexElement.FORMAT_BIT_COUNT_3)>0){
					count+=3;
				}
				else if((format&VertexElement.FORMAT_BIT_COUNT_4)>0){
					count+=4;
				}
				else if((format&VertexElement.FORMAT_COLOR_RGBA)>0){
					if(bufferDataType==BDT_BYTE){
						count+=4;
					}
					else if(bufferDataType==BDT_SHORT){
						count+=2;
					}
					else{
						count+=1;
					}
				}
			}

			if(bufferDataType==BDT_BYTE){
				mBuffer=ByteBuffer.allocateDirect(mSize*count);
				for(i=0;i<numVertexElements;++i){
					VertexElement vertexElement=mVertexFormat.getVertexElement(i);
					vertexElement.offsetBuffer=((ByteBuffer)mBuffer.position(vertexElement.formatOffset)).slice();
				}
			}
			else if(bufferDataType==BDT_SHORT){
				ByteBuffer buffer=ByteBuffer.allocateDirect(mSize*count*2);
				buffer.order(ByteOrder.nativeOrder());
				mBuffer=buffer.asShortBuffer();
				for(i=0;i<numVertexElements;++i){
					VertexElement vertexElement=mVertexFormat.getVertexElement(i);
					vertexElement.offsetBuffer=((ShortBuffer)mBuffer.position(vertexElement.formatOffset)).slice();
				}
			}
			else if(bufferDataType==BDT_INT){
				ByteBuffer buffer=ByteBuffer.allocateDirect(mSize*count*4);
				buffer.order(ByteOrder.nativeOrder());
				mBuffer=buffer.asIntBuffer();
				for(i=0;i<numVertexElements;++i){
					VertexElement vertexElement=mVertexFormat.getVertexElement(i);
					vertexElement.offsetBuffer=((IntBuffer)mBuffer.position(vertexElement.formatOffset)).slice();
				}
			}
			else if(bufferDataType==BDT_FLOAT){
				ByteBuffer buffer=ByteBuffer.allocateDirect(mSize*count*4);
				buffer.order(ByteOrder.nativeOrder());
				mBuffer=buffer.asFloatBuffer();
				for(i=0;i<numVertexElements;++i){
					VertexElement vertexElement=mVertexFormat.getVertexElement(i);
					vertexElement.offsetBuffer=((FloatBuffer)mBuffer.position(vertexElement.formatOffset)).slice();
				}
			}
			mBuffer.rewind();
		}
		else{
			throw new RuntimeException("Invalid number of vertex elements");
		}

		//#ifdef USE_HARDWAREBUFFERS
			mTarget=GL11.GL_ARRAY_BUFFER;
		//#endif
		mElementSize=mVertexFormat.getVertexSize();
	}

	public java.nio.Buffer lock(byte lockType){
		return mBuffer;
	}

	public void unlock(){
		//#ifdef USE_HARDWAREBUFFERS
			updateData=true;
		//#endif
	}

	//#ifdef USE_HARDWAREBUFFERS
		public void unload(){
			if(gl11!=null && handle[0]!=0){
				gl11.glDeleteBuffers(handle.length,handle,0);
				handle[0]=0;
			}
		}
	//#endif

	public void bind(GL10 gl){
		if(gl10==null){
			gl10=gl;
			//#ifdef USE_HARDWAREBUFFERS
				try{
					gl11=(GL11)gl;
				}
				catch(Exception ex){
					gl11=null;
				}
			//#endif
		}

		//#ifdef USE_HARDWAREBUFFERS
			if(gl11!=null){
				if(updateData){
					updateData=false;
					if(handle[0]!=0){
						int usage=0;
						switch(mUsageType){
							case(UT_STATIC):
								usage=gl11.GL_STATIC_DRAW;
							break;
							case(UT_STREAM):
							case(UT_DYNAMIC):
								usage=gl11.GL_DYNAMIC_DRAW;
							break;
						}			

						gl11.glGenBuffers(1,handle);
						gl11.glBindBuffer(target,handle[0]);
						gl11.glBufferData(target,mElementSize*mSize,mBuffer,usage);
					}
					else{
						gl11.glBindBuffer(target,handle[0]);
						gl11.glBufferSubData(target,0,mElementSize*mSize,mBuffer);
					}
				}
				else{
					gl11.glBindBuffer(target,handle[0]);
				}
			}
		//#endif

		if(mBufferType==BT_VERTEX){
			int numVertexElements=mVertexFormat.getNumVertexElements();
			int i;

			for(i=0;i<numVertexElements;++i){
				VertexElement vertexElement=mVertexFormat.getVertexElement(i);
				byte type=vertexElement.type;
				int elementCount=getGLElementCount(vertexElement.format);
				int dataType=getGLDataType(vertexElement.format);

				switch(type){
					case(VertexElement.TYPE_POSITION):
						gl.glEnableClientState(GL10.GL_VERTEX_ARRAY);
						gl.glVertexPointer(elementCount,dataType,mElementSize,vertexElement.offsetBuffer);
					break;
					case(VertexElement.TYPE_NORMAL):
						gl.glEnableClientState(GL10.GL_NORMAL_ARRAY);
						gl.glNormalPointer(dataType,mElementSize,vertexElement.offsetBuffer);
					break;
					case(VertexElement.TYPE_COLOR):
						gl.glEnableClientState(GL10.GL_COLOR_ARRAY);
						gl.glColorPointer(4,GL10.GL_UNSIGNED_BYTE,mElementSize,vertexElement.offsetBuffer);
					break;
					case(VertexElement.TYPE_TEX_COORD):
						gl.glEnableClientState(GL10.GL_TEXTURE_COORD_ARRAY);
						gl.glTexCoordPointer(elementCount,dataType,mElementSize,vertexElement.offsetBuffer);
					break;
				}
			}
		}
	}

	public void unbind(GL10 gl){
		if(mBufferType==BT_VERTEX){
			int numVertexElements=mVertexFormat.getNumVertexElements();
			int i;
			for(i=0;i<numVertexElements;++i){
				VertexElement vertexElement=mVertexFormat.getVertexElement(i);
				byte type=vertexElement.type;

				switch(type){
					case(VertexElement.TYPE_POSITION):
						gl.glDisableClientState(GL10.GL_VERTEX_ARRAY);
					break;
					case(VertexElement.TYPE_NORMAL):
						gl.glDisableClientState(GL10.GL_NORMAL_ARRAY);
					break;
					case(VertexElement.TYPE_COLOR):
						gl.glDisableClientState(GL10.GL_COLOR_ARRAY);
					break;
					case(VertexElement.TYPE_TEX_COORD):
						gl.glDisableClientState(GL10.GL_TEXTURE_COORD_ARRAY);
					break;
				}
			}
		}
	}

	public int getSize(){return mSize;}

	public byte getIndexDataType(){return mIndexDataType;}

	public VertexFormat getVertexFormat(){return mVertexFormat;}

	// BufferDataTypes
	protected final static byte BDT_BYTE=1;
	protected final static byte BDT_SHORT=2;
	protected final static byte BDT_INT=3;
	protected final static byte BDT_FLOAT=4;
	
	protected byte getBufferDataType(int format){
		if((format&VertexElement.FORMAT_BIT_UINT_8)>0){
			return BDT_BYTE;
		}
		else if((format&VertexElement.FORMAT_BIT_INT_16)>0){
			return BDT_SHORT;
		}
		else if((format&(VertexElement.FORMAT_BIT_FIXED_32|VertexElement.FORMAT_COLOR_RGBA))>0){
			return BDT_INT;
		}
		else if((format&VertexElement.FORMAT_BIT_FLOAT_32)>0){
			return BDT_FLOAT;
		}
		return 0;
	}

	protected int getGLElementCount(int format){
		if((format&VertexElement.FORMAT_BIT_COUNT_1)>0){
			return 1;
		}
		else if((format&VertexElement.FORMAT_BIT_COUNT_2)>0){
			return 2;
		}
		else if((format&VertexElement.FORMAT_BIT_COUNT_3)>0){
			return 3;
		}
		else if((format&(VertexElement.FORMAT_BIT_COUNT_4|VertexElement.FORMAT_COLOR_RGBA))>0){
			return 4;
		}
		return 0;
	}

	protected int getGLDataType(int format){
		if((format&VertexElement.FORMAT_BIT_UINT_8)>0){
			return GL10.GL_UNSIGNED_BYTE;
		}
		//else if((format&VertexElement.FORMAT_BIT_INT_8)>0){
		//	return GL10.GL_BYTE;
		//}
		else if((format&VertexElement.FORMAT_BIT_INT_16)>0){
			return GL10.GL_SHORT;
		}
		//else if((format&VertexElement.FORMAT_BIT_INT_32)>0){
		//	return GL10.GL_INT;
		//}
		else if((format&VertexElement.FORMAT_BIT_FIXED_32)>0){
			return GL10.GL_FIXED;
		}
		else if((format&VertexElement.FORMAT_BIT_FLOAT_32)>0){
			return GL10.GL_FLOAT;
		}
		//else if((format&VertexElement.FORMAT_BIT_DOUBLE_64)>0){
		//	return GL10.GL_DOUBLE;
		//}
		return 0;
	}

	byte mBufferType;
	byte mUsageType;
	byte mAccessType;
	int mSize;

	// IndexBuffer data
	byte mIndexDataType;

	// VertexBuffer data
	VertexFormat mVertexFormat;

	int mElementSize;

	java.nio.Buffer mBuffer;

	//#ifdef USE_HARDWAREBUFFERS
		int target;
		int[] handle=new int[1];
	//#endif

	GL10 gl10;
	//#ifdef USE_HARDWAREBUFFERS
		boolean updateData;
		GL11 gl11;
	//#endif
}
