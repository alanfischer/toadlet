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
#include <toadlet/peeper/CapabilityState.h>

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

	// BackableTextures only store the first mipLevel, so if it's not a staging resource, then we need to autogen mipmaps
	if((usage&Usage_BIT_STAGING)==0){
		mUsage|=Usage_BIT_AUTOGEN_MIPMAPS;
	}

	// BackableTextures only keep a Data allocation if its not a Dyanmic or Streamed resource
	if((usage&(Usage_BIT_STREAM|Usage_BIT_DYNAMIC|Usage_BIT_RENDERTARGET))==0){
		mData=new tbyte[mDataSize];
	}

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
	bool result=false;

	if(mBack!=NULL){
		result=convertLoad(mBack,mFormat,width,height,depth,mipLevel,mipData);
	}
	else if(mipLevel==0 && mData!=NULL){
		ImageFormatConversion::convert(mipData,mFormat,mRowPitch,mSlicePitch,mData,mFormat,mRowPitch,mSlicePitch,width,height,depth);
		result=true;
	}

	return result;
}

bool BackableTexture::read(int width,int height,int depth,int mipLevel,tbyte *mipData){
	bool result=false;

	if(mBack!=NULL){
		result=convertRead(mBack,mFormat,width,height,depth,mipLevel,mipData);
	}
	else if(mipLevel==0 && mData!=NULL){
		ImageFormatConversion::convert(mData,mFormat,mRowPitch,mSlicePitch,mipData,mFormat,mRowPitch,mSlicePitch,width,height,depth);
		result=true;
	}

	return result;
}

void BackableTexture::setBack(Texture::ptr back,Renderer *renderer){
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
		// Create texture on setting the back, otherwise D3D10 static textures will not load data in load
		convertCreate(mBack,renderer,mUsage,mDimension,mFormat,mWidth,mHeight,mDepth,1,(mData!=NULL?&mData:NULL));

		for(i=0;i<mBuffers.size();++i){
			if(mBuffers[i]!=NULL){
				mBuffers[i]->resetCreate();
			}
		}
	}
}

bool BackableTexture::convertLoad(Texture::ptr texture,int format,int width,int height,int depth,int mipLevel,tbyte *mipData){
	if(texture->getFormat()==format){
		return texture->load(width,height,depth,mipLevel,mipData);
	}
	else{
		int pitch=ImageFormatConversion::getRowPitch(format,width);
		int textureFormat=texture->getFormat();
		int texturePitch=ImageFormatConversion::getRowPitch(textureFormat,width);
		tbyte *textureData=new tbyte[texturePitch * height];
		ImageFormatConversion::convert(mipData,format,pitch,pitch*height,textureData,textureFormat,texturePitch,texturePitch*height,width,height,depth);
		bool result=texture->load(width,height,depth,mipLevel,textureData);
		delete[] textureData;
		return result;
	}
}

bool BackableTexture::convertRead(Texture::ptr texture,int format,int width,int height,int depth,int mipLevel,tbyte *mipData){
	if(texture->getFormat()==format){
		return texture->read(width,height,depth,mipLevel,mipData);
	}
	else{
		int pitch=ImageFormatConversion::getRowPitch(format,width);
		int textureFormat=texture->getFormat();
		int texturePitch=ImageFormatConversion::getRowPitch(textureFormat,width);
		tbyte *textureData=new tbyte[texturePitch * height];
		bool result=texture->read(width,height,depth,mipLevel,textureData);
		ImageFormatConversion::convert(textureData,textureFormat,texturePitch,texturePitch*height,mipData,format,pitch,pitch*height,width,height,depth);
		delete[] textureData;
		return result;
	}
}

