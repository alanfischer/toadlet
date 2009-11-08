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

#include <toadlet/tadpole/TextureManager.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/peeper/BackableTexture.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::image;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{

TextureManager::TextureManager(Engine *engine):ResourceManager(engine){
	mEngine=engine;
}

Texture::ptr TextureManager::createTexture(const Image::ptr &image){
	BackableTexture::ptr texture(new BackableTexture());
	texture->create(Texture::UsageFlags_AUTOGEN_MIPMAPS,image->getDimension(),image->getFormat(),image->getWidth(),image->getHeight(),image->getDepth(),0);
	if(mEngine->getRenderer()!=NULL){
		Texture::ptr back(mEngine->getRenderer()->createTexture());
		back->create(Texture::UsageFlags_AUTOGEN_MIPMAPS,image->getDimension(),image->getFormat(),image->getWidth(),image->getHeight(),image->getDepth(),0);
		texture->setBack(back,true);
	}
	texture->load(image->getFormat(),image->getWidth(),image->getHeight(),image->getDepth(),image->getData());
	mTextures.add(texture);
	return texture;
}

Texture::ptr TextureManager::createTexture(int usageFlags,Texture::Dimension dimension,int format,int width,int height,int depth,int mipLevels){
	BackableTexture::ptr texture(new BackableTexture());
	texture->create(usageFlags,dimension,format,width,height,depth,mipLevels);
	if(mEngine->getRenderer()!=NULL){
		Texture::ptr back(mEngine->getRenderer()->createTexture());
		back->create(usageFlags,dimension,format,width,height,depth,mipLevels);
		texture->setBack(back);
	}
	mTextures.add(texture);
	return texture;
}

Image::ptr TextureManager::createImage(const Texture::ptr &texture){
	Image::ptr image(new Image(texture->getDimension(),texture->getFormat(),texture->getWidth(),texture->getHeight(),texture->getDepth()));
	texture->read(image->getFormat(),image->getWidth(),image->getHeight(),image->getDepth(),image->getData());
	return image;
}

void TextureManager::contextActivate(peeper::Renderer *renderer){
	int i;
	for(i=0;i<mTextures.size();++i){
		BackableTexture::ptr texture=mTextures[i];
		Texture::ptr back(renderer->createTexture());
		back->create(texture->getUsageFlags(),texture->getDimension(),texture->getFormat(),texture->getWidth(),texture->getHeight(),texture->getDepth(),0);
		texture->setBack(back);
	}
}

void TextureManager::contextDeactivate(peeper::Renderer *renderer){
	int i;
	for(i=0;i<mTextures.size();++i){
		BackableTexture::ptr texture=mTextures[i];
		texture->setBack(NULL);
	}
}

void TextureManager::contextUpdate(peeper::Renderer *renderer){
	int i;
	for(i=0;i<mTextures.size();++i){
		BackableTexture::ptr texture=mTextures[i];
		if(texture->getBack()==NULL){
			Texture::ptr back(renderer->createTexture());
			back->create(texture->getUsageFlags(),texture->getDimension(),texture->getFormat(),texture->getWidth(),texture->getHeight(),texture->getDepth(),0);
			texture->setBack(back);
		}
	}
}

void TextureManager::preContextReset(peeper::Renderer *renderer){
}

void TextureManager::postContextReset(peeper::Renderer *renderer){
}

}
}

