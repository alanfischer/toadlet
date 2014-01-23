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
#include <toadlet/peeper/PixelBufferRenderTarget.h>
#include <toadlet/tadpole/ResourceManager.h>
#include <toadlet/tadpole/TextureData.h>
#include <toadlet/tadpole/VideoHandler.h>

namespace toadlet{
namespace tadpole{

class Engine;

class TOADLET_API TextureManager:public ResourceManager{
public:
	TOADLET_OBJECT(TextureManager);

	TextureManager(Engine *engine);
	virtual ~TextureManager();

	void destroy();

	Texture::ptr createTexture(TextureFormat::ptr format,tbyte *data=NULL);
	Texture::ptr createTexture(TextureFormat::ptr format,tbyte *mipDatas[]);
	Texture::ptr createTexture(int usage,TextureFormat::ptr format,tbyte *data=NULL);
	Texture::ptr createTexture(int usage,TextureFormat::ptr format,tbyte *mipDatas[]);
	Texture::ptr findTexture(const String &name){return shared_static_cast<Texture>(ResourceManager::find(name));}
	Texture::ptr findTexture(const String &name,ResourceData::ptr data){return shared_static_cast<Texture>(ResourceManager::find(name,data));}

	PixelBufferRenderTarget::ptr createPixelBufferRenderTarget();

	bool textureLoad(Texture::ptr texture,TextureFormat *format,tbyte *mipData);

	void contextActivate(RenderDevice *renderDevice);
	void contextDeactivate(RenderDevice *renderDevice);
	void preContextReset(RenderDevice *renderDevice);
	void postContextReset(RenderDevice *renderDevice);

	void resourceDestroyed(Resource *resource);

	void setVideoHandler(VideoHandler::ptr handler){mVideoHandler=handler;}
	VideoHandler::ptr getVideoHandler() const{return mVideoHandler;}

protected:
	Collection<PixelBufferRenderTarget::ptr> mRenderTargets;
	VideoHandler::ptr mVideoHandler;
};

}
}

#endif
