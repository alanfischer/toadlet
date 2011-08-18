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

namespace toadlet{
namespace egg{
namespace image{

Image *Image::createAndReallocate(int dimension,int format,int width,int height,int depth){
	Image *image=new Image();
	
	bool result=false;
	TOADLET_TRY
		result=image->reallocate(dimension,format,width,height,depth);
	TOADLET_CATCH(Exception){
		result=false;
	}
	
	if(result==false){
		delete image;
		Error::insufficientMemory(Categories::TOADLET_EGG,
			"Image::createAndReallocate");
		return NULL;
	}
	
	return image;
}

Image::Image(){
	mDimension=Dimension_UNKNOWN;
	mFormat=Format_UNKNOWN;
	mWidth=0;
	mHeight=0;
	mDepth=0;
	mData=NULL;
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

	bool result=false;
	TOADLET_TRY
		result=image->reallocate(mDimension,mFormat,mWidth,mHeight,mDepth,false);
	TOADLET_CATCH(Exception){
		result=false;
	}

	if(result==true){
		memcpy(image->mData,mData,getSlicePitch()*mDepth);
	}
	else{
		delete image;
		Error::insufficientMemory(Categories::TOADLET_EGG,
			"Image::clone");
		return NULL;
	}

	return image;
}

bool Image::reallocate(int dimension,int format,int width,int height,int depth,bool clear){
	if(mData){
		delete[] mData;
		mData=NULL;
	}

	if(width<=0) width=1;
	if(height<=0) height=1;
	if(depth<=0) depth=1;

	if(dimension==Dimension_UNKNOWN || format==Format_UNKNOWN){
		Error::invalidParameters(Categories::TOADLET_EGG,
			"Image::reallocate: invalid parameters");
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

	int size=getSlicePitch()*mDepth;
	mData=new tbyte[size];
	if(mData==NULL){
		Error::insufficientMemory(Categories::TOADLET_EGG,
			"Image::reallocate");
		return false;
	}

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

}
}
}

