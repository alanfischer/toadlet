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

#ifndef TOADLET_TADPOLE_HANDLER_GIFHANDLER_H
#define TOADLET_TADPOLE_HANDLER_GIFHANDLER_H

#include <toadlet/egg/image/GIFHandler.h>
#include <toadlet/tadpole/ResourceHandler.h>

namespace toadlet{
namespace tadpole{
namespace handler{

class TOADLET_API GIFHandler:public ResourceHandler{
public:
	TOADLET_SHARED_POINTERS(GIFHandler);

	GIFHandler(TextureManager *textureManager){mTextureManager=textureManager;}

	Resource::ptr load(Stream::ptr stream,const ResourceHandlerData *handlerData){
		Collection<Image*> images;
		Collection<int> delays;

		mHandler.loadAnimatedImage(stream,images,delays);

		if(images.size()==0){
			return NULL;
		}
		else if(images.size()==1){
			return mTextureManager->createTexture(Image::ptr(images[0]));
		}
		else{
/// @todo: Revive this somehow, probably as a 3d texture
//			SequenceTexture::ptr sequence(new SequenceTexture(Texture::Dimension_D2,images.size()));
//			int i;
//			for(i=0;i<images.size();++i){
//				sequence->setTexture(i,mTextureManager->createTexture(Image::ptr(images[i])),Math::fromMilli(delays[i]));
//			}
//			return shared_static_cast<Texture>(sequence);
return NULL;
		}
	}

	bool save(Texture::ptr resource,Stream::ptr stream){
		return mHandler.saveImage(mTextureManager->createImage(resource),stream);
	}

protected:
	TextureManager *mTextureManager;
	egg::image::GIFHandler mHandler;
};

}
}
}

#endif

