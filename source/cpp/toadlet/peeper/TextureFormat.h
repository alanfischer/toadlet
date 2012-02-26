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

#ifndef TOADLET_PEEPER_TEXTUREFORMAT_H
#define TOADLET_PEEPER_TEXTUREFORMAT_H

#include <toadlet/peeper/Types.h>

namespace toadlet{
namespace peeper{

class TOADLET_API TextureFormat{
public:
	TOADLET_SPTR(TextureFormat);

	enum Dimension{
		Dimension_UNKNOWN=0,
		Dimension_D1,
		Dimension_D2,
		Dimension_D3,
		Dimension_CUBE,
	};

	enum CubeSide{
		CubeSide_POSITIVE_X=0,
		CubeSide_NEGATIVE_X=1,
		CubeSide_POSITIVE_Y=2,
		CubeSide_NEGATIVE_Y=3,
		CubeSide_POSITIVE_Z=4,
		CubeSide_NEGATIVE_Z=5,
		CubeSide_MAX=6,
	};

	enum Format{
		Format_UNKNOWN=				0,

		// Format semantics
		Format_MASK_SEMANTICS=		0xFF,
		Format_SEMANTIC_L=			1,
		Format_SEMANTIC_A=			2,
		Format_SEMANTIC_LA=			3,
		Format_SEMANTIC_R=			4,
		Format_SEMANTIC_RG=			5,
		Format_SEMANTIC_RGB=		6,
		Format_SEMANTIC_BGR=		7,
		Format_SEMANTIC_RGBA=		8,
		Format_SEMANTIC_BGRA=		9,
		Format_SEMANTIC_ARGB=		10,
		Format_SEMANTIC_ABGR=		11,
		Format_SEMANTIC_YUV=		12,
		Format_SEMANTIC_DEPTH=		13,

		// Format types
		Format_SHIFT_TYPES=			8,
		Format_MASK_TYPES=			0xFF00,
		Format_TYPE_UINT_8=			1<<Format_SHIFT_TYPES,
		Format_TYPE_UINT_16=		2<<Format_SHIFT_TYPES,
		Format_TYPE_UINT_24=		3<<Format_SHIFT_TYPES,
		Format_TYPE_UINT_32=		4<<Format_SHIFT_TYPES,
		Format_TYPE_FLOAT_16=		5<<Format_SHIFT_TYPES,
		Format_TYPE_FLOAT_32=		6<<Format_SHIFT_TYPES,
		Format_TYPE_UINT_5_6_5=		7<<Format_SHIFT_TYPES,
		Format_TYPE_UINT_5_5_5_1=	8<<Format_SHIFT_TYPES,
		Format_TYPE_UINT_1_5_5_5=	9<<Format_SHIFT_TYPES,
		Format_TYPE_UINT_4_4_4_4=	10<<Format_SHIFT_TYPES,
		Format_TYPE_DXT1=			11<<Format_SHIFT_TYPES,
		Format_TYPE_DXT2=			12<<Format_SHIFT_TYPES,
		Format_TYPE_DXT3=			13<<Format_SHIFT_TYPES,
		Format_TYPE_DXT4=			14<<Format_SHIFT_TYPES,
		Format_TYPE_DXT5=			15<<Format_SHIFT_TYPES,
		Format_TYPE_YUY2=			16<<Format_SHIFT_TYPES,
		Format_TYPE_NV12=			17<<Format_SHIFT_TYPES,

