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

#ifndef TOADLET_TADPOLE_WADSTREAMER_H
#define TOADLET_TADPOLE_WADSTREAMER_H

#include <toadlet/tadpole/ResourceStreamer.h>
#include "WADArchive.h"

namespace toadlet{
namespace tadpole{

class TOADLET_API WADStreamer:public Object,public ResourceStreamer{
public:
	TOADLET_IOBJECT(WADStreamer);

	WADStreamer(TextureManager *textureManager){
		mTextureManager=textureManager;
	}

	Resource::ptr load(Stream::ptr stream,ResourceData *data,ProgressListener *listener){
		WADArchive::ptr archive(new WADArchive(mTextureManager));
		bool result=archive->open(stream);
		if(result){
			return shared_static_cast<Archive>(archive);
		}
		else{
			return NULL;
		}
	}

protected:
	TextureManager *mTextureManager;
};

}
}

#endif

