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

#ifndef TOADLET_EGG_IMAGE_IMAGE_H
#define TOADLET_EGG_IMAGE_IMAGE_H

#include <toadlet/Types.h>
#include <toadlet/egg/SharedPointer.h>
#include <toadlet/egg/WeakPointer.h>
#include <toadlet/egg/image/ImageDefinitions.h>
#include <toadlet/egg/image/ImageFormatConversion.h>

namespace toadlet{
namespace egg{
namespace image{

template <typename T> class Pixel{
public:
	Pixel(T red=0,T green=0,T blue=0,T alpha=0):
		r(red),
		g(green),
		b(blue),
		a(alpha)
	{}
	
	inline T getRed() const{return r;}
	inline T getGreen() const{return g;}
	inline T getBlue() const{return b;}
	inline T getAlpha() const{return a;}
	inline T getLuminance() const{return r;}
	
	T r,g,b,a;
};

class TOADLET_API Image:public ImageDefinitions{
public:
	TOADLET_SHARED_POINTERS(Image);

	Image();
	Image(Dimension dimension,int format,int width,int height=1,int depth=1);

	virtual ~Image();

	Image *clone();

	bool reallocate(Dimension dimension,int format,int width,int height=1,int depth=1,bool clear=true);
	bool reallocate(const Image *image,bool clear);

	bool hasSameParametersAs(const Image *image) const;

	inline int getWidth() const{return mWidth;}
	inline int getHeight() const{return mHeight;}
	inline int getDepth() const{return mDepth;}

	inline Dimension getDimension() const{return mDimension;}
	inline int getFormat() const{return mFormat;}
	inline tbyte *getData(){return mData;}
	inline const tbyte *getData() const{return mData;}
	
	inline int getPixelSize() const{return ImageFormatConversion::getPixelSize(mFormat);}
	inline int getRowPitch() const{return getPixelSize()*mWidth;}
	inline int getSlicePitch() const{return getRowPitch()*mHeight;}

	void getPixel(Pixel<uint8> &pixel,int x,int y=0,int z=0) const{
		int o=((z*mHeight*mWidth)+(y*mWidth)+x);

		switch(mFormat){
			case Format_RGBA_8:
				o*=4;
				pixel.a=mData[o+3];
				pixel.b=mData[o+2];
				pixel.g=mData[o+1];
				pixel.r=mData[o+0];
			break;
			case Format_BGRA_8:
				o*=4;
				pixel.a=mData[o+3];
				pixel.r=mData[o+2];
				pixel.g=mData[o+1];
				pixel.b=mData[o+0];
			break;
			case Format_RGB_8:
				o*=3;
				pixel.r=mData[o+0];
				pixel.g=mData[o+1];
				pixel.b=mData[o+2];
			break;
			case Format_BGR_8:
				o*=3;
				pixel.r=mData[o+2];
				pixel.g=mData[o+1];
				pixel.b=mData[o+0];
			break;
			case Format_LA_8:
				o*=2;
				pixel.a=mData[o+1];
				pixel.r=mData[o+0];
			break;
			case Format_L_8:
				pixel.r=mData[o+0];
			break;
			case Format_A_8:
				pixel.a=mData[o+0];
			break;
			case Format_RGB_5_6_5:
				pixel.r=ImageFormatConversion::getRedFromRGB565(((uint16*)mData)[o]);
				pixel.g=ImageFormatConversion::getGreenFromRGB565(((uint16*)mData)[o]);
				pixel.b=ImageFormatConversion::getBlueFromRGB565(((uint16*)mData)[o]);
			break;
			case Format_RGBA_5_5_5_1:
				pixel.r=ImageFormatConversion::getRedFromRGBA5551(((uint16*)mData)[o]);
				pixel.g=ImageFormatConversion::getGreenFromRGBA5551(((uint16*)mData)[o]);
				pixel.b=ImageFormatConversion::getBlueFromRGBA5551(((uint16*)mData)[o]);
				pixel.a=ImageFormatConversion::getAlphaFromRGBA5551(((uint16*)mData)[o]);
			break;
			case Format_RGBA_4_4_4_4:
				pixel.r=ImageFormatConversion::getRedFromRGBA4444(((uint16*)mData)[o]);
				pixel.g=ImageFormatConversion::getGreenFromRGBA4444(((uint16*)mData)[o]);
				pixel.b=ImageFormatConversion::getBlueFromRGBA4444(((uint16*)mData)[o]);
				pixel.a=ImageFormatConversion::getAlphaFromRGBA4444(((uint16*)mData)[o]);
			break;
			default:
				TOADLET_ASSERT("uint8 pixel not valid with this type of image" && false);
		}
	}

