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

#ifndef TOADLET_TADPOLE_HANDLER_OSXFONTHANDLER_H
#define TOADLET_TADPOLE_HANDLER_OSXFONTHANDLER_H

#include <toadlet/tadpole/Font.h>
#include <toadlet/tadpole/FontData.h>
#include <toadlet/tadpole/TextureManager.h>

namespace toadlet{
namespace tadpole{
namespace handler{

/// @brief  A lighter weight font handling class for the OSX platform
///  If the FreeTypeHandler is unavailable or too heavy weight to include, this class may be used.

class TOADLET_API OSXFontHandler:public ResourceHandler{
public:
	OSXFontHandler(TextureManager *textureManager);
	virtual ~OSXFontHandler(){}

	egg::Resource::ptr load(egg::io::Stream::ptr in,const ResourceHandlerData *handlerData);

protected:
	TextureManager *mTextureManager;
};

}
}
}

#endif
