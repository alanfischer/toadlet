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

import java.io.*;
import java.nio.*;
import javax.microedition.khronos.opengles.*;
//#ifdef TOADLET_DEBUG
	import com.lightningtoads.toadlet.egg.*;
//#endif

public final class Texture{
	// Dimensions
	public final static byte DIMENSION_UNKNOWN=0;

	public final static byte DIMENSION_1D=1;
	public final static byte DIMENSION_2D=2;
	public final static byte DIMENSION_3D=3;
	public final static byte DIMENSION_CUBEMAP=4;

	public final static byte MAX_DIMENSIONS=5;

	// CubemapSides
	public final static byte CS_POSITIVE_X=0;
	public final static byte CS_NEGATIVE_X=1;
	public final static byte CS_POSITIVE_Y=2;
	public final static byte CS_NEGATIVE_Y=3;
	public final static byte CS_POSITIVE_Z=4;
	public final static byte CS_NEGATIVE_Z=5;

	public final static byte MAX_CUBEMAPSIDES=6;

	// Formats
	public final static int FORMAT_UNKNOWN=0;

	public final static int FORMAT_BIT_L=1<<0;
	public final static int FORMAT_BIT_A=1<<1;
	public final static int FORMAT_BIT_LA=1<<2;
	public final static int FORMAT_BIT_RGB=1<<3;
	public final static int FORMAT_BIT_RGBA=1<<4;
	public final static int FORMAT_BIT_DEPTH=1<<5;

	public final static int FORMAT_BIT_UINT_8=1<<10;
	public final static int FORMAT_BIT_FLOAT_32=1<<11;
	public final static int FORMAT_BIT_UINT_5_6_5=1<<12;
	public final static int FORMAT_BIT_UINT_5_5_5_1=1<<13;
	public final static int FORMAT_BIT_UINT_4_4_4_4=1<<14;

	public final static int FORMAT_L_8=FORMAT_BIT_L|FORMAT_BIT_UINT_8;
	public final static int FORMAT_A_8=FORMAT_BIT_A|FORMAT_BIT_UINT_8;
	public final static int FORMAT_LA_8=FORMAT_BIT_LA|FORMAT_BIT_UINT_8;
	public final static int FORMAT_RGB_8=FORMAT_BIT_RGB|FORMAT_BIT_UINT_8;
	public final static int FORMAT_RGBA_8=FORMAT_BIT_RGBA|FORMAT_BIT_UINT_8;
	public final static int FORMAT_DEPTH_8=FORMAT_BIT_DEPTH|FORMAT_BIT_UINT_8;
	public final static int FORMAT_L_32=FORMAT_BIT_L|FORMAT_BIT_FLOAT_32;
	public final static int FORMAT_A_32=FORMAT_BIT_A|FORMAT_BIT_FLOAT_32;
	public final static int FORMAT_LA_32=FORMAT_BIT_LA|FORMAT_BIT_FLOAT_32;
	public final static int FORMAT_RGB_32=FORMAT_BIT_RGB|FORMAT_BIT_FLOAT_32;
	public final static int FORMAT_RGBA_32=FORMAT_BIT_RGBA|FORMAT_BIT_FLOAT_32;
	public final static int FORMAT_DEPTH_32=FORMAT_BIT_DEPTH|FORMAT_BIT_FLOAT_32;
	public final static int FORMAT_RGB_5_6_5=FORMAT_BIT_RGB|FORMAT_BIT_UINT_5_6_5;
	public final static int FORMAT_RGBA_5_5_5_1=FORMAT_BIT_RGBA|FORMAT_BIT_UINT_5_5_5_1;
	public final static int FORMAT_RGBA_4_4_4_4=FORMAT_BIT_RGBA|FORMAT_BIT_UINT_4_4_4_4;

	// AddressModes
	public final static byte AM_REPEAT=0;
	public final static byte AM_CLAMP_TO_EDGE=1;
	public final static byte AM_CLAMP_TO_BORDER=2;
	public final static byte AM_MIRRORED_REPEAT=3;

	public final static byte MAX_ADDRESSMODE=4;

	// Filters
	public final static byte FILTER_NONE=0;
	public final static byte FILTER_NEAREST=1;
	public final static byte FILTER_LINEAR=2;

	public final static byte MAX_FILTERS=3;

