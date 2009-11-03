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

#include <toadlet/peeper/BackableTexture.h>
#include <string.h>

using namespace toadlet::egg::image;

namespace toadlet{
namespace peeper{

BackableTexture::BackableTexture():
	//mName,
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

	mUsageFlags=usageFlags;
	mDimension=dimension;
	mFormat=format;
	mWidth=width;
	mHeight=height;
	mDepth=depth;
	mMipLevels=mipLevels;
	mDataSize=ImageFormatConversion::getPixelSize(format)*mWidth*mHeight*mDepth;
	
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
Surface::ptr BackableTexture::getMipSuface(int i) const{
	return mBack->getMipSuface(i);
}

bool BackableTexture::load(int format,int width,int height,int depth,uint8 *data){
	if(mBack!=NULL){
		return mBack->load(format,width,height,depth,data);
	}
	else{
		// TODO: This should modify the data to fit into our buffer
		memcpy(mData,data,mDataSize);
		return true;
	}
}

bool BackableTexture::read(int format,int width,int height,int depth,uint8 *data){
	if(mBack!=NULL){
		return mBack->read(format,width,height,depth,data);
	}
	else{
		// TODO: This should modify the data to fit into our buffer
		memcpy(data,mData,mDataSize);
		return true;
	}
}

void BackableTexture::setBack(Texture::ptr back){
	if(back!=mBack && mBack!=NULL){
		mData=new uint8[mDataSize];
		mBack->read(mFormat,mWidth,mHeight,mDepth,mData);
	}

	mBack=back;
	
	if(mBack!=NULL && mData!=NULL){
		mBack->load(mFormat,mWidth,mHeight,mDepth,mData);
		delete[] mData;
		mData=NULL;
	}
}

}
}
