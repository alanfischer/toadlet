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
#include <toadlet/peeper/Renderer.h>
#include <toadlet/peeper/BackableTexture.h>

namespace toadlet{
namespace tadpole{

class Engine;

class TextureManager:public ResourceManager{
public:
	TextureManager(Engine *engine);

	peeper::Texture::ptr createTexture(const egg::image::Image::ptr &image);
	peeper::Texture::ptr createTexture(int usageFlags,peeper::Texture::Dimension dimension,int format,int width,int height,int depth,int mipLevels);
	egg::image::Image::ptr createImage(const peeper::Texture::ptr &texture);

	virtual void contextActivate(peeper::Renderer *renderer);
	virtual void contextDeactivate(peeper::Renderer *renderer);
	virtual void contextUpdate(peeper::Renderer *renderer);
	virtual void preContextReset(peeper::Renderer *renderer);
	virtual void postContextReset(peeper::Renderer *renderer);

protected:
	Engine *mEngine;
	egg::Collection<peeper::BackableTexture::ptr> mTextures;
};

}
}

#endif
