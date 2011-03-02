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
#include <toadlet/peeper/PixelBufferRenderTarget.h>

namespace toadlet{
namespace tadpole{

class Engine;

class TOADLET_API TextureManager:public ResourceManager,public peeper::RenderTargetDestroyedListener{
public:
	TextureManager(Engine *engine,bool backable);
	virtual ~TextureManager();

	void destroy();

	peeper::Texture::ptr createTexture(egg::image::Image::ptr image,int usage=peeper::Texture::Usage_BIT_STATIC|peeper::Texture::Usage_BIT_AUTOGEN_MIPMAPS,int mipLevels=0);
	peeper::Texture::ptr createTexture(egg::image::Image::ptr images[],int mipLevels,int usage=peeper::Texture::Usage_BIT_STATIC);
	peeper::Texture::ptr createTexture(int usage,peeper::Texture::Dimension dimension,int format,int width,int height,int depth,int mipLevels);
	egg::image::Image::ptr createImage(peeper::Texture *texture);
	peeper::PixelBufferRenderTarget::ptr createPixelBufferRenderTarget();

	peeper::Texture::ptr findTexture(const egg::String &name){return egg::shared_static_cast<peeper::Texture>(ResourceManager::find(name));}

	void contextActivate(peeper::Renderer *renderer);
	void contextDeactivate(peeper::Renderer *renderer);
	void preContextReset(peeper::Renderer *renderer);
	void postContextReset(peeper::Renderer *renderer);

	void renderTargetDestroyed(peeper::RenderTarget *renderTarget);

	peeper::Renderer *getRenderer();

	peeper::Texture::ptr createNormalization(int size);

protected:
	egg::Collection<peeper::PixelBufferRenderTarget::ptr> mRenderTargets;

	Engine *mEngine;
	bool mBackable;
};

}
}

#endif
