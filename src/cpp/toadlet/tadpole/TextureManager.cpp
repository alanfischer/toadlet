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

TextureManager::TextureManager(Engine *engine):ResourceManager(engine->getArchiveManager()),
	mEngine(NULL),
	mBackable(true)
{
	mEngine=engine;
}

Texture::ptr TextureManager::createTexture(const Image::ptr &image,int usageFlags,int mipLevels){
	Image::Dimension dimension=image->getDimension();
	int format=image->getFormat();
	int width=image->getWidth(),height=image->getHeight(),depth=image->getDepth();
	byte *data=image->getData();
	bool hasNonPowerOf2=mEngine->getRenderer()==NULL?false:mEngine->getRenderer()->getCapabilitySet().textureNonPowerOf2;
	bool hasAutogen=mEngine->getRenderer()==NULL?false:mEngine->getRenderer()->getCapabilitySet().textureAutogenMipMaps;
	// Only handle any autogen simulation if we actually have a renderer
	bool wantsAutogen=mEngine->getRenderer()==NULL?false:(usageFlags&Texture::UsageFlags_AUTOGEN_MIPMAPS)>0;
	Image::ptr scaledImage;
	if(hasNonPowerOf2==false && (Math::isPowerOf2(width)==false || Math::isPowerOf2(height)==false) || Math::isPowerOf2(depth)==false){
		int dwidth=width>1?(Math::nextPowerOf2(width)>>1):1;
		int dheight=height>1?(Math::nextPowerOf2(height)>>1):1;
		int ddepth=depth>1?(Math::nextPowerOf2(depth)>>1):1;

		scaledImage=Image::ptr(new Image(dimension,format,dwidth,dheight,ddepth));

		Pixel<uint8> pixel;
		int x,y,z;
		for(z=0;z<ddepth;++z){
			for(y=0;y<dheight;++y){
				for(x=0;x<dwidth;++x){
					image->getPixel(pixel,x*width/dwidth,y*height/dheight,z*depth/ddepth);
					scaledImage->setPixel(pixel,x,y,z);
				}
			}
		}

		width=dwidth;
		height=dheight;
		depth=ddepth;
		data=scaledImage->getData();
	}

	if(hasAutogen==false && wantsAutogen==true){
		usageFlags&=~Texture::UsageFlags_AUTOGEN_MIPMAPS;

		if(mipLevels==0){
			int hwidth=width,hheight=height,hdepth=depth;
			while(hwidth>0 || hheight>0 || hdepth>0){
				mipLevels++;
				hwidth/=2; hheight/=2; hdepth/=2;
			}
		}
	}

	Texture::ptr texture;
	if(mBackable){
		BackableTexture::ptr backableTexture(new BackableTexture());
		backableTexture->create(usageFlags,dimension,format,width,height,depth,mipLevels);
		if(mEngine->getRenderer()!=NULL){
			Texture::ptr back(mEngine->getRenderer()->createTexture());
			back->create(usageFlags,dimension,format,width,height,depth,mipLevels);
			backableTexture->setBack(back,true);
		}
		texture=backableTexture;
	}
	else if(mEngine->getRenderer()!=NULL){
		texture=Texture::ptr(mEngine->getRenderer()->createTexture());
		texture->create(usageFlags,dimension,format,width,height,depth,mipLevels);
	}
	else{
		Error::nullPointer("can not create a non-backable Texture without a renderer");
		return NULL;
	}

	texture->load(format,width,height,depth,0,data);

	if(hasAutogen==false && wantsAutogen==true){
		int mipLevels=texture->getNumMipLevels();
		int hwidth=width,hheight=height,hdepth=depth;
		int i;
		for(i=1;i<mipLevels;++i){
			hwidth/=2; hheight/=2; hdepth/=2;
			int xoff=width/(hwidth+1),yoff=height/(hheight+1),zoff=depth/(hdepth+1);

			Image::ptr mipImage(new Image(dimension,format,hwidth,hheight,hdepth));

			Pixel<uint8> pixel;
			int x,y,z;
			for(z=0;z<hdepth;++z){
				for(y=0;y<hheight;++y){
					for(x=0;x<hwidth;++x){
						image->getPixel(pixel,xoff+x*(1<<i),yoff+y*(1<<i),zoff+z*(1<<i));
						mipImage->setPixel(pixel,x,y,z);
					}
				}
			}

			texture->load(format,mipImage->getWidth(),mipImage->getHeight(),mipImage->getDepth(),i,mipImage->getData());
		}
	}

	manage(shared_static_cast<Texture>(texture));

	return texture;
}

Texture::ptr TextureManager::createTexture(int usageFlags,Texture::Dimension dimension,int format,int width,int height,int depth,int mipLevels){
	Texture::ptr texture;
	if(mBackable){
		BackableTexture::ptr backableTexture(new BackableTexture());
		backableTexture->create(usageFlags,dimension,format,width,height,depth,mipLevels);
		if(mEngine->getRenderer()!=NULL){
			Texture::ptr back(mEngine->getRenderer()->createTexture());
			back->create(usageFlags,dimension,format,width,height,depth,mipLevels);
			backableTexture->setBack(back,true);
		}
		texture=backableTexture;
	}
	else if(mEngine->getRenderer()!=NULL){
		texture=Texture::ptr(mEngine->getRenderer()->createTexture());
		texture->create(usageFlags,dimension,format,width,height,depth,mipLevels);
	}
	else{
		Error::nullPointer("can not create a non-backable Texture without a renderer");
		return NULL;
	}

	manage(shared_static_cast<Texture>(texture));

	return texture;
}

Image::ptr TextureManager::createImage(Texture *texture){
	Image::ptr image(new Image(texture->getDimension(),texture->getFormat(),texture->getWidth(),texture->getHeight(),texture->getDepth()));
	texture->read(image->getFormat(),image->getWidth(),image->getHeight(),image->getDepth(),0,image->getData());
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
			back->create(texture->getUsageFlags(),texture->getDimension(),texture->getFormat(),texture->getWidth(),texture->getHeight(),texture->getDepth(),0);
			shared_static_cast<BackableTexture>(texture)->setBack(back);
		}
		else{
			texture->createContext();
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
		else{
			texture->destroyContext(true);
		}
	}
}

void TextureManager::preContextReset(peeper::Renderer *renderer){
	int i;
	for(i=0;i<mResources.size();++i){
		Texture::ptr texture=shared_static_cast<Texture>(mResources[i]);
		if(texture->contextNeedsReset()){
			texture->destroyContext(true);
		}
	}
}

void TextureManager::postContextReset(peeper::Renderer *renderer){
	int i;
	for(i=0;i<mResources.size();++i){
		Texture::ptr texture=shared_static_cast<Texture>(mResources[i]);
		if(texture->contextNeedsReset()){
			texture->createContext();
		}
	}
}

Renderer *TextureManager::getRenderer(){return mEngine->getRenderer();}

}
}

