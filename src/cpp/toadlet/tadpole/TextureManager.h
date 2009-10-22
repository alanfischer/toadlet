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

#ifndef TOADLET_TADPOLE_TEXTUREMANAGER_H
#define TOADLET_TADPOLE_TEXTUREMANAGER_H

#include <toadlet/tadpole/ResourceManager.h>
#include <toadlet/peeper/Texture.h>
#include <toadlet/tadpole/Engine.h>

namespace toadlet{
namespace tadpole{

class TextureManager:public ResourceManager{
public:
	TextureManager(Engine *engine);

	peeper::Texture::ptr createTexture(const egg::image::Image::ptr &image);
	egg::image::Image::ptr createImage(const peeper::Texture::ptr &texture);
	
	void resourceLoaded(const egg::Resource::ptr &resource);

protected:
	Engine *mEngine;
};

}
}

#endif
