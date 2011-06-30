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

#include <toadlet/peeper/RenderDevice.h>
#include <toadlet/peeper/BackableTexture.h>
#include <toadlet/peeper/PixelBufferRenderTarget.h>
#include <toadlet/tadpole/ResourceManager.h>

namespace toadlet{
namespace tadpole{

class Engine;

class TOADLET_API TextureManager:public ResourceManager,public RenderTargetDestroyedListener{
public:
	TextureManager(Engine *engine,bool backable);
	virtual ~TextureManager();

	void destroy();

	/// @todo: We need a TextureFormat class, which has a PixelFormat, plus usage, dimension, width, height, depth, pitch, and mipLevels
	Texture::ptr createTexture(Image::ptr image);
	Texture::ptr createTexture(int mipLevels,Image::ptr mipImages[]);
	Texture::ptr createTexture(int usage,Texture::Dimension dimension,int format,int width,int height,int depth,int mipLevels,tbyte *mipDatas[]=NULL);
	Image::ptr createImage(Texture *texture);
	PixelBufferRenderTarget::ptr createPixelBufferRenderTarget();

	Texture::ptr findTexture(const String &name){return shared_static_cast<Texture>(ResourceManager::find(name));}

	bool textureLoad(Texture::ptr texture,int pixelFormat,int width,int height,int depth,int mipLevel,tbyte *mipData);

	void contextActivate(RenderDevice *renderDevice);
	void contextDeactivate(RenderDevice *renderDevice);
	void preContextReset(RenderDevice *renderDevice);
	void postContextReset(RenderDevice *renderDevice);

	void renderTargetDestroyed(RenderTarget *renderTarget);

	Texture::ptr createNormalization(int size);

protected:
	Collection<PixelBufferRenderTarget::ptr> mRenderTargets;

	Engine *mEngine;
	bool mBackable;
};

}
}

#endif
