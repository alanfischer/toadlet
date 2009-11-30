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

#ifndef TOADLET_TADPOLE_MATERIALMANAGER_H
#define TOADLET_TADPOLE_MATERIALMANAGER_H

#include <toadlet/tadpole/Material.h>
#include <toadlet/tadpole/ResourceManager.h>
#include <toadlet/tadpole/TextureManager.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API MaterialManager:public ResourceManager{
public:
	MaterialManager(egg::io::InputStreamFactory *inputStreamFactory,TextureManager *textureManager);

	Material::ptr createMaterial();

	Material::ptr findMaterial(const egg::String &name){return egg::shared_static_cast<Material>(ResourceManager::find(name));}

	peeper::TextureStage::ptr createTextureStage(peeper::Texture::ptr texture);

	virtual egg::Resource::ptr unableToFindHandler(const egg::String &name,const ResourceHandlerData *handlerData);

	void setDefaultMinFilter(peeper::TextureStage::Filter filter){mDefaultMinFilter=filter;}
	inline peeper::TextureStage::Filter getDefaultMinFilter(){return mDefaultMinFilter;}
	void setDefaultMagFilter(peeper::TextureStage::Filter filter){mDefaultMagFilter=filter;}
	inline peeper::TextureStage::Filter getDefaultMagFilter(){return mDefaultMagFilter;}
	void setDefaultMipFilter(peeper::TextureStage::Filter filter){mDefaultMipFilter=filter;}
	inline peeper::TextureStage::Filter getDefaultMipFilter(){return mDefaultMipFilter;}

protected:
	TextureManager *mTextureManager;
	peeper::TextureStage::Filter mDefaultMinFilter;
	peeper::TextureStage::Filter mDefaultMagFilter;
	peeper::TextureStage::Filter mDefaultMipFilter;
};

}
}

#endif
