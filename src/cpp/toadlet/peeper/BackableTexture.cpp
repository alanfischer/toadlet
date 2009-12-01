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

#include <toadlet/egg/Logger.h>
#include <toadlet/peeper/BackableTexture.h>
#include <string.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;

namespace toadlet{
namespace peeper{

BackableTexture::BackableTexture():BaseResource(),
	mUsageFlags(0),
	mDimension(Dimension_UNKNOWN),
	mFormat(0),
	mWidth(0),
	mHeight(0),
	mDepth(0),
	mMipLevels(0),
	
	mDataSize(0),
	mData(NULL)
	//mBack
{
}

BackableTexture::~BackableTexture(){
	destroy();
}

bool BackableTexture::create(int usageFlags,Dimension dimension,int format,int width,int height,int depth,int mipLevels){
	destroy();

	if(mipLevels==0){
		int w=width,h=height;
		while(w>0 && h>0){
			mipLevels++;
			w/=2; h/=2;
		}
	}

	mUsageFlags=usageFlags;
	mDimension=dimension;
	mFormat=format;
	mWidth=width;
	mHeight=height;
	mDepth=depth;
	mMipLevels=mipLevels;

	if(mDimension==Dimension_D1){
		mDataSize=ImageFormatConversion::getPixelSize(format)*mWidth;
	}
	else if(mDimension==Dimension_D2){
		mDataSize=ImageFormatConversion::getPixelSize(format)*mWidth*mHeight;
	}
	else if(mDimension==Dimension_D3){
		mDataSize=ImageFormatConversion::getPixelSize(format)*mWidth*mHeight*mDepth;
	}
	else if(mDimension==Dimension_CUBEMAP){
		mDataSize=ImageFormatConversion::getPixelSize(format)*mWidth*mHeight*6;
	}

	mData=new uint8[mDataSize];

	return true;
}

void BackableTexture::destroy(){
	if(mBack!=NULL){
		mBack->destroy();
	}

	if(mData!=NULL){
		delete[] mData;
		mData=NULL;
	}
}

// TODO: This should return a BackableSurface, which will be able to change surface pointers
Surface::ptr BackableTexture::getMipSuface(int i){
	return mBack->getMipSuface(i);
}

bool BackableTexture::load(int format,int width,int height,int depth,int mipLevel,uint8 *data){
	if(mBack!=NULL){
		return mBack->load(format,width,height,depth,mipLevel,data);
	}
	else{
		// TODO: This should modify the data to fit into our buffer
		if(mipLevel==0){
			memcpy(mData,data,mDataSize);
			return true;
		}
		else{
			return false;
		}
	}
}

bool BackableTexture::read(int format,int width,int height,int depth,int mipLevel,uint8 *data){
	if(mBack!=NULL){
		return mBack->read(format,width,height,depth,mipLevel,data);
	}
	else{
		// TODO: This should modify the data to fit into our buffer
		if(mipLevel==0){
			memcpy(data,mData,mDataSize);
			return true;
		}
		else{
			return false;
		}
	}
}

void BackableTexture::setBack(Texture::ptr back,bool initial){
	if(back!=mBack && mBack!=NULL){
		mData=new uint8[mDataSize];
		mBack->read(mFormat,mWidth,mHeight,mDepth,0,mData);
	}

	mBack=back;
	
	if(initial==false && mBack!=NULL && mData!=NULL){
		mBack->load(mFormat,mWidth,mHeight,mDepth,0,mData);
		delete[] mData;
		mData=NULL;
	}
}

}
}
