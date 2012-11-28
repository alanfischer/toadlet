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

#include <toadlet/egg/Log.h>
#include <toadlet/peeper/BackableTexture.h>
#include <toadlet/peeper/BackableTextureMipPixelBuffer.h>
#include <toadlet/peeper/RenderCaps.h>
#include <toadlet/peeper/TextureFormatConversion.h>

namespace toadlet{
namespace peeper{

BackableTexture::BackableTexture():BaseResource(),
	mUsage(0),
	//mFormat,
	
	mData(NULL)
	//mBack
{}

bool BackableTexture::create(int usage,TextureFormat::ptr format,tbyte *mipDatas[]){
	mUsage=usage;
	mFormat=new TextureFormat(format);

	// BackableTextures only store the first mipLevel, so if it's not a staging resource and has multiple mip levels, then we need to autogen mipmaps
	if((usage&Usage_BIT_STAGING)==0 && format->getMipMax()!=1){
		mUsage|=Usage_BIT_AUTOGEN_MIPMAPS;
	}

	// BackableTextures only keep a Data allocation if its not Dyanmic
	if((usage&(Usage_BIT_DYNAMIC|Usage_BIT_RENDERTARGET))==0){
		mData=new tbyte[format->getDataSize()];
	}

	if(mipDatas!=NULL){
		load(format,mipDatas[0]);
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

	BaseResource::destroy();
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
	if(mFormat->getDimension()==TextureFormat::Dimension_CUBE){
		index=level*6+cubeSide;
	}

	if(mBuffers.size()<=index){
		mBuffers.resize(index+1);
	}

	if(mBuffers[index]==NULL){
		PixelBuffer::ptr buffer=new BackableTextureMipPixelBuffer(this,level,cubeSide);
		mBuffers[index]=buffer;
	}

	return mBuffers[index];
}

bool BackableTexture::load(TextureFormat *format,tbyte *data){
	bool result=false;

	if(mBack!=NULL){
		result=convertLoad(mBack,format,data);
	}
	
	if(format->getMipMin()==0 && mData!=NULL){
		if(format->getWidth()!=mFormat->getWidth() || format->getHeight()!=mFormat->getHeight() || format->getDepth()!=mFormat->getDepth()){
			TextureFormat *tempFormat=format;
			tbyte *tempData=data;
			if(format->getPixelFormat()!=mFormat->getPixelFormat()){
				tempFormat=new TextureFormat(format);
				tempFormat->setPixelFormat(mFormat->getPixelFormat());
				tempData=new tbyte[tempFormat->getDataSize()];

				result=TextureFormatConversion::convert(data,format,tempData,tempFormat);
			}

			int j,k;
			for(k=0;k<tempFormat->getDepth();++k){
				for(j=0;j<tempFormat->getHeight();++j){
					tbyte *dst=mData+mFormat->getOffset(tempFormat->getXMin(),tempFormat->getYMin()+j,tempFormat->getZMin()+k);
					tbyte *src=tempData+tempFormat->getOffset(0,j,k);
					memcpy(dst,src,tempFormat->getXPitch());
				}
			}
			
			if(format->getPixelFormat()!=mFormat->getPixelFormat()){
				delete tempFormat;
				delete[] tempData;
			}
		}
		else{
			result=TextureFormatConversion::convert(data,format,mData,mFormat);
		}
	}

	return result;
}

bool BackableTexture::read(TextureFormat *format,tbyte *data){
	bool result=false;

	if(mBack!=NULL){
		result=convertRead(mBack,format,data);
	}
	else if(format->getMipMax()<=1 && mData!=NULL){
		if(format->getWidth()!=mFormat->getWidth() || format->getHeight()!=mFormat->getHeight() || format->getDepth()!=mFormat->getDepth()){
			TextureFormat *tempFormat=format;
			tbyte *tempData=data;
			if(format->getPixelFormat()!=mFormat->getPixelFormat()){
				tempFormat=new TextureFormat(format);
				tempFormat->setPixelFormat(mFormat->getPixelFormat());
				tempData=new tbyte[tempFormat->getDataSize()];
			}

			int j,k;
			for(k=0;k<tempFormat->getDepth();++k){
				for(j=0;j<tempFormat->getHeight();++j){
					tbyte *dst=tempData+tempFormat->getOffset(0,j,k);
					tbyte *src=mData+mFormat->getOffset(tempFormat->getXMin(),tempFormat->getYMin()+j,tempFormat->getZMin()+k);
					memcpy(dst,src,tempFormat->getXPitch());
				}
			}
			
			if(format->getPixelFormat()!=mFormat->getPixelFormat()){
				result=TextureFormatConversion::convert(tempData,tempFormat,data,format);

				delete tempFormat;
				delete[] tempData;
			}
		}
		else{
			result=TextureFormatConversion::convert(mData,mFormat,data,format);
		}
	}

	return result;
}

void BackableTexture::setBack(Texture::ptr back,RenderDevice *renderDevice){
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
		TextureFormat::ptr format=new TextureFormat(mFormat);
		format->setMips(0,(mUsage&Usage_BIT_AUTOGEN_MIPMAPS)!=0?0:1);
		convertCreate(mBack,renderDevice,mUsage,format,(mData!=NULL?&mData:NULL));

		for(i=0;i<mBuffers.size();++i){
			if(mBuffers[i]!=NULL){
				mBuffers[i]->resetCreate();
			}
		}
	}
}

bool BackableTexture::convertLoad(Texture::ptr texture,TextureFormat *format,tbyte *data){
	if(texture->getFormat()->getPixelFormat()==format->getPixelFormat()){
		return texture->load(format,data);
	}
	else{
		TextureFormat *textureFormat=new TextureFormat(format);
		textureFormat->setPixelFormat(texture->getFormat()->getPixelFormat());
		int textureSize=textureFormat->getDataSize();
		tbyte *textureData=new tbyte[textureSize];
		TextureFormatConversion::convert(data,format,textureData,textureFormat);
		bool result=texture->load(textureFormat,textureData);
		delete[] textureData;
		delete textureFormat;
		return result;
	}
}

bool BackableTexture::convertRead(Texture::ptr texture,TextureFormat *format,tbyte *data){
	if(texture->getFormat()->getPixelFormat()==format->getPixelFormat()){
		return texture->read(format,data);
	}
	else{
		int textureSize=texture->getFormat()->getDataSize();
		tbyte *textureData=new tbyte[textureSize];
		TextureFormat *textureFormat=new TextureFormat(format);
		textureFormat->setPixelFormat(texture->getFormat()->getPixelFormat());
		bool result=texture->read(textureFormat,textureData);
		TextureFormatConversion::convert(textureData,textureFormat,data,format);
		delete textureFormat;
		delete[] textureData;
		return result;
	}
}

bool BackableTexture::convertCreate(Texture::ptr texture,RenderDevice *renderDevice,int usage,TextureFormat::ptr format,tbyte *mipDatas[]){
	RenderCaps caps;
	renderDevice->getRenderCaps(caps);
	int newPixelFormat=renderDevice->getClosePixelFormat(format->getPixelFormat(),usage);
	bool hasNPOT=caps.textureNonPowerOf2;
	bool wantsNPOT=!format->isPowerOf2();
	bool needsNPOT=wantsNPOT & !hasNPOT;
	bool hasAutogen=caps.textureAutogenMipMaps;
	bool wantsAutogen=(usage&Texture::Usage_BIT_AUTOGEN_MIPMAPS)>0;
	bool needsAutogen=wantsAutogen & !hasAutogen;
	bool needsConvert=(newPixelFormat!=format->getPixelFormat());

	bool result=false;
	if(mipDatas==NULL){
		TextureFormat::ptr newFormat=new TextureFormat(format);
		newFormat->setPixelFormat(newPixelFormat);
		result=texture->create(usage,newFormat,NULL);
	}
	else{
		if(format->getMipMax()==0 && wantsAutogen==false){
			Log::debug(Categories::TOADLET_PEEPER,
				"Auto calculated mip levels specified with no autogen, non zero levels may be empty");
		}

		int totalMipLevels=format->getMipMaxPossible();
		int specifiedMipLevels=0;
		int allocatedMipLevels=0;
		if(needsAutogen || format->getMipMax()>0){
			if(format->getMipMax()>0){
				specifiedMipLevels=format->getMipMax();
			}
			else{
				specifiedMipLevels=totalMipLevels;
			}
			allocatedMipLevels=specifiedMipLevels;
		}
		else{
			allocatedMipLevels=1;
		}

		int i;
		if(!needsNPOT && !needsAutogen && !needsConvert){
			result=texture->create(usage,format,mipDatas);
		}
		else{
			Log::alert(Categories::TOADLET_PEEPER,String("BackableTexture::convertCreate - converting texture:")+needsNPOT+","+needsAutogen+","+needsConvert);

			TextureFormat::ptr newFormat=new TextureFormat(format);
			newFormat->setPixelFormat(newPixelFormat);
			if(needsNPOT){
				int newWidth=Math::nextPowerOf2(format->getWidth()),newHeight=Math::nextPowerOf2(format->getHeight()),newDepth=Math::nextPowerOf2(format->getDepth());
				newFormat->setSize(newWidth,newHeight,newDepth);
			}
			tbyte **newMipDatas=new tbyte*[allocatedMipLevels];

			for(i=0;i<allocatedMipLevels;++i){
				TextureFormat::ptr mipFormat=new TextureFormat(format,i);
				TextureFormat::ptr newMipFormat=new TextureFormat(newFormat,i);
				newMipDatas[i]=new tbyte[newMipFormat->getDataSize()];
		
				TextureFormatConversion::convert(mipDatas[i],mipFormat,newMipDatas[i],newMipFormat);
			}

			if(needsAutogen){
				usage&=~Usage_BIT_AUTOGEN_MIPMAPS;
			}

			result=texture->create(usage,newFormat,newMipDatas);

			for(i=0;i<specifiedMipLevels;++i){
				delete[] newMipDatas[i];
			}
			delete[] newMipDatas;
		}
	}

	return result;
}

}
}
