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
	Texture::ptr texture(mEngine->getRenderer()->createTexture());
	texture->create(Texture::UsageFlags_NONE,image->getDimension(),image->getFormat(),image->getWidth(),image->getHeight(),image->getDepth());
	texture->load(image->getFormat(),image->getWidth(),image->getHeight(),image->getDepth(),image->getData());
	return texture;
}

Image::ptr TextureManager::createImage(const Texture::ptr &texture){
	Image::ptr image(new Image(texture->getDimension(),texture->getFormat(),texture->getWidth(),texture->getHeight(),texture->getDepth()));
	texture->read(image->getFormat(),image->getWidth(),image->getHeight(),image->getDepth(),image->getData());
	return image;
}

void TextureManager::resourceLoaded(const Resource::ptr &resource){
	Texture::ptr texture=shared_static_cast<Texture>(resource);

//	texture->setAutoGenerateMipMaps(true);
//	texture->setMinFilter(Texture::Filter_LINEAR);
//	texture->setMipFilter(Texture::Filter_NEAREST);
//	texture->setMagFilter(Texture::Filter_LINEAR);
}

}
}

