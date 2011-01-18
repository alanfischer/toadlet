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
#include <toadlet/peeper/CapabilitySet.h>
#include <toadlet/tadpole/TextureManager.h>
#include <toadlet/tadpole/Engine.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::image;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{

TextureManager::TextureManager(Engine *engine,bool backable):ResourceManager(engine->getArchiveManager()),
	mEngine(NULL),
	mBackable(false)
{
	mEngine=engine;
	mBackable=backable;
}

Texture::ptr TextureManager::createTexture(Image::ptr image,int usage,int mipLevels){
	Logger::debug(Categories::TOADLET_TADPOLE,"TextureManager::createTexture");

	Renderer *renderer=getRenderer();

	Image::Dimension dimension=image->getDimension();
	int format=image->getFormat();
	int width=image->getWidth(),height=image->getHeight(),depth=image->getDepth();
	int closestFormat=renderer==NULL?format:renderer->getClosestTextureFormat(format);
/// @todo: I need to remove this.  It is here so that all Textures get a unified format
///  When you use a BackableTexture, if the new texture can't handle the old format, it
///  will either error if strictFormats is on, or it my crash when loading the data into d3d10
///  This could be solved by doing a conversion when the convertedFormat != format on the data before
///  loading, but would require some allocations.  Perhaps the key is to put the conversion into
///  BackableTexture, so when setting the back, it will create, then check the format to see if it needs to
///  convert!  That should be pretty easy to, and keep all that possibly time consuming stuff in BackableTexture where it belongs.
closestFormat=Image::Format_RGBA_8;

	// Check if the renderer supports npot textures
	bool hasNonPowerOf2=renderer==NULL?false:renderer->getCapabilitySet().textureNonPowerOf2;
	// Check if the renderer can autogenerate mipmaps
	bool hasAutogen=renderer==NULL?false:renderer->getCapabilitySet().textureAutogenMipMaps;
	// See if we want autogenerate, and only do so if we have a renderer
	bool wantsAutogen=renderer==NULL?false:(usage&Texture::Usage_BIT_AUTOGEN_MIPMAPS)>0;
//#error our d3d10 problem is really 2fold, and somewhat a d3d9 problem too
//#error If the BackableTexture is loading the data, it doesn't have the ability to convert formats, and generate mipMaps like we do here.
//#error  We really need to somehow tie the BackableTexture loading in with this conversion.
//#error  Perhaps we can move the convert/mip functions out, and then when setting the backing texture, just grab the previous data from the back and run it through those functions
if(mipLevels==0){mipLevels=1;}

	Image::ptr finalImage=image;
	if((hasAutogen==false || hasNonPowerOf2==false) && (Math::isPowerOf2(width)==false || Math::isPowerOf2(height)==false || (dimension!=Image::Dimension_CUBE && Math::isPowerOf2(depth)==false))){
		Logger::debug(Categories::TOADLET_TADPOLE,"making image power of 2");

		int dwidth=width>1?(Math::nextPowerOf2(width)>>1):1;
		int dheight=height>1?(Math::nextPowerOf2(height)>>1):1;
		int ddepth=depth>1?(Math::nextPowerOf2(depth)>>1):1;

		finalImage=Image::ptr(Image::createAndReallocate(dimension,format,dwidth,dheight,ddepth));
		if(finalImage==NULL){
			return NULL;
		}

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

	if(format!=closestFormat){
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
		usage&=~Texture::Usage_BIT_AUTOGEN_MIPMAPS;

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

	Texture::ptr texture;
	if(mBackable || renderer==NULL){
		Logger::debug(Categories::TOADLET_TADPOLE,"creating BackableTexture");

		BackableTexture::ptr backableTexture(new BackableTexture());
		backableTexture->create(usage,dimension,format,width,height,depth,mipLevels,mipDatas);
		if(mEngine->getRenderer()!=NULL){
			Texture::ptr back(renderer->createTexture());
			backableTexture->setBack(back);
		}
		texture=backableTexture;
	}
	else{
		Logger::debug(Categories::TOADLET_TADPOLE,"creating Texture");

		texture=Texture::ptr(renderer->createTexture());
		if(texture->create(usage,dimension,format,width,height,depth,mipLevels,mipDatas)==false){
			return NULL;
		}
	}

	manage(shared_static_cast<Texture>(texture));

	Logger::debug(Categories::TOADLET_TADPOLE,"TextureManager::createTexture finished");

	return texture;
}

Texture::ptr TextureManager::createTexture(Image::ptr images[],int mipLevels,int usage){
	if(images==NULL || mipLevels==0){
		Error::nullPointer("createTexture called without images or mipLevels");
		return NULL;
	}

	Renderer *renderer=getRenderer();

	Image::Dimension dimension=images[0]->getDimension();
	int format=images[0]->getFormat();
	int width=images[0]->getWidth(),height=images[0]->getHeight(),depth=images[0]->getDepth();
	int closestFormat=renderer==NULL?format:renderer->getClosestTextureFormat(format);
/// @todo: REMOVEME
closestFormat=Image::Format_RGBA_8;

	egg::Collection<Image::ptr> mipImages;
	egg::Collection<tbyte*> mipDatas;

	if(format!=closestFormat){
		Logger::debug(Categories::TOADLET_TADPOLE,String("converting image from:")+format+" to:"+closestFormat);

		int i;
		for(i=0;i<mipLevels;++i){
			Image::ptr convertedImage(Image::createAndReallocate(dimension,closestFormat,images[i]->getWidth(),images[i]->getHeight(),images[i]->getDepth()));
			if(convertedImage==NULL){
				return NULL;
			}
			
			ImageFormatConversion::convert(
				images[i]->getData(),images[i]->getFormat(),images[i]->getRowPitch(),images[i]->getSlicePitch(),
				convertedImage->getData(),convertedImage->getFormat(),convertedImage->getRowPitch(),convertedImage->getSlicePitch(),convertedImage->getWidth(),convertedImage->getHeight(),convertedImage->getDepth());

			mipImages.add(convertedImage);
			mipDatas.add(convertedImage->getData());

			format=closestFormat;
		}
	}
	else{
		int i;
		for(i=0;i<mipLevels;++i){
			mipImages.add(images[i]);
			mipDatas.add(images[i]->getData());
		}
	}

	Texture::ptr texture;
	if(mBackable || mEngine->getRenderer()==NULL){
		BackableTexture::ptr backableTexture(new BackableTexture());
		backableTexture->create(usage,dimension,format,width,height,depth,mipLevels,mipDatas);
		if(mEngine->getRenderer()!=NULL){
			Texture::ptr back(mEngine->getRenderer()->createTexture());
			backableTexture->setBack(back);
		}
		texture=backableTexture;
	}
	else{
		texture=Texture::ptr(mEngine->getRenderer()->createTexture());
		if(texture->create(usage,dimension,format,width,height,depth,mipLevels,mipDatas)==false){
			return NULL;
		}
	}

	manage(shared_static_cast<Texture>(texture));

	return texture;
}

Texture::ptr TextureManager::createTexture(int usage,Texture::Dimension dimension,int format,int width,int height,int depth,int mipLevels){
	Texture::ptr texture;
	if(mBackable || mEngine->getRenderer()==NULL){
		BackableTexture::ptr backableTexture(new BackableTexture());
		backableTexture->create(usage,dimension,format,width,height,depth,mipLevels,NULL);
		if(mEngine->getRenderer()!=NULL){
			Texture::ptr back(mEngine->getRenderer()->createTexture());
			backableTexture->setBack(back);
		}
		texture=backableTexture;
	}
	else{
		texture=Texture::ptr(mEngine->getRenderer()->createTexture());
		if(texture->create(usage,dimension,format,width,height,depth,mipLevels,NULL)==false){
			return NULL;
		}
	}

	manage(shared_static_cast<Texture>(texture));

	return texture;
}

Image::ptr TextureManager::createImage(Texture *texture){
	Image::ptr image(Image::createAndReallocate(texture->getDimension(),texture->getFormat(),texture->getWidth(),texture->getHeight(),texture->getDepth()));
	if(image==NULL){
		return NULL;
	}
	
	texture->read(image->getWidth(),image->getHeight(),image->getDepth(),0,image->getData());
	return image;
}

PixelBufferRenderTarget::ptr TextureManager::createPixelBufferRenderTarget(){
	/// @todo: Make this use a BackableSufaceRenderTarget
	if(mEngine->getRenderer()!=NULL){
		PixelBufferRenderTarget::ptr back(mEngine->getRenderer()->createPixelBufferRenderTarget());
		if(back!=NULL){
			back->create();
		}
		return back;
	}
	return NULL;
}

void TextureManager::contextActivate(peeper::Renderer *renderer){
	int i;
	for(i=0;i<mResources.size();++i){
		Texture::ptr texture=shared_static_cast<Texture>(mResources[i]);
		if(texture!=NULL && texture->getRootTexture(0)!=texture){
			Texture::ptr back(renderer->createTexture());
			shared_static_cast<BackableTexture>(texture)->setBack(back);
		}
	}
}

void TextureManager::contextDeactivate(peeper::Renderer *renderer){
	int i;
	for(i=0;i<mResources.size();++i){
		Texture::ptr texture=shared_static_cast<Texture>(mResources[i]);
		if(texture!=NULL && texture->getRootTexture(0)!=texture){
			shared_static_cast<BackableTexture>(texture)->setBack(NULL);
		}
	}
}

void TextureManager::preContextReset(peeper::Renderer *renderer){
	Logger::debug("TextureManager::preContextReset");

	int i;
	for(i=0;i<mResources.size();++i){
		Texture::ptr texture=shared_static_cast<Texture>(mResources[i]);
		if(texture!=NULL){
			texture->resetDestroy();
		}
	}
}

void TextureManager::postContextReset(peeper::Renderer *renderer){
	Logger::debug("TextureManager::postContextReset");

	int i;
	for(i=0;i<mResources.size();++i){
		Texture::ptr texture=shared_static_cast<Texture>(mResources[i]);
		if(texture!=NULL){
			texture->resetCreate();
		}
	}
}

Renderer *TextureManager::getRenderer(){return mEngine->getRenderer();}

Texture::ptr TextureManager::createNormalization(int size){
	Image::ptr image(Image::createAndReallocate(Image::Dimension_CUBE,Image::Format_RGB_8,size,size,Image::CubeSide_MAX));

	Vector3 HALF_VECTOR3(Math::HALF,Math::HALF,Math::HALF);

	uint8 *data=image->getData();

	float offset = 0.5f;
	float halfSize = size * 0.5f;
	Vector3 temp;
	int pos=0;

	int i,j;
	for(j=0;j<size;++j){
		for(i=0;i<size;++i){
			temp.set(halfSize,(j+offset-halfSize),-(i+offset-halfSize));
			Math::normalize(temp);
			Math::madd(temp,Math::HALF,HALF_VECTOR3);
			data[pos+0]=(tbyte)(temp.x*255);
			data[pos+1]=(tbyte)(temp.y*255);
			data[pos+2]=(tbyte)(temp.z*255);

			pos+=3;
		}
	}

	for(j=0;j<size;++j){
		for(i=0;i<size;++i){
			temp.set(-halfSize,(j+offset-halfSize),(i+offset-halfSize));
			Math::normalize(temp);
			Math::madd(temp,Math::HALF,HALF_VECTOR3);

			data[pos+0]=(tbyte)(temp.x*255);
			data[pos+1]=(tbyte)(temp.y*255);
			data[pos+2]=(tbyte)(temp.z*255);

			pos+=3;
		}
	}

	for(j=0;j<size;++j){
		for(i=0;i<size;++i){
			temp.set((i+offset-halfSize),-halfSize,(j+offset-halfSize));
			Math::normalize(temp);
			Math::madd(temp,Math::HALF,HALF_VECTOR3);

			data[pos+0]=(tbyte)(temp.x*255);
			data[pos+1]=(tbyte)(temp.y*255);
			data[pos+2]=(tbyte)(temp.z*255);

			pos+=3;
		}
	}

	for(j=0;j<size;++j){
		for(i=0;i<size;++i){
			temp.set((i+offset-halfSize),halfSize,-(j+offset-halfSize));
			Math::normalize(temp);
			Math::madd(temp,Math::HALF,HALF_VECTOR3);

			data[pos+0]=(tbyte)(temp.x*255);
			data[pos+1]=(tbyte)(temp.y*255);
			data[pos+2]=(tbyte)(temp.z*255);

			pos+=3;
		}
	}

	for(j=0;j<size;++j){
		for(i=0;i<size;++i){
			temp.set((i+offset-halfSize),(j+offset-halfSize),halfSize);
			Math::normalize(temp);
			Math::madd(temp,Math::HALF,HALF_VECTOR3);

			data[pos+0]=(tbyte)(temp.x*255);
			data[pos+1]=(tbyte)(temp.y*255);
			data[pos+2]=(tbyte)(temp.z*255);

			pos+=3;
		}
	}

	for(j=0;j<size;++j){
		for(i=0;i<size;++i){
			temp.set(-(i+offset-halfSize),(j+offset-halfSize),-halfSize);
			Math::normalize(temp);
			Math::madd(temp,Math::HALF,HALF_VECTOR3);

			data[pos+0]=(tbyte)(temp.x*255);
			data[pos+1]=(tbyte)(temp.y*255);
			data[pos+2]=(tbyte)(temp.z*255);

			pos+=3;
		}
	}

	return createTexture(image,Texture::Usage_BIT_STATIC,1);
}

}
}