		Format_L_8=					Format_SEMANTIC_L|Format_TYPE_UINT_8,
		Format_R_8=					Format_SEMANTIC_R|Format_TYPE_UINT_8,
		Format_A_8=					Format_SEMANTIC_A|Format_TYPE_UINT_8,
		Format_LA_8=				Format_SEMANTIC_LA|Format_TYPE_UINT_8,
		Format_RG_8=				Format_SEMANTIC_RG|Format_TYPE_UINT_8,
		Format_RGB_8=				Format_SEMANTIC_RGB|Format_TYPE_UINT_8,
		Format_BGR_8=				Format_SEMANTIC_BGR|Format_TYPE_UINT_8,
		Format_RGBA_8=				Format_SEMANTIC_RGBA|Format_TYPE_UINT_8,
		Format_BGRA_8=				Format_SEMANTIC_BGRA|Format_TYPE_UINT_8,
		Format_ARGB_8=				Format_SEMANTIC_ARGB|Format_TYPE_UINT_8,
		Format_ABGR_8=				Format_SEMANTIC_ABGR|Format_TYPE_UINT_8,
		Format_DEPTH_8=				Format_SEMANTIC_DEPTH|Format_TYPE_UINT_8,
		Format_DEPTH_16=			Format_SEMANTIC_DEPTH|Format_TYPE_UINT_16,
		Format_DEPTH_24=			Format_SEMANTIC_DEPTH|Format_TYPE_UINT_24,
		Format_DEPTH_32=			Format_SEMANTIC_DEPTH|Format_TYPE_UINT_32,
		Format_L_F32=				Format_SEMANTIC_L|Format_TYPE_FLOAT_32,
		Format_A_F32=				Format_SEMANTIC_A|Format_TYPE_FLOAT_32,
		Format_LA_F32=				Format_SEMANTIC_LA|Format_TYPE_FLOAT_32,
		Format_RGB_F32=				Format_SEMANTIC_RGB|Format_TYPE_FLOAT_32,
		Format_BGR_F32=				Format_SEMANTIC_BGR|Format_TYPE_FLOAT_32,
		Format_RGBA_F32=			Format_SEMANTIC_RGBA|Format_TYPE_FLOAT_32,
		Format_BGRA_F32=			Format_SEMANTIC_BGRA|Format_TYPE_FLOAT_32,
		Format_ARGB_F32=			Format_SEMANTIC_ARGB|Format_TYPE_FLOAT_32,
		Format_ABGR_F32=			Format_SEMANTIC_ABGR|Format_TYPE_FLOAT_32,
		Format_DEPTH_F32=			Format_SEMANTIC_DEPTH|Format_TYPE_FLOAT_32,
		Format_RGB_5_6_5=			Format_SEMANTIC_RGB|Format_TYPE_UINT_5_6_5,
		Format_BGR_5_6_5=			Format_SEMANTIC_BGR|Format_TYPE_UINT_5_6_5,
		Format_RGBA_5_5_5_1=		Format_SEMANTIC_RGBA|Format_TYPE_UINT_5_5_5_1,
		Format_BGRA_5_5_5_1=		Format_SEMANTIC_BGRA|Format_TYPE_UINT_5_5_5_1,
		Format_ARGB_1_5_5_5=		Format_SEMANTIC_ARGB|Format_TYPE_UINT_1_5_5_5,
		Format_ABGR_1_5_5_5=		Format_SEMANTIC_ABGR|Format_TYPE_UINT_1_5_5_5,
		Format_RGBA_4_4_4_4=		Format_SEMANTIC_RGBA|Format_TYPE_UINT_4_4_4_4,
		Format_BGRA_4_4_4_4=		Format_SEMANTIC_BGRA|Format_TYPE_UINT_4_4_4_4,
		Format_ARGB_4_4_4_4=		Format_SEMANTIC_ARGB|Format_TYPE_UINT_4_4_4_4,
		Format_ABGR_4_4_4_4=		Format_SEMANTIC_ABGR|Format_TYPE_UINT_4_4_4_4,
		Format_RGB_DXT1=			Format_SEMANTIC_RGB|Format_TYPE_DXT1,
		Format_RGBA_DXT2=			Format_SEMANTIC_RGBA|Format_TYPE_DXT2,
		Format_RGBA_DXT3=			Format_SEMANTIC_RGBA|Format_TYPE_DXT3,
		Format_RGBA_DXT4=			Format_SEMANTIC_RGBA|Format_TYPE_DXT4,
		Format_RGBA_DXT5=			Format_SEMANTIC_RGBA|Format_TYPE_DXT5,
		Format_YUY2=				Format_SEMANTIC_YUV|Format_TYPE_YUY2,
		Format_NV12=				Format_SEMANTIC_YUV|Format_TYPE_NV12,
	};

	static inline uint8 getRedBits(int format){
		switch(format){
			case Format_RGB_8:
			case Format_RGBA_8:
			case Format_BGR_8:
			case Format_BGRA_8:
				return 8;
			case Format_RGB_F32:
			case Format_RGBA_F32:
			case Format_BGR_F32:
			case Format_BGRA_F32:
				return 32;
			case Format_RGB_5_6_5:
			case Format_RGBA_5_5_5_1:
			case Format_BGR_5_6_5:
			case Format_BGRA_5_5_5_1:
				return 5;
			case Format_RGBA_4_4_4_4:
			case Format_BGRA_4_4_4_4:
				return 4;
			default:
				return 0;
		}
	}		

