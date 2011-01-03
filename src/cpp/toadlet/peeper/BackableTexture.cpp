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
	mUsage(0),
	mDimension(Dimension_UNKNOWN),
	mFormat(0),
	mWidth(0),
	mHeight(0),
	mDepth(0),
	
	mRowPitch(0),
	mSlicePitch(0),
	mDataSize(0),
	mData(NULL)
	//mBack
{
}

BackableTexture::~BackableTexture(){
	destroy();
}

bool BackableTexture::create(int usage,Dimension dimension,int format,int width,int height,int depth,int mipLevels,tbyte *mipDatas[]){
	mUsage=usage;
	mDimension=dimension;
	mFormat=format;
	mWidth=width;
	mHeight=height;
	mDepth=depth;

	mRowPitch=ImageFormatConversion::getPixelSize(mFormat)*mWidth;
	mSlicePitch=mRowPitch*mHeight;
	if(mDimension==Dimension_D1){
		mDataSize=mRowPitch;
	}
	else if(mDimension==Dimension_D2){
		mDataSize=mSlicePitch;
	}
	else if(mDimension==Dimension_D3 || mDimension==Dimension_CUBE){
		mDataSize=mSlicePitch*mDepth;
	}

	if((usage&Usage_BIT_STAGING)==0){
		// BackableTextures only store the first mipLevel, so if it's not a staging resource, then we need to autogen mipmaps
		mUsage|=Usage_BIT_AUTOGEN_MIPMAPS;
	}

	if(mData!=NULL){
		delete[] mData;
		mData=NULL;
	}

	mData=new tbyte[mDataSize];

	if(mipDatas!=NULL){
		load(width,height,depth,0,mipDatas[0]);
	}

	return true;
}

void BackableTexture::destroy(){
	if(mBack!=NULL){
		mBack->destroy();
		mBack=NULL;
	}

	if(mData!=NULL){
		delete[] mData;
		mData=NULL;
	}
}

void BackableTexture::resetCreate(){
	if(mBack!=NULL){
		mBack->resetCreate();
	}
}

void BackableTexture::resetDestroy(){
	if(mBack!=NULL){
		mBack->resetDestroy();
	}
}

/// @todo: This should return a BackablePixelBuffer, which will be able to change surface pointers
PixelBuffer::ptr BackableTexture::getMipPixelBuffer(int level,int cubeSide){
	return mBack->getMipPixelBuffer(level,cubeSide);
}

bool BackableTexture::load(int width,int height,int depth,int mipLevel,tbyte *mipData){
	if(mBack!=NULL){
		return mBack->load(width,height,depth,mipLevel,mipData);
	}
	else{
		if(mipLevel==0){
			ImageFormatConversion::convert(mipData,mFormat,mRowPitch,mSlicePitch,mData,mFormat,mRowPitch,mSlicePitch,width,height,depth);
			return true;
		}
		else{
			return false;
		}
	}
}

bool BackableTexture::read(int width,int height,int depth,int mipLevel,tbyte *mipData){
	if(mBack!=NULL){
		return mBack->read(width,height,depth,mipLevel,mipData);
	}
	else{
		if(mipLevel==0){
			ImageFormatConversion::convert(mData,mFormat,mRowPitch,mSlicePitch,mipData,mFormat,mRowPitch,mSlicePitch,width,height,depth);
			return true;
		}
		else{
			return false;
		}
	}
}

void BackableTexture::setBack(Texture::ptr back){
	if(back!=mBack && mBack!=NULL){
		mData=new uint8[mDataSize];
		mBack->read(mWidth,mHeight,mDepth,0,mData);
		mBack->destroy();
	}

	mBack=back;
	
	if(mBack!=NULL && mData!=NULL){
		// Create texture on setting the back, otherwise D3D10 static textures will not load data in load
		if(true){
			tbyte *mipDatas[1]={mData};
			mBack->create(mUsage,mDimension,mFormat,mWidth,mHeight,mDepth,0,mipDatas);
		}
		else{
			mBack->load(mWidth,mHeight,mDepth,0,mData);
		}
		delete[] mData;
		mData=NULL;
	}
}

}
}
