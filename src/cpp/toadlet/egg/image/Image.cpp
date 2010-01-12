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

#include <toadlet/egg/image/Image.h>
#include <toadlet/egg/Error.h>
#include <string.h> // memcpy

namespace toadlet{
namespace egg{
namespace image{

Image::Image(){
	mDimension=Dimension_UNKNOWN;
	mFormat=Format_UNKNOWN;
	mWidth=0;
	mHeight=0;
	mDepth=0;
	mData=NULL;
}

Image::Image(Dimension dimension,int format,unsigned int width,unsigned int height,unsigned int depth){
	mData=NULL;
	reallocate(dimension,format,width,height,depth,true);
}

Image::~Image(){
	if(mData){
		delete[] mData;
		mData=NULL;
	}
	
	mDimension=Dimension_UNKNOWN;
	mFormat=Format_UNKNOWN;
	mWidth=0;
	mHeight=0;
	mDepth=0;
}

Image *Image::clone(){
	Image *image=new Image();

	image->reallocate(mDimension,mFormat,mWidth,mHeight,mDepth,false);

	memcpy(image->mData,mData,getPixelSize()*mWidth*mHeight*mDepth);

	return image;
}

bool Image::reallocate(Dimension dimension,int format,unsigned int width,unsigned int height,unsigned int depth,bool clear){
	if(mData){
		delete[] mData;
		mData=NULL;
	}

	if(dimension==Dimension_UNKNOWN || format==Format_UNKNOWN || width<1 || height<1 || depth<1){
		Error::invalidParameters(Categories::TOADLET_EGG,
			"Image::reallocate: Categories parameters");
		return false; 
	}

	if(dimension==Dimension_CUBE && depth!=CubeSide_MAX){
		Error::invalidParameters(Categories::TOADLET_EGG,
			"Image::reallocate: Dimension_CUBEMAP not used with depth of CubeSide_MAX");
		return false; 
	}

	mDimension=dimension;
	mFormat=format;
	mWidth=width;
	mHeight=height;
	mDepth=depth;

	int size=getPixelSize()*mWidth*mHeight*mDepth;
	mData=new byte[size];
	if(clear){
		memset(mData,0,size);
	}

	return true;
}

bool Image::reallocate(const Image *image,bool clear){
	return reallocate(image->mDimension,image->mFormat,image->mWidth,image->mHeight,image->mDepth,clear);
}

bool Image::hasSameParametersAs(const Image *image) const{
	return
		mDimension==image->mDimension &&
		mFormat==image->mFormat && 
		mWidth==image->mWidth &&
		mHeight==image->mHeight &&
		mDepth==image->mDepth;
}

unsigned int Image::getPixelSize() const{
	return ImageFormatConversion::getPixelSize(mFormat);
}

}
}
}