	void getPixel(Pixel<float> &pixel,int x,int y=0,int z=0) const{
		int o=((z*mHeight*mWidth)+(y*mWidth)+x);

		switch(mFormat){
			case ImageFormatConversion::Format_RGBA_F32:
				o*=4;
				pixel.a=((float*)mData)[o+3];
				pixel.b=((float*)mData)[o+2];
				pixel.g=((float*)mData)[o+1];
				pixel.r=((float*)mData)[o+0];
			break;	
			case ImageFormatConversion::Format_RGB_F32:
				o*=3;
				pixel.b=((float*)mData)[o+2];
				pixel.g=((float*)mData)[o+1];
				pixel.r=((float*)mData)[o+0];
			break;
			case ImageFormatConversion::Format_LA_F32:
				o*=2;
				pixel.a=((float*)mData)[o+1];
				pixel.r=((float*)mData)[o+0];
			break;
			case ImageFormatConversion::Format_L_F32:
				pixel.r=((float*)mData)[o+0];
			break;
			case ImageFormatConversion::Format_A_F32:
				pixel.a=((float*)mData)[o+0];
			break;
			default:
				TOADLET_ASSERT("float pixel not valid with this type of image" && false);
		}
	}

	void setPixel(const Pixel<uint8> &pixel,int x,int y=0,int z=0){
		int o=((z*mHeight*mWidth)+(y*mWidth)+x);

		switch(mFormat){
			case Format_RGBA_8:
				o*=4;
				mData[o+3]=pixel.a;
				mData[o+2]=pixel.b;
				mData[o+1]=pixel.g;
				mData[o+0]=pixel.r;
			break;
			case Format_BGRA_8:
				o*=4;
				mData[o+3]=pixel.a;
				mData[o+2]=pixel.r;
				mData[o+1]=pixel.g;
				mData[o+0]=pixel.b;
			break;
			case Format_RGB_8:
				o*=3;
				mData[o+2]=pixel.b;
				mData[o+1]=pixel.g;
				mData[o+0]=pixel.r;
			break;
			case Format_BGR_8:
				o*=3;
				mData[o+2]=pixel.r;
				mData[o+1]=pixel.g;
				mData[o+0]=pixel.b;
			break;
			case Format_LA_8:
				o*=2;
				mData[o+1]=pixel.a;
				mData[o+0]=pixel.r;
			break;
			case Format_L_8:
				mData[o+0]=pixel.r;
			break;
			case Format_A_8:
				mData[o+0]=pixel.a;
			break;
			case Format_RGB_5_6_5:
				((uint16*)mData)[o]=ImageFormatConversion::makeRGB565(pixel.r,pixel.g,pixel.b);
			break;
			case Format_RGBA_5_5_5_1:
				((uint16*)mData)[o]=ImageFormatConversion::makeRGBA5551(pixel.r,pixel.g,pixel.b,pixel.a);
			break;
			case Format_RGBA_4_4_4_4:
				((uint16*)mData)[o]=ImageFormatConversion::makeRGBA4444(pixel.r,pixel.g,pixel.b,pixel.a);
			break;
			default:
				TOADLET_ASSERT("uint8 pixel not valid with this type of image" && false);
		}
	}

	void setPixel(const Pixel<float> &pixel,int x,int y=0,int z=0){
		int o=((z*mHeight*mWidth)+(y*mWidth)+x);

		switch(mFormat){
			case Format_RGBA_F32:
				o*=4;
				((float*)mData)[o+3]=pixel.a;
				((float*)mData)[o+2]=pixel.b;
				((float*)mData)[o+1]=pixel.g;
				((float*)mData)[o+0]=pixel.r;
			break;	
			case Format_RGB_F32:
				o*=3;
				((float*)mData)[o+2]=pixel.b;
				((float*)mData)[o+1]=pixel.g;
				((float*)mData)[o+0]=pixel.r;
			break;
			case Format_LA_F32:
				o*=2;
				((float*)mData)[o+1]=pixel.a;
				((float*)mData)[o+0]=pixel.r;
			break;
			case Format_L_F32:
				((float*)mData)[o+0]=pixel.r;
			break;
			case Format_A_F32:
				((float*)mData)[o+0]=pixel.a;
			break;
			default:
				TOADLET_ASSERT("float pixel not valid with this type of image" && false);
		}
	}

protected:
	Dimension mDimension;
	int mFormat;
	int mWidth,mHeight,mDepth;
	tbyte *mData;
};

}
}
}

#endif