	static inline uint8 getGreenBits(int format){
		switch(format){
			case Format_RGB_8:
			case Format_RGBA_8:
			case Format_BGR_8:
			case Format_BGRA_8:
				return 8;
			case Format_RGB_F32:
			case Format_RGBA_F32:
			case Format_BGR_F32:
			case Format_BGRA_F32:
				return 32;
			case Format_RGB_5_6_5:
			case Format_BGR_5_6_5:
				return 6;
			case Format_RGBA_5_5_5_1:
			case Format_BGRA_5_5_5_1:
				return 5;
			case Format_RGBA_4_4_4_4:
			case Format_BGRA_4_4_4_4:
				return 4;
			default:
				return 0;
		}
	}		

	static inline uint8 getBlueBits(int format){
		switch(format){
			case Format_RGB_8:
			case Format_RGBA_8:
			case Format_BGR_8:
			case Format_BGRA_8:
				return 8;
			case Format_RGB_F32:
			case Format_RGBA_F32:
			case Format_BGR_F32:
			case Format_BGRA_F32:
				return 32;
			case Format_RGB_5_6_5:
			case Format_RGBA_5_5_5_1:
			case Format_BGR_5_6_5:
			case Format_BGRA_5_5_5_1:
				return 5;
			case Format_RGBA_4_4_4_4:
			case Format_BGRA_4_4_4_4:
				return 4;
			default:
				return 0;
		}
	}		

	static inline uint8 getAlphaBits(int format){
		switch(format){
			case Format_A_8:
			case Format_LA_8:
			case Format_RGBA_8:
			case Format_BGRA_8:
				return 8;
			case Format_A_F32:
			case Format_LA_F32:
			case Format_RGBA_F32:
			case Format_BGRA_F32:
				return 32;
			case Format_RGBA_5_5_5_1:
			case Format_BGRA_5_5_5_1:
				return 1;
			case Format_RGBA_4_4_4_4:
			case Format_BGRA_4_4_4_4:
				return 4;
			default:
				return 0;
		}
	}

	static inline int getPixelSize(int format){
		switch(format){
			case Format_L_8:
			case Format_A_8:
				return sizeof(uint8);
			case Format_LA_8:
				return sizeof(uint8)*2;
			case Format_RGB_8:
			case Format_BGR_8:
				return sizeof(uint8)*3;
			case Format_RGBA_8:
			case Format_BGRA_8:
				return sizeof(uint8)*4;
			case Format_RGB_5_6_5:
			case Format_BGR_5_6_5:
			case Format_RGBA_5_5_5_1:
			case Format_BGRA_5_5_5_1:
			case Format_RGBA_4_4_4_4:
			case Format_BGRA_4_4_4_4:
				return sizeof(uint16);
			case Format_L_F32:
			case Format_A_F32:
				return sizeof(float);
			case Format_LA_F32:
				return sizeof(float)*2;
			case Format_RGB_F32:
			case Format_BGR_F32:
				return sizeof(float)*3;
			case Format_RGBA_F32:
			case Format_BGRA_F32:
				return sizeof(float)*4;
			default:
				return 0;
		}
	}

	TextureFormat(int dimension1,int pixelFormat1,int width,int height,int depth,int mipMax1):
		dimension(dimension1),
		pixelFormat(pixelFormat1),
		xMin(0),yMin(0),zMin(0),
		xMax(width),yMax(height),zMax(depth),
		xPitch(0),yPitch(0),zPitch(0),
		mipMin(0),mipMax(mipMax1)
	{
		TOADLET_ASSERT(getWidth()>=1 && getHeight()>=1 && getDepth()>=1);
		updatePitch();
	}

	TextureFormat(int dimension1,int pixelFormat1,int xMin1,int yMin1,int zMin1,int xMax1,int yMax1,int zMax1,int mip):
		dimension(dimension1),
		pixelFormat(pixelFormat1),
		xMin(xMin1),yMin(yMin1),zMin(zMin1),
		xMax(xMax1),yMax(yMax1),zMax(zMax1),
		xPitch(0),yPitch(0),zPitch(0),
		mipMin(mip),mipMax(mip)
	{
		TOADLET_ASSERT(getWidth()>=1 && getHeight()>=1 && getDepth()>=1);
		updatePitch();
	}

	TextureFormat(const TextureFormat *format,int mipLevel=-1):
		dimension(Dimension_UNKNOWN),
		pixelFormat(Format_UNKNOWN),
		xMin(0),yMin(0),zMin(0),
		xMax(0),yMax(0),zMax(0),
		xPitch(0),yPitch(0),zPitch(0),
		mipMin(0),mipMax(0)
	{
		if(format!=NULL){
			dimension=format->dimension;
			pixelFormat=format->pixelFormat;
			xMin=format->xMin,yMin=format->yMin,zMin=format->zMin;
			xMax=format->xMax,yMax=format->yMax,zMax=format->zMax;
			xPitch=format->xPitch;yPitch=format->yPitch;zPitch=format->zPitch;
			mipMin=format->mipMin;mipMax=format->mipMax;
		}

		if(mipLevel!=-1){
			mipMin=mipLevel;mipMax=mipLevel;
			int div=1<<mipLevel;
			xMin/=div;yMin/=div;zMin/=div;
			xMax/=div;yMax/=div;zMax/=div;
			xMax=xMax>0?xMax:1;yMax=yMax>0?yMax:1;zMax=zMax>0?zMax:1;
			updatePitch();
		}
	}

