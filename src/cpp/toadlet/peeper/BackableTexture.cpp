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
#include <toadlet/egg/image/ImageFormatConversion.h>
#include <toadlet/egg/image/Image.h>
#include <toadlet/peeper/BackableTexture.h>
#include <toadlet/peeper/BackableTextureMipPixelBuffer.h>
#include <toadlet/peeper/RenderCaps.h>

namespace toadlet{
namespace peeper{

BackableTexture::BackableTexture():BaseResource(),
	mUsage(0),
	//mFormat,
	
	mRowPitch(0),
	mSlicePitch(0),
	mDataSize(0),
	mData(NULL)
	//mBack
{}

BackableTexture::~BackableTexture(){
	destroy();
}

bool BackableTexture::create(int usage,TextureFormat::ptr format,tbyte *mipDatas[]){
	mUsage=usage;
	mFormat=TextureFormat::ptr(new TextureFormat(format));

	mRowPitch=ImageFormatConversion::getPixelSize(mFormat->pixelFormat)*mFormat->width;
	mSlicePitch=mRowPitch*mFormat->height;
	switch(mFormat->dimension){
		case TextureFormat::Dimension_D1:
			mDataSize=mRowPitch;
		break;
		case TextureFormat::Dimension_D2:
			mDataSize=mSlicePitch;
		break;
		case TextureFormat::Dimension_D3:
		case TextureFormat::Dimension_CUBE:
			mDataSize=mSlicePitch*mFormat->depth;
		break;
	}

	// BackableTextures only store the first mipLevel, so if it's not a staging resource and has multiple mip levels, then we need to autogen mipmaps
	if((usage&Usage_BIT_STAGING)==0 && format->mipLevels!=1){
		mUsage|=Usage_BIT_AUTOGEN_MIPMAPS;
	}

	// BackableTextures only keep a Data allocation if its not a Dyanmic or Streamed resource
	if((usage&(Usage_BIT_STREAM|Usage_BIT_DYNAMIC|Usage_BIT_RENDERTARGET))==0){
		mData=new tbyte[mDataSize];
	}

	if(mipDatas!=NULL){
		load(format->width,format->height,format->depth,format->mipLevels==1?1:0,mipDatas[0]);
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
	if(mFormat->dimension==TextureFormat::Dimension_CUBE){
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
		result=convertLoad(mBack,mFormat->pixelFormat,width,height,depth,mipLevel,mipData);
	}
	else if(mipLevel==0 && mData!=NULL){
		ImageFormatConversion::convert(mipData,mFormat->pixelFormat,mRowPitch,mSlicePitch,mData,mFormat->pixelFormat,mRowPitch,mSlicePitch,width,height,depth);
		result=true;
	}

	return result;
}

bool BackableTexture::read(int width,int height,int depth,int mipLevel,tbyte *mipData){
	bool result=false;

	if(mBack!=NULL){
		result=convertRead(mBack,mFormat->pixelFormat,width,height,depth,mipLevel,mipData);
	}
	else if(mipLevel==0 && mData!=NULL){
		ImageFormatConversion::convert(mData,mFormat->pixelFormat,mRowPitch,mSlicePitch,mipData,mFormat->pixelFormat,mRowPitch,mSlicePitch,width,height,depth);
		result=true;
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
		TextureFormat::ptr format(new TextureFormat(mFormat));
		format->mipLevels=(mUsage&Usage_BIT_AUTOGEN_MIPMAPS)!=0?0:1;
		convertCreate(mBack,renderDevice,mUsage,format,(mData!=NULL?&mData:NULL));

		for(i=0;i<mBuffers.size();++i){
			if(mBuffers[i]!=NULL){
				mBuffers[i]->resetCreate();
			}
		}
	}
}

bool BackableTexture::convertLoad(Texture::ptr texture,int pixelFormat,int width,int height,int depth,int mipLevel,tbyte *mipData){
	if(texture->getFormat()->pixelFormat==pixelFormat){
		return texture->load(width,height,depth,mipLevel,mipData);
	}
	else{
		int pitch=ImageFormatConversion::getRowPitch(pixelFormat,width);
		int textureFormat=texture->getFormat()->pixelFormat;
		int texturePitch=ImageFormatConversion::getRowPitch(textureFormat,width);
		tbyte *textureData=new tbyte[texturePitch * height];
		ImageFormatConversion::convert(mipData,pixelFormat,pitch,pitch*height,textureData,textureFormat,texturePitch,texturePitch*height,width,height,depth);
		bool result=texture->load(width,height,depth,mipLevel,textureData);
		delete[] textureData;
		return result;
	}
}

bool BackableTexture::convertRead(Texture::ptr texture,int pixelFormat,int width,int height,int depth,int mipLevel,tbyte *mipData){
	if(texture->getFormat()->pixelFormat==pixelFormat){
		return texture->read(width,height,depth,mipLevel,mipData);
	}
	else{
		int pitch=ImageFormatConversion::getRowPitch(pixelFormat,width);
		int textureFormat=texture->getFormat()->pixelFormat;
		int texturePitch=ImageFormatConversion::getRowPitch(textureFormat,width);
		tbyte *textureData=new tbyte[texturePitch * height];
		bool result=texture->read(width,height,depth,mipLevel,textureData);
		ImageFormatConversion::convert(textureData,textureFormat,texturePitch,texturePitch*height,mipData,pixelFormat,pitch,pitch*height,width,height,depth);
		delete[] textureData;
		return result;
	}
}

bool BackableTexture::convertCreate(Texture::ptr texture,RenderDevice *renderDevice,int usage,TextureFormat::ptr format,tbyte *mipDatas[]){
	RenderCaps caps;
	renderDevice->getRenderCaps(caps);
	int newPixelFormat=renderDevice->getClosePixelFormat(format->pixelFormat,usage);
	bool hasNPOT=caps.textureNonPowerOf2;
	bool wantsNPOT=(Math::isPowerOf2(format->width)==false || Math::isPowerOf2(format->height)==false || (format->dimension!=TextureFormat::Dimension_CUBE && Math::isPowerOf2(format->depth)==false));
	bool needsNPOT=wantsNPOT & !hasNPOT;
	bool hasAutogen=caps.textureAutogenMipMaps;
	bool wantsAutogen=(usage&Texture::Usage_BIT_AUTOGEN_MIPMAPS)>0;
	bool needsAutogen=wantsAutogen & !hasAutogen;
	bool needsConvert=(newPixelFormat!=format->pixelFormat);

	bool result=false;
	if(mipDatas==NULL){
		TextureFormat::ptr newFormat(new TextureFormat(format));
		newFormat->pixelFormat=newPixelFormat;
		result=texture->create(usage,newFormat,NULL);
	}
	else{
		int totalMipLevels=Math::intLog2(Math::maxVal(format->width,Math::maxVal(format->height,format->depth)));
		int specifiedMipLevels=0;
		int allocatedMipLevels=0;
		if(needsAutogen || format->mipLevels>0){
			if(format->mipLevels>0){
				specifiedMipLevels=format->mipLevels;
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
			Logger::alert(Categories::TOADLET_PEEPER,String("BackableTexture::convertCreate - converting texture:")+needsNPOT+","+needsAutogen+","+needsConvert);

			tbyte **newMipDatas=new tbyte*[allocatedMipLevels];
			int newWidth=Math::nextPowerOf2(format->width),newHeight=Math::nextPowerOf2(format->height),newDepth=Math::nextPowerOf2(format->depth);
			int newMipWidth=newWidth,newMipHeight=newHeight,newMipDepth=newDepth;
			int mipWidth=format->width,mipHeight=format->height,mipDepth=format->depth;

			for(i=0;i<allocatedMipLevels;++i){
				int newPitch=ImageFormatConversion::getRowPitch(newPixelFormat,newMipWidth);
				newMipDatas[i]=new tbyte[newPitch*newMipHeight*newMipDepth];

				if((format->mipLevels==0 && i==0) || i<format->mipLevels){
					convertAndScale(mipDatas[i],format->pixelFormat,mipWidth,mipHeight,mipDepth,newMipDatas[i],newPixelFormat,newMipWidth,newMipHeight,newMipDepth);
				}
				else{
					convertAndScale(mipDatas[0],format->pixelFormat,format->width,format->height,format->depth,newMipDatas[i],newPixelFormat,newMipWidth,newMipHeight,newMipDepth);
				}

				mipWidth=mipWidth>1?mipWidth/2:1;mipHeight=mipHeight>1?mipHeight/2:1;mipDepth=mipDepth>1?mipDepth/2:1;
				newMipWidth=newMipWidth>1?newMipWidth/2:1;newMipHeight=newMipHeight>1?newMipHeight/2:1;newMipDepth=newMipDepth>1?newMipDepth/2:1;
			}

			if(needsAutogen){
				usage&=~Usage_BIT_AUTOGEN_MIPMAPS;
			}

			TextureFormat::ptr newFormat(new TextureFormat(format->dimension,newPixelFormat,newWidth,newHeight,newDepth,specifiedMipLevels));
			result=texture->create(usage,newFormat,newMipDatas);

			for(i=0;i<specifiedMipLevels;++i){
				delete[] newMipDatas[i];
			}
			delete[] newMipDatas;
		}
	}

	return result;
}

/// @todo: Fix the mipmap generation.  It's just nearest neighbor right now.  The format conversion, mipmap generation, and scaling should all be moved into egg::image, and work with raw memory, not Image objects.
void BackableTexture::convertAndScale(tbyte *src,int srcFormat,int srcWidth,int srcHeight,int srcDepth,tbyte *dst,int dstFormat,int dstWidth,int dstHeight,int dstDepth){
	int srcPitch=ImageFormatConversion::getRowPitch(srcFormat,srcWidth);

	if(srcWidth==dstWidth && srcHeight==dstHeight && srcDepth==dstDepth){
		int dstPitch=ImageFormatConversion::getRowPitch(dstFormat,dstWidth);

		ImageFormatConversion::convert(src,srcFormat,srcPitch,srcPitch*srcHeight,dst,dstFormat,dstPitch,dstPitch*dstHeight,srcWidth,srcHeight,srcDepth);
	}
	else{
		int srcPitch=ImageFormatConversion::getRowPitch(srcFormat,srcWidth);
		int srcPitch2=ImageFormatConversion::getRowPitch(dstFormat,srcWidth);

		/// @todo: Merge these methods so we can do the conversion & scaling in 1 pass, and not this unoptimal mess we have now
		Image::ptr converted(Image::createAndReallocate(Image::Dimension_D2,dstFormat,srcWidth,srcHeight,srcDepth));

		ImageFormatConversion::convert(src,srcFormat,srcPitch,srcPitch*srcHeight,converted->getData(),dstFormat,srcPitch2,srcPitch2*srcHeight,srcWidth,srcHeight,srcDepth);

		Image::ptr scaled(Image::createAndReallocate(Image::Dimension_D2,dstFormat,dstWidth,dstHeight,dstDepth));
		
		Pixel<uint8> pixel;
		int x,y,z;
		for(z=0;z<dstDepth;++z){
			for(y=0;y<dstHeight;++y){
				for(x=0;x<dstWidth;++x){
					converted->getPixel(pixel,x*srcWidth/dstWidth + srcWidth/dstWidth/2,y*srcHeight/dstHeight + srcHeight/dstHeight/2,z*srcDepth/dstDepth + srcDepth/dstDepth/2);
					scaled->setPixel(pixel,x,y,z);
				}
			}
		}

		memcpy(dst,scaled->getData(),ImageFormatConversion::getRowPitch(dstFormat,dstWidth)*dstHeight*dstDepth);
	}
}
/*
	int pitch=ImageFormatConversion::getRowPitch(format,width);
	int closePitch=ImageFormatConversion::getRowPitch(closeFormat,width);
	tbyte *closeData=new tbyte[closePitch * height];
	ImageFormatConversion::convert(mipDatas[0],format,pitch,pitch*height,closeData,closeFormat,closePitch,closePitch*height,width,height,depth);

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

	Collection<Image::ptr> mipImages;
	Collection<tbyte*> mipDatas;

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
