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

#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/peeper/Texture.h>
#include <toadlet/tadpole/MaterialManager.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{

MaterialManager::MaterialManager(InputStreamFactory *inputStreamFactory,TextureManager *textureManager):ResourceManager(inputStreamFactory){
	mTextureManager=textureManager;
	mDefaultMinFilter=TextureStage::Filter_LINEAR;
	mDefaultMagFilter=TextureStage::Filter_LINEAR;
	mDefaultMipFilter=TextureStage::Filter_LINEAR;
}

Material::ptr MaterialManager::createMaterial(){
	Material::ptr material(new Material());
	manage(material);
	return material;
}

TextureStage::ptr MaterialManager::createTextureStage(Texture::ptr texture){
	TextureStage::ptr textureStage(new TextureStage(texture));
	textureStage->setMinFilter(mDefaultMinFilter);
	textureStage->setMagFilter(mDefaultMagFilter);
	if(texture!=NULL && texture->getNumMipLevels()>1){
		textureStage->setMipFilter(mDefaultMipFilter);
	}
	else{
		textureStage->setMipFilter(TextureStage::Filter_NONE);
	}
	return textureStage;
}

Resource::ptr MaterialManager::unableToFindHandler(const egg::String &name,const ResourceHandlerData *handlerData){
	Texture::ptr texture=mTextureManager->findTexture(name);
	if(texture!=NULL){
		Material::ptr material(new Material());

		material->setTextureStage(0,createTextureStage(texture));

		material->setLighting(true);

		return material;
	}
	else{
		return ResourceManager::unableToFindHandler(name,handlerData);
	}
}

}
}