	public Texture(){
		mDimension=DIMENSION_UNKNOWN;
		mFormat=FORMAT_UNKNOWN;
		mWidth=0;
		mHeight=0;
		mDepth=0;

		mAutoGenerateMipMaps=false;

		mSAddressMode=AM_REPEAT;
		mTAddressMode=AM_REPEAT;
		mRAddressMode=AM_REPEAT;

		mMipFilter=FILTER_NONE;
		mMinFilter=FILTER_NEAREST;
		mMagFilter=FILTER_NEAREST;
	}

	public Texture(byte dimension,int format,int width,int height,int depth){
		mDimension=dimension;
		mFormat=format;
    	mWidth=width;
    	mHeight=height;
		mDepth=depth;

		mAutoGenerateMipMaps=false;

		mSAddressMode=AM_REPEAT;
		mTAddressMode=AM_REPEAT;
		mRAddressMode=AM_REPEAT;

		mMipFilter=FILTER_NONE;
		mMinFilter=FILTER_NEAREST;
		mMagFilter=FILTER_NEAREST;
	}
	
	public byte getDimension(){return mDimension;}
	public int getFormat(){return mFormat;}

	public int getWidth(){return mWidth;}
	public int getHeight(){return mHeight;}
	public int getDepth(){return mDepth;}

	public void setAutoGenerateMipMaps(boolean mipmaps){mAutoGenerateMipMaps=mipmaps;}
	public boolean getAutoGenerateMipMaps(){return mAutoGenerateMipMaps;}

	public void setSAddressMode(byte addressMode){mSAddressMode=addressMode;}
	public byte getSAddressMode(){return mSAddressMode;}

	public void setTAddressMode(byte addressMode){mTAddressMode=addressMode;}
	public byte getTAddressMode(){return mTAddressMode;}

	public void setRAddressMode(byte addressMode){mRAddressMode=addressMode;}
	public byte getRAddressMode(){return mRAddressMode;}

	public void setMinFilter(byte minFilter){mMinFilter=minFilter;}
	public byte getMinFilter(){return mMinFilter;}

	public void setMipFilter(byte mipFilter){mMipFilter=mipFilter;}
	public byte getMipFilter(){return mMipFilter;}

	public void setMagFilter(byte magFilter){mMagFilter=magFilter;}
	public byte getMagFilter(){return mMagFilter;}

	public void setName(String name){mName=name;}
	public String getName(){return mName;}

	public void setBuffer(java.nio.Buffer buffer){mBuffer=buffer;}
	public java.nio.Buffer getBuffer(){return mBuffer;}

	public void bind(GL10 gl){
		gl.glBindTexture(target,handle[0]);
	}

	public void load(GL10 gl){
		if(handle[0]==0){
			target=GL10.GL_TEXTURE_2D;
			int format=getGLFormat(mFormat);
			int type=getGLType(mFormat);

			gl.glGenTextures(1,handle,0);

		    gl.glBindTexture(target,handle[0]);

			gl.glTexParameterx(target,GL10.GL_TEXTURE_WRAP_S,getGLWrap(mSAddressMode));
			gl.glTexParameterx(target,GL10.GL_TEXTURE_WRAP_T,getGLWrap(mTAddressMode));
			//gl.glTexParameterx(target,GL10.GL_TEXTURE_WRAP_R,getGLWrap(mRAddressMode));

			gl.glTexParameterx(target,GL10.GL_TEXTURE_MIN_FILTER,getGLMinFilter(mMinFilter,mMipFilter));
			gl.glTexParameterx(target,GL10.GL_TEXTURE_MAG_FILTER,getGLMagFilter(mMagFilter));

			gl.glTexImage2D(target,0,format,mWidth,mHeight,0,format,type,mBuffer);
		}

		if(mAutoGenerateMipMaps){
			throw new RuntimeException("AutoGeneration of mip maps is not yet supported");
		}
	}

	public void unload(GL10 gl){
		if(handle[0]!=0){
			gl.glDeleteTextures(1,handle,0);
			handle[0]=0;
		}
	}

