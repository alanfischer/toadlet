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

#ifndef TOADLET_EGG_IMAGE_IMAGEFORMATCONVERSION_H
#define TOADLET_EGG_IMAGE_IMAGEFORMATCONVERSION_H

#include <toadlet/egg/image/ImageDefinitions.h>

namespace toadlet{
namespace egg{
namespace image{

class TOADLET_API ImageFormatConversion:public ImageDefinitions{
public:
	static inline uint8 getRedFromRGB565(const uint16 pixel){return (pixel&0xF800)>>8;}
	static inline uint8 getGreenFromRGB565(const uint16 pixel){return (pixel&0x7E0)>>3;}
	static inline uint8 getBlueFromRGB565(const uint16 pixel){return (pixel&0x1F)<<3;}

	static inline uint8 getRedFromRGBA5551(const uint16 pixel){return (pixel&0xF800)>>8;}
	static inline uint8 getGreenFromRGBA5551(const uint16 pixel){return (pixel&0x7C0)>>3;}
	static inline uint8 getBlueFromRGBA5551(const uint16 pixel){return (pixel&0x3E)<<2;}
	static inline uint8 getAlphaFromRGBA5551(const uint16 pixel){return (pixel&0x1)<<7;}

	static inline uint8 getRedFromRGBA4444(const uint16 pixel){return (pixel&0xF000)>>8;}
	static inline uint8 getGreenFromRGBA4444(const uint16 pixel){return (pixel&0xF00)>>4;}
	static inline uint8 getBlueFromRGBA4444(const uint16 pixel){return (pixel&0xF0);}
	static inline uint8 getAlphaFromRGBA4444(const uint16 pixel){return (pixel&0xF)<<4;}

	static inline uint16 makeRGB565(const uint8 red,const uint8 green,const uint8 blue){
		return ((red<<8)&0xF800) | ((green<<3)&0x7E0) | ((blue>>3)&0x1F);
	}

	static inline uint16 makeRGBA5551(const uint8 red,const uint8 green,const uint8 blue,const uint8 alpha){
		return ((red<<8)&0xF800) | ((green<<3)&0x7C0) | ((blue>>2)&0x3E) | ((alpha>>7)&0x1);
	}

	static inline uint16 makeRGBA4444(const uint8 red,const uint8 green,const uint8 blue,const uint8 alpha){
		return ((red<<8)&0xF000) | ((green<<4)&0xF00) | ((blue)&0xF0) | ((alpha<<4)&0xF);
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

	static inline int getRowPitch(int format,int width){
		switch(format){
			case Format_RGB_DXT1:
				return width/2;
			case Format_RGBA_DXT2:
			case Format_RGBA_DXT3:
			case Format_RGBA_DXT4:
			case Format_RGBA_DXT5:
				return width;
			default:
				return getPixelSize(format)*width;
		}
	}

	static bool isFormatCompressed(int format){
		switch(format){
			case Format_RGB_DXT1:
			case Format_RGBA_DXT2:
			case Format_RGBA_DXT3:
			case Format_RGBA_DXT4:
			case Format_RGBA_DXT5:
				return true;
			default:
				return false;
		}
	}

	static bool convert(tbyte *src,int srcFormat,int srcRowPitch,int srcSlicePitch,tbyte *dst,int dstFormat,int dstRowPitch,int dstSlicePitch,int width,int height,int depth);
};

}
}
}

#endif
