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

#ifndef TOADLET_TADPOLE_WADARCHIVE_H
#define TOADLET_TADPOLE_WADARCHIVE_H

#include <toadlet/egg/BaseResource.h>
#include <toadlet/egg/io/Archive.h>
#include <toadlet/egg/io/DataStream.h>
#include <toadlet/tadpole/TextureManager.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API WADArchive:public Archive,public BaseResource{
public:
	TOADLET_RESOURCE(WADArchive,Archive);

	WADArchive(TextureManager *textureManager);

	void destroy();

	bool open(Stream::ptr stream);

	Stream::ptr openStream(const String &name){return NULL;}
	Resource::ptr openResource(const String &name);

	const Collection<String> &getEntries(){return mEntries;}

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

	static Texture::ptr createTexture(TextureManager *textureManager,wmiptex *miptex,tbyte *pal=NULL);

protected:
	unsigned char mInBuffer[640*480*85/64]; // Maxlump

	TextureManager *mTextureManager;
	DataStream::ptr mStream;

	wheader mHeader;
	Collection<wlumpinfo> mLumpinfos;
	Map<String,int> mNameMap;
	Collection<String> mEntries;
};

}
}

#endif

