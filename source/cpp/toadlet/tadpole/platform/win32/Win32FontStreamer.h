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

#ifndef TOADLET_TADPOLE_WIN32FONTSTREAMER_H
#define TOADLET_TADPOLE_WIN32FONTSTREAMER_H

#include <toadlet/tadpole/Font.h>
#include <toadlet/tadpole/FontData.h>
#include <toadlet/tadpole/TextureManager.h>
#include <windows.h>

namespace toadlet{
namespace tadpole{

/// @brief  A lighter weight font handling class for the Win32 platform
class TOADLET_API Win32FontStreamer:public Object,public ResourceStreamer{
public:
	TOADLET_OBJECT(Win32FontStreamer);

	Win32FontStreamer(TextureManager *textureManager);
	virtual ~Win32FontStreamer();
	bool valid();

	Resource::ptr load(Stream::ptr stream,ResourceData *data,ProgressListener *listener);

protected:
	TextureManager *mTextureManager;
	#if !defined(TOADLET_PLATFORM_WINCE)
		ULONG_PTR mToken;
	#endif
};

}
}

#endif
