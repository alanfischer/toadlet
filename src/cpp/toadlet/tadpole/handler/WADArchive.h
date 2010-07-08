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

#ifndef TOADLET_TADPOLE_HANDLER_WADARCHIVE_H
#define TOADLET_TADPOLE_HANDLER_WADARCHIVE_H

#include <toadlet/egg/BaseResource.h>
#include <toadlet/egg/io/Archive.h>
#include <toadlet/egg/io/DataStream.h>
#include <toadlet/tadpole/TextureManager.h>

namespace toadlet{
namespace tadpole{
namespace handler{

class TOADLET_API WADArchive:public egg::io::Archive,public egg::BaseResource{
	TOADLET_BASERESOURCE_PASSTHROUGH(Archive);
public:
	TOADLET_SHARED_POINTERS(WADArchive);

	WADArchive(TextureManager *textureManager);
	virtual ~WADArchive();

	void destroy();

	bool open(egg::io::Stream::ptr stream);

	egg::io::Stream::ptr openStream(const egg::String &name){return NULL;}
	egg::Resource::ptr openResource(const egg::String &name);

	egg::Collection<egg::String>::ptr getEntries();

	struct wheader{
		char identification[4]; // should be WAD2 or 2DAW
		int numlumps;
		int	infotableofs;
	};

	struct wlumpinfo{
		int filepos;
		int disksize;
		int size; // uncompressed
		char type;
		char compression;
		char pad1, pad2;
		char name[16]; // must be null terminated
	};

	const static int MIPLEVELS=4;
	struct wmiptex{
		char name[16];
		unsigned int width, height;
		unsigned int offsets[MIPLEVELS]; // four mip maps stored
	};

	static peeper::Texture::ptr createTexture(TextureManager *textureManager,wmiptex *miptex);

protected:
	unsigned char mInBuffer[640*480*85/64]; // Maxlump

	TextureManager *mTextureManager;
	egg::io::DataStream::ptr mStream;

	wheader mHeader;
	egg::Collection<wlumpinfo> mLumpinfos;
	egg::Map<egg::String,int> mNameMap;
};

}
}
}

#endif

