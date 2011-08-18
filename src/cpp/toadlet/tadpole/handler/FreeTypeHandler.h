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

#ifndef TOADLET_TADPOLE_HANDLER_FREETYPEHANDLER_H
#define TOADLET_TADPOLE_HANDLER_FREETYPEHANDLER_H

#include <toadlet/tadpole/Font.h>
#include <toadlet/tadpole/ResourceStreamer.h>
#include <toadlet/tadpole/TextureManager.h>

extern "C" {
	#define USE_FREETYPE2_STATIC
	#include <ft2build.h>
	#include <freetype/freetype.h>
	#include <freetype/ftglyph.h>
	#include <freetype/ftoutln.h>
	#include <freetype/fttrigon.h>
}

namespace toadlet{
namespace tadpole{
namespace handler{

class FreeTypeHandler:public ResourceStreamer{
public:
	TOADLET_SHARED_POINTERS(FreeTypeHandler);

	FreeTypeHandler(TextureManager *textureManager);
	virtual ~FreeTypeHandler();

	Resource::ptr load(Stream::ptr stream,ResourceData *data,ProgressListener *listener);

protected:
	TextureManager *mTextureManager;
	FT_Library mLibrary;
};

}
}
}

#endif