	void setPixelFormat(int pixelFormat1){
		pixelFormat=pixelFormat1;
		updatePitch();
	}

	void setOrigin(int x,int y,int z){
		xMax=(xMax-xMin)+x;
		yMax=(yMax-yMin)+y;
		zMax=(zMax-zMin)+z;
		xMin=x;
		yMin=y;
		zMin=z;
		TOADLET_ASSERT(getWidth()>=1 && getHeight()>=1 && getDepth()>=1);
		updatePitch();
	}

	void setSize(int width,int height,int depth){
		xMax=xMin+width;
		yMax=yMin+height;
		zMax=zMin+depth;
		TOADLET_ASSERT(getWidth()>=1 && getHeight()>=1 && getDepth()>=1);
		updatePitch();
	}

	void setMips(int mipMin1,int mipMax1){
		mipMin=mipMin1;
		mipMax=mipMax1;
		updatePitch();
	}

	void setPitches(int xPitch1,int yPitch1,int zPitch1){
		xPitch=xPitch1;
		yPitch=yPitch1;
		zPitch=zPitch1;
	}

	inline int getDimension() const{return dimension;}
	inline int getPixelFormat() const{return pixelFormat;}
	inline int getXMin() const{return xMin;}
	inline int getYMin() const{return yMin;}
	inline int getZMin() const{return zMin;}
	inline int getXMax() const{return xMax;}
	inline int getYMax() const{return yMax;}
	inline int getZMax() const{return zMax;}
	inline int getWidth() const{return xMax-xMin;}
	inline int getHeight() const{return yMax-yMin;}
	inline int getDepth() const{return zMax-zMin;}
	inline int getXPitch() const{return xPitch;}
	inline int getYPitch() const{return yPitch;}
	inline int getZPitch() const{return zPitch;}
	inline int getMipMin() const{return mipMin;}
	inline int getMipMax() const{return mipMax;}
	inline int getDataSize(){return zPitch;}

	inline bool isPowerOf2(){
		return Math::isPowerOf2(xMax) && Math::isPowerOf2(yMax) && (dimension==Dimension_CUBE || Math::isPowerOf2(zMax));
	}

	inline bool isSectionOf(TextureFormat *format){
		return xMin>=format->xMin && yMin>=format->yMin && zMin>=format->zMin &&
			xMax<=format->xMax && yMax<=format->yMax && zMax<=format->zMax;
	}

	inline int getMipMaxPossible(){
		return Math::intLog2(Math::maxVal(xMax,Math::maxVal(yMax,zMax)));
	}

	inline int getOffset(int x,int y,int z){
		return z*yPitch + y*xPitch + x*getPixelSize(pixelFormat);
	}

	bool equals(TextureFormat *format) const{
		return dimension==format->dimension && pixelFormat==format->pixelFormat &&
			xMin==format->xMin && yMin==format->yMin && zMin==format->zMin &&
			xMax==format->xMax && yMax==format->yMax && zMax==format->zMax &&
			xPitch==format->xPitch && yPitch==format->yPitch && zPitch==format->zPitch &&
			mipMin==format->mipMin && mipMax==format->mipMax;
	}

protected:
	void updatePitch(){
		switch(pixelFormat){
			case Format_RGB_DXT1:
				xPitch=getWidth()/2;
			break;
			case Format_RGBA_DXT2:
			case Format_RGBA_DXT3:
			case Format_RGBA_DXT4:
			case Format_RGBA_DXT5:
				xPitch=getWidth();
			break;
			case Format_YUY2:
				xPitch=getWidth()*2;
			break;
			case Format_NV12:
				xPitch=0;
			default:
				xPitch=getPixelSize(pixelFormat)*getWidth();
			break;
		}		
		yPitch=xPitch*getHeight();
		zPitch=yPitch*getDepth();
	}

	int dimension;
	int pixelFormat;
	int xMin,yMin,zMin;
	int xMax,yMax,zMax;
	int xPitch,yPitch,zPitch;
	int mipMin,mipMax;
};

}
}

#endif