bool BackableTexture::convertCreate(Texture::ptr texture,Renderer *renderer,int usage,Dimension dimension,int format,int width,int height,int depth,int mipLevels,tbyte *mipDatas[]){
	int closeFormat=renderer->getCloseTextureFormat(format,usage);
	bool hasNPOT=renderer->getCapabilityState().textureNonPowerOf2;
	bool wantsNPOT=(Math::isPowerOf2(width)==false || Math::isPowerOf2(height)==false || (dimension!=Image::Dimension_CUBE && Math::isPowerOf2(depth)==false));
	bool hasAutogen=renderer->getCapabilityState().textureAutogenMipMaps;
	bool wantsAutogen=(usage&Texture::Usage_BIT_AUTOGEN_MIPMAPS)>0;

	bool result=false;
	if(mipDatas==NULL){
		result=texture->create(usage,dimension,closeFormat,width,height,depth,mipLevels,NULL);
	}
	else{
		int pitch=ImageFormatConversion::getRowPitch(format,width);
		int closePitch=ImageFormatConversion::getRowPitch(closeFormat,width);
		tbyte *closeData=new tbyte[closePitch * height];
		ImageFormatConversion::convert(mipDatas[0],format,pitch,pitch*height,closeData,closeFormat,closePitch,closePitch*height,width,height,depth);

/// Till we get this finished
usage|=Usage_BIT_AUTOGEN_MIPMAPS;
//		if((hasAutogen==false && wantsAutogen==true) || (hasNPOT==false && wantsNPOT==true)){
//		}

		result=texture->create(usage,dimension,closeFormat,width,height,depth,0,&closeData);
		delete[] closeData;
	}

	return result;
}
/*(
	Image::ptr image;
	if((hasAutogen==false || hasNonPowerOf2==false) && (Math::isPowerOf2(width)==false || Math::isPowerOf2(height)==false || (dimension!=Image::Dimension_CUBE && Math::isPowerOf2(depth)==false))){
		Logger::debug(Categories::TOADLET_TADPOLE,"making image power of 2");

		int dwidth=width>1?(Math::nextPowerOf2(width)>>1):1;
		int dheight=height>1?(Math::nextPowerOf2(height)>>1):1;
		int ddepth=depth>1?(Math::nextPowerOf2(depth)>>1):1;

		image=Image::ptr(Image::createAndReallocate(dimension,format,dwidth,dheight,ddepth));
		if(image==NULL){
			return false;
		}

		ImageFormatConversion::scale(
		ImageFormatConversion::convert(mData,mFormat,mRowPitch,mSlicePitch,backData,backFormat,backPitch,backPitch * mHeight,mWidth,mHeight,mDepth);
		Pixel<uint8> pixel;
		int x,y,z;
		for(z=0;z<ddepth;++z){
			for(y=0;y<dheight;++y){
				for(x=0;x<dwidth;++x){
					image->getPixel(pixel,x*width/dwidth,y*height/dheight,z*depth/ddepth);
					finalImage->setPixel(pixel,x,y,z);
				}
			}
		}

		width=dwidth;
		height=dheight;
		depth=ddepth;
	}

	Image::ptr finalImage=image;

	if(format!=closestFormat){
if(closestFormat==0)closestFormat=Texture::Format_RGBA_8;
		Logger::debug(Categories::TOADLET_TADPOLE,String("converting image from:")+format+" to:"+closestFormat);

		Image::ptr convertedImage(Image::createAndReallocate(dimension,closestFormat,width,height,depth));
		if(convertedImage==NULL){
			return NULL;
		}

		ImageFormatConversion::convert(
			finalImage->getData(),finalImage->getFormat(),finalImage->getRowPitch(),finalImage->getSlicePitch(),
			convertedImage->getData(),convertedImage->getFormat(),convertedImage->getRowPitch(),convertedImage->getSlicePitch(),width,height,depth);
		finalImage=convertedImage;
		format=closestFormat;
	}

	egg::Collection<Image::ptr> mipImages;
	egg::Collection<tbyte*> mipDatas;

	mipImages.add(finalImage);
	mipDatas.add(finalImage->getData());

	if(hasAutogen==false && wantsAutogen==true){
		Logger::debug(Categories::TOADLET_TADPOLE,"simulating mipmap generation");

		if(mipLevels==0){
			int hwidth=width,hheight=height,hdepth=depth;
			while(hwidth>1 || hheight>1 || hdepth>1){
				mipLevels++;
				hwidth/=2;hheight/=2;hdepth/=2;
				hwidth=hwidth>0?hwidth:1;hheight=hheight>0?hheight:1;hdepth=hdepth>0?hdepth:1;
				int xoff=width/(hwidth+1),yoff=height/(hheight+1),zoff=depth/(hdepth+1);

				Image::ptr mipImage(Image::createAndReallocate(dimension,format,hwidth,hheight,hdepth));
				if(mipImage==NULL){
					return NULL;
				}

				Pixel<uint8> pixel;
				int x,y,z;
				for(z=0;z<hdepth;++z){
					for(y=0;y<hheight;++y){
						for(x=0;x<hwidth;++x){
							finalImage->getPixel(pixel,xoff+x*(1<<mipLevels),yoff+y*(1<<mipLevels),zoff+z*(1<<mipLevels));
							mipImage->setPixel(pixel,x,y,z);
						}
					}
				}

				mipImages.add(mipImage);
				mipDatas.add(mipImage->getData());
			}
		}
	}
*/

}
}
