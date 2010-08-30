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

#ifndef TOADLET_TADPOLE_HANDLER_TGAHANDLER_H
#define TOADLET_TADPOLE_HANDLER_TGAHANDLER_H

#include <toadlet/egg/image/TGAHandler.h>
#include <toadlet/peeper/Texture.h>
#include <toadlet/tadpole/ResourceHandler.h>

namespace toadlet{
namespace tadpole{
namespace handler{

class TOADLET_API TGAHandler:public ResourceHandler{
public:
	TOADLET_SHARED_POINTERS(TGAHandler);

	TGAHandler(TextureManager *textureManager){mTextureManager=textureManager;}

	egg::Resource::ptr load(egg::io::Stream::ptr stream,const ResourceHandlerData *handlerData){
		egg::image::Image::ptr image(mHandler.loadImage(stream));
		if(image!=NULL){
			return mTextureManager->createTexture(image);
		}
		else{
			return NULL;
		}
	}

	bool save(peeper::Texture::ptr resource,egg::io::Stream::ptr stream){
		return mHandler.saveImage(mTextureManager->createImage(resource),stream);
	}

protected:
	TextureManager *mTextureManager;
	egg::image::TGAHandler mHandler;
};

}
}
}

#endif

