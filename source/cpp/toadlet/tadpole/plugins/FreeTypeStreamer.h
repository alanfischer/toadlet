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

#ifndef TOADLET_TADPOLE_FREETYPESTREAMER_H
#define TOADLET_TADPOLE_FREETYPESTREAMER_H

#include <toadlet/tadpole/Font.h>
#include <toadlet/tadpole/BaseResourceStreamer.h>
#include <toadlet/tadpole/TextureManager.h>

extern "C" {
	#define USE_FREETYPE2_STATIC
	#include <ft2build.h>
	#include FT_FREETYPE_H
	#include FT_GLYPH_H
	#include FT_OUTLINE_H
	#include FT_TRIGONOMETRY_H
}

namespace toadlet{
namespace tadpole{

class FreeTypeStreamer:public BaseResourceStreamer{
public:
	TOADLET_OBJECT(FreeTypeStreamer);

	FreeTypeStreamer(TextureManager *textureManager);
	virtual ~FreeTypeStreamer();

	Resource::ptr load(Stream::ptr stream,ResourceData *data);

protected:
	TextureManager *mTextureManager;
	FT_Library mLibrary;
};

}
}

#endif
