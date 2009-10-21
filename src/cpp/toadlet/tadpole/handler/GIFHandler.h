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
#include <toadlet/peeper/TextureSequence.h>
#include <toadlet/tadpole/ResourceHandler.h>

namespace toadlet{
namespace tadpole{
namespace handler{

class TOADLET_API GIFHandler:public ResourceHandler{
public:
	TOADLET_SHARED_POINTERS(GIFHandler,ResourceHandler);

	GIFHandler(ResourceManager *textureManager){
		mTextureManager=textureManager;
	}

	egg::Resource *load(egg::io::InputStream::ptr in,const ResourceHandlerData *handlerData){
		egg::Collection<egg::image::Image*> images;
		egg::Collection<int> delayMilliseconds;

		mHandler.loadAnimatedImage(in,images,delayMilliseconds);

		if(images.size()==0){
			return NULL;
		}
		else if(images.size()==1){
			return new peeper::Texture(egg::image::Image::ptr(images[0]));
		}
		else{
			peeper::TextureSequence *textureSequence=new peeper::TextureSequence();
			textureSequence->setNumFrames(images.size());

			int i;
			for(i=0;i<images.size();++i){
				egg::image::Image::ptr image=egg::image::Image::ptr(images[i]);
				peeper::Texture::ptr frame(new peeper::Texture(image));
				// TODO: Work delay into TextureSequences
				textureSequence->setFrame(i,egg::shared_static_cast<peeper::Texture>(mTextureManager->load(frame)));
			}

			return textureSequence;
		}
	}

	bool save(peeper::Texture *resource,egg::io::OutputStream::ptr out){
		return mHandler.saveImage(resource->getImage(),out);
	}

protected:
	egg::image::GIFHandler mHandler;
	ResourceManager *mTextureManager;
};

}
}
}

#endif

