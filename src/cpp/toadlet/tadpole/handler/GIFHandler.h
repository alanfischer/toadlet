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
#include <toadlet/peeper/SequenceTexture.h>
#include <toadlet/tadpole/ResourceHandler.h>

namespace toadlet{
namespace tadpole{
namespace handler{

class TOADLET_API GIFHandler:public ResourceHandler{
public:
	TOADLET_SHARED_POINTERS(GIFHandler);

	GIFHandler(TextureManager *textureManager){mTextureManager=textureManager;}

	egg::Resource::ptr load(egg::io::Stream::ptr stream,const ResourceHandlerData *handlerData){
		egg::Collection<egg::image::Image*> images;
		egg::Collection<int> delayMilliseconds;

		mHandler.loadAnimatedImage(stream,images,delayMilliseconds);

		if(images.size()==0){
			return NULL;
		}
		else if(images.size()==1){
			return mTextureManager->createTexture(egg::image::Image::ptr(images[0]));
		}
		else{
			peeper::SequenceTexture::ptr sequence(new peeper::SequenceTexture(peeper::Texture::Dimension_D2,images.size()));
			int i;
			for(i=0;i<images.size();++i){
				sequence->setTexture(i,mTextureManager->createTexture(egg::image::Image::ptr(images[i])));
			}
			return egg::shared_static_cast<peeper::Texture>(sequence);
		}
	}

	bool save(peeper::Texture::ptr resource,egg::io::Stream::ptr stream){
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

