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
#include <toadlet/peeper/BackableTextureMipPixelBuffer.h>

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
{}

BackableTexture::~BackableTexture(){
	destroy();
}

bool BackableTexture::create(int usage,Dimension dimension,int format,int width,int height,int depth,int mipLevels,tbyte *mipDatas[]){
	destroy();

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

	mData=new tbyte[mDataSize];

	if(mipDatas!=NULL){
		load(width,height,depth,0,mipDatas[0]);
	}

	return true;
}

void BackableTexture::destroy(){
	int i;
	for(i=0;i<mBuffers.size();++i){
		if(mBuffers[i]!=NULL){
			mBuffers[i]->destroy();
		}
	}
	mBuffers.clear();

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

		int i;
		for(i=0;i<mBuffers.size();++i){
			if(mBuffers[i]!=NULL){
				mBuffers[i]->resetCreate();
			}
		}
	}
}

void BackableTexture::resetDestroy(){
	if(mBack!=NULL){
		int i;
		for(i=0;i<mBuffers.size();++i){
			if(mBuffers[i]!=NULL){
				mBuffers[i]->resetDestroy();
			}
		}

		mBack->resetDestroy();
	}
}

PixelBuffer::ptr BackableTexture::getMipPixelBuffer(int level,int cubeSide){
	int index=level;
	if(mDimension==Dimension_CUBE){
		index=level*6+cubeSide;
	}

	if(mBuffers.size()<=index){
		mBuffers.resize(index+1);
	}

	if(mBuffers[index]==NULL){
		PixelBuffer::ptr buffer(new BackableTextureMipPixelBuffer(this,level,cubeSide));
		mBuffers[index]=buffer;
	}

	return mBuffers[index];
}

bool BackableTexture::load(int width,int height,int depth,int mipLevel,tbyte *mipData){
	if(mBack!=NULL){
		mBack->load(width,height,depth,mipLevel,mipData);
	}

	if(mipLevel==0){
		ImageFormatConversion::convert(mipData,mFormat,mRowPitch,mSlicePitch,mData,mFormat,mRowPitch,mSlicePitch,width,height,depth);
		return true;
	}
	else{
		return false;
	}
}

bool BackableTexture::read(int width,int height,int depth,int mipLevel,tbyte *mipData){
	if(mipLevel==0){
		ImageFormatConversion::convert(mData,mFormat,mRowPitch,mSlicePitch,mipData,mFormat,mRowPitch,mSlicePitch,width,height,depth);
		return true;
	}
	else{
		return false;
	}
}

void BackableTexture::setBack(Texture::ptr back){
	int i;
	if(back!=mBack && mBack!=NULL){
		for(i=0;i<mBuffers.size();++i){
			if(mBuffers[i]!=NULL){
				mBuffers[i]->resetDestroy();
			}
		}

		mBack->destroy();
	}

	mBack=back;
	
	if(mBack!=NULL){
		if(mData!=NULL){
			// Create texture on setting the back, otherwise D3D10 static textures will not load data in load
			tbyte *mipDatas[1]={mData};
			mBack->create(mUsage,mDimension,mFormat,mWidth,mHeight,mDepth,1,mipDatas);
		}

		for(i=0;i<mBuffers.size();++i){
			if(mBuffers[i]!=NULL){
				mBuffers[i]->resetCreate();
			}
		}
	}
}

}
}
