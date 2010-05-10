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
	Renderer *renderer=getRenderer();

	Image::Dimension dimension=image->getDimension();
	int format=image->getFormat();
	int width=image->getWidth(),height=image->getHeight(),depth=image->getDepth();
	int closestFormat=renderer==NULL?format:renderer->getClosestTextureFormat(format);

	// Check if the renderer supports npot textures
	bool hasNonPowerOf2=renderer==NULL?false:renderer->getCapabilitySet().textureNonPowerOf2;
	// Check if the renderer can autogenerate mipmaps
	bool hasAutogen=renderer==NULL?false:renderer->getCapabilitySet().textureAutogenMipMaps;
	// See if we want autogenerate, and only do so if we have a renderer
	bool wantsAutogen=renderer==NULL?false:(usage&Texture::Usage_BIT_AUTOGEN_MIPMAPS)>0;

	Image::ptr finalImage=image;
	if((hasAutogen==false || hasNonPowerOf2==false) && (Math::isPowerOf2(width)==false || Math::isPowerOf2(height)==false || Math::isPowerOf2(depth)==false)){
		int dwidth=width>1?(Math::nextPowerOf2(width)>>1):1;
		int dheight=height>1?(Math::nextPowerOf2(height)>>1):1;
		int ddepth=depth>1?(Math::nextPowerOf2(depth)>>1):1;

		finalImage=Image::ptr(new Image(dimension,format,dwidth,dheight,ddepth));

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
		Image::ptr convertedImage=Image::ptr(new Image(dimension,closestFormat,width,height,depth));
		ImageFormatConversion::convert(
			finalImage->getData(),finalImage->getFormat(),finalImage->getRowPitch(),finalImage->getSlicePitch(),
			convertedImage->getData(),convertedImage->getFormat(),convertedImage->getRowPitch(),convertedImage->getSlicePitch(),width,height,depth);
		finalImage=convertedImage;
		format=closestFormat;
	}

	egg::Collection<Image::ptr> mipImages;
	egg::Collection<byte*> mipDatas;

	mipImages.add(finalImage);
	mipDatas.add(finalImage->getData());

	if(hasAutogen==false && wantsAutogen==true){
		usage&=~Texture::Usage_BIT_AUTOGEN_MIPMAPS;

		if(mipLevels==0){
			int hwidth=width,hheight=height,hdepth=depth;
			while(hwidth>1 || hheight>1 || hdepth>1){
				mipLevels++;
				hwidth/=2;hheight/=2;hdepth/=2;
				hwidth=hwidth>0?hwidth:1;hheight=hheight>0?hheight:1;hdepth=hdepth>0?hdepth:1;
				int xoff=width/(hwidth+1),yoff=height/(hheight+1),zoff=depth/(hdepth+1);

				Image::ptr mipImage(new Image(dimension,format,hwidth,hheight,hdepth));

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
		BackableTexture::ptr backableTexture(new BackableTexture());
		backableTexture->create(usage,dimension,format,width,height,depth,mipLevels,mipDatas.begin());
		if(mEngine->getRenderer()!=NULL){
			Texture::ptr back(renderer->createTexture());
			backableTexture->setBack(back);
		}
		texture=backableTexture;
	}
	else{
		texture=Texture::ptr(renderer->createTexture());
		texture->create(usage,dimension,format,width,height,depth,mipLevels,mipDatas.begin());
	}

	manage(shared_static_cast<Texture>(texture));

	return texture;
}

Texture::ptr TextureManager::createTexture(Image::ptr images[],int usage,int mipLevels){
	if(images==NULL || mipLevels==0){
		Error::nullPointer("createTexture called without images or mipLevels");
		return NULL;
	}

	Renderer *renderer=getRenderer();

	Image::Dimension dimension=images[0]->getDimension();
	int format=images[0]->getFormat();
	int width=images[0]->getWidth(),height=images[0]->getHeight(),depth=images[0]->getDepth();
	int closestFormat=renderer==NULL?format:renderer->getClosestTextureFormat(format);

	egg::Collection<Image::ptr> mipImages;
	egg::Collection<byte*> mipDatas;

	if(format!=closestFormat){
		int i;
		for(i=0;i<mipLevels;++i){
			Image::ptr convertedImage=Image::ptr(new Image(dimension,closestFormat,images[i]->getWidth(),images[i]->getHeight(),images[i]->getDepth()));
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
		backableTexture->create(usage,dimension,format,width,height,depth,mipLevels,mipDatas.begin());
		if(mEngine->getRenderer()!=NULL){
			Texture::ptr back(mEngine->getRenderer()->createTexture());
			backableTexture->setBack(back);
		}
		texture=backableTexture;
	}
	else{
		texture=Texture::ptr(mEngine->getRenderer()->createTexture());
		texture->create(usage,dimension,format,width,height,depth,mipLevels,mipDatas.begin());
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
		texture->create(usage,dimension,format,width,height,depth,mipLevels,NULL);
	}

	manage(shared_static_cast<Texture>(texture));

	return texture;
}

Image::ptr TextureManager::createImage(Texture *texture){
	Image::ptr image(new Image(texture->getDimension(),texture->getFormat(),texture->getWidth(),texture->getHeight(),texture->getDepth()));
	texture->read(image->getWidth(),image->getHeight(),image->getDepth(),0,image->getData());
	return image;
}

SurfaceRenderTarget::ptr TextureManager::createSurfaceRenderTarget(){
	/// @todo: Make this use a BackableSufaceRenderTarget
	if(mEngine->getRenderer()!=NULL){
		SurfaceRenderTarget::ptr back(mEngine->getRenderer()->createSurfaceRenderTarget());
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
		if(texture->getRootTexture(0)!=texture){
			Texture::ptr back(renderer->createTexture());
			shared_static_cast<BackableTexture>(texture)->setBack(back);
		}
	}
}

void TextureManager::contextDeactivate(peeper::Renderer *renderer){
	int i;
	for(i=0;i<mResources.size();++i){
		Texture::ptr texture=shared_static_cast<Texture>(mResources[i]);
		if(texture->getRootTexture(0)!=texture){
			shared_static_cast<BackableTexture>(texture)->setBack(NULL);
		}
	}
}

void TextureManager::preContextReset(peeper::Renderer *renderer){
	int i;
	for(i=0;i<mResources.size();++i){
		Texture::ptr texture=shared_static_cast<Texture>(mResources[i]);
		texture->resetDestroy();
	}
}

void TextureManager::postContextReset(peeper::Renderer *renderer){
	int i;
	for(i=0;i<mResources.size();++i){
		Texture::ptr texture=shared_static_cast<Texture>(mResources[i]);
		texture->resetCreate();
	}
}

Renderer *TextureManager::getRenderer(){return mEngine->getRenderer();}

}
}