	public static int getGLFormat(int textureFormat){
		int format=0;
		
		if((textureFormat&FORMAT_BIT_L)>0){
			format=GL10.GL_LUMINANCE;
		}
		else if((textureFormat&FORMAT_BIT_A)>0){
			format=GL10.GL_ALPHA;
		}
		else if((textureFormat&FORMAT_BIT_LA)>0){
			format=GL10.GL_LUMINANCE_ALPHA;
		}
		else if((textureFormat&FORMAT_BIT_RGB)>0){
			format=GL10.GL_RGB;
		}
		else if((textureFormat&FORMAT_BIT_RGBA)>0){
			format=GL10.GL_RGBA;
		}

		if(format==0){
			throw new RuntimeException("GLTexturePeer::getGLFormat: Invalid format");
		}

		return format;
	}

	public static int getGLType(int textureFormat){
		int type=0;

		if((textureFormat&FORMAT_BIT_UINT_8)>0){
			type=GL10.GL_UNSIGNED_BYTE;
		}
		else if((textureFormat&FORMAT_BIT_FLOAT_32)>0){
			type=GL10.GL_FLOAT;
		}
		else if((textureFormat&FORMAT_BIT_UINT_5_6_5)>0){
			type=GL10.GL_UNSIGNED_SHORT_5_6_5;
		}
		else if((textureFormat&FORMAT_BIT_UINT_5_5_5_1)>0){
			type=GL10.GL_UNSIGNED_SHORT_5_5_5_1;
		}
		else if((textureFormat&FORMAT_BIT_UINT_4_4_4_4)>0){
			type=GL10.GL_UNSIGNED_SHORT_4_4_4_4;
		}

		if(type==0){
			throw new RuntimeException("GLTexturePeer::getGLType: Invalid type");
		}

		return type;
	}

	public static int getGLWrap(byte addressMode){
		int wrap=0;

		switch(addressMode){
			case(AM_REPEAT):
				wrap=GL10.GL_REPEAT;
			break;
			case(AM_CLAMP_TO_EDGE):
				wrap=GL10.GL_CLAMP_TO_EDGE;
			break;
			//case(AM_CLAMP_TO_BORDER):
			//	wrap=GL10.GL_CLAMP_TO_BORDER;
			//break;
			//case(AM_MIRRORED_REPEAT):
			//	wrap=GL10.GL_MIRRORED_REPEAT;
			//break;
		}

		if(wrap==0){
			throw new RuntimeException("GLTexturePeer::getGLWrap: Invalid address mode");
		}

		return wrap;
	}

	public static int getGLMinFilter(byte minFilter,byte mipFilter){
		int filter=0;

		switch(mipFilter){
			case(FILTER_NONE):
				switch(minFilter){
					case(FILTER_NEAREST):
						filter=GL10.GL_NEAREST;
					break;
					case(FILTER_LINEAR):
						filter=GL10.GL_LINEAR;
					break;
				}
			break;
			case(FILTER_NEAREST):
				switch(minFilter){
					case(FILTER_NEAREST):
						filter=GL10.GL_NEAREST_MIPMAP_NEAREST;
					break;
					case(FILTER_LINEAR):
						filter=GL10.GL_NEAREST_MIPMAP_LINEAR;
					break;
				}
			break;
			case(FILTER_LINEAR):
				switch(minFilter){
					case(FILTER_NEAREST):
						filter=GL10.GL_LINEAR_MIPMAP_NEAREST;
					break;
					case(FILTER_LINEAR):
						filter=GL10.GL_LINEAR_MIPMAP_LINEAR;
					break;
				}
			break;
		}

		if(filter==0){
			throw new RuntimeException("GLTexturePeer::getGLMinFilter: Invalid filter");
		}

		return filter;
	}

	public static int getGLMagFilter(byte magFilter){
		int filter=0;

		switch(magFilter){
			case(FILTER_NEAREST):
				filter=GL10.GL_NEAREST;
			break;
			case(FILTER_LINEAR):
				filter=GL10.GL_LINEAR;
			break;
		}

		if(filter==0){
			throw new RuntimeException("GLTexturePeer::getGLMagFilter: Invalid filter");
		}

		return filter;
	}

	byte mDimension;
	int mFormat;
	int mWidth;
	int mHeight;
	int mDepth;

	boolean mAutoGenerateMipMaps;

	byte mSAddressMode;
	byte mTAddressMode;
	byte mRAddressMode;

	byte mMinFilter;
	byte mMipFilter;
	byte mMagFilter;

	String mName;
	
	java.nio.Buffer mBuffer;

	int target;
	int[] handle=new int[1];
}
