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

TextureManager::TextureManager(Engine *engine):ResourceManager(engine){
	mEngine=engine;
}

Texture::ptr TextureManager::createTexture(const Image::ptr &image,int usageFlags,int mipLevels){
	bool hasAutogen=mEngine->getRenderer()->getCapabilitySet().textureAutogenMipMaps;
	bool wantsAutogen=(usageFlags&Texture::UsageFlags_AUTOGEN_MIPMAPS)>0;

	if(hasAutogen==false && wantsAutogen==true){
		usageFlags&=~Texture::UsageFlags_AUTOGEN_MIPMAPS;

		if(mipLevels==0){
			int hwidth=image->getWidth(),hheight=image->getHeight();
			while(hwidth>0 && hheight>0){
				mipLevels++;
				hwidth/=2; hheight/=2;
			}
		}
	}

	BackableTexture::ptr texture(new BackableTexture());
	texture->create(usageFlags,image->getDimension(),image->getFormat(),image->getWidth(),image->getHeight(),image->getDepth(),mipLevels);
	if(mEngine->getRenderer()!=NULL){
		Texture::ptr back(mEngine->getRenderer()->createTexture());
		back->create(usageFlags,image->getDimension(),image->getFormat(),image->getWidth(),image->getHeight(),image->getDepth(),mipLevels);
		texture->setBack(back,true);
	}

	texture->load(image->getFormat(),image->getWidth(),image->getHeight(),image->getDepth(),0,image->getData());
	if(hasAutogen==false && wantsAutogen==true){
		int mipLevels=texture->getNumMipLevels();
		int width=image->getWidth(),height=image->getHeight();
		int hwidth=width,hheight=height;
		int i;
		for(i=1;i<mipLevels;++i){
			hwidth/=2; hheight/=2;
			int xoff=width/(hwidth+1),yoff=height/(hheight+1);

			Image::ptr mipImage(new Image(image->getDimension(),image->getFormat(),hwidth,hheight,image->getDepth()));

			Pixel<uint8> pixel;
			int x,y;
			for(x=0;x<hwidth;++x){
				for(y=0;y<hheight;++y){
					image->getPixel(pixel,xoff+x*(1<<i),yoff+y*(1<<i));
					mipImage->setPixel(pixel,x,y);
				}
			}

			texture->load(mipImage->getFormat(),mipImage->getWidth(),mipImage->getHeight(),mipImage->getDepth(),i,mipImage->getData());
		}
	}

	manage(shared_static_cast<Texture>(texture));

	return texture;
}

Texture::ptr TextureManager::createTexture(int usageFlags,Texture::Dimension dimension,int format,int width,int height,int depth,int mipLevels){
	BackableTexture::ptr texture(new BackableTexture());
	texture->create(usageFlags,dimension,format,width,height,depth,mipLevels);
	if(mEngine->getRenderer()!=NULL){
		Texture::ptr back(mEngine->getRenderer()->createTexture());
		back->create(usageFlags,dimension,format,width,height,depth,mipLevels);
		texture->setBack(back,true);
	}

	manage(shared_static_cast<Texture>(texture));

	return texture;
}

Image::ptr TextureManager::createImage(const Texture::ptr &texture){
	Image::ptr image(new Image(texture->getDimension(),texture->getFormat(),texture->getWidth(),texture->getHeight(),texture->getDepth()));
	texture->read(image->getFormat(),image->getWidth(),image->getHeight(),image->getDepth(),0,image->getData());
	return image;
}

SurfaceRenderTarget::ptr TextureManager::createSurfaceRenderTarget(){
	// TODO: Make this use a BackableSufaceRenderTarget
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
		if(texture->getRootTexture()!=texture){
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
		if(texture->getRootTexture()!=texture){
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

}
}

