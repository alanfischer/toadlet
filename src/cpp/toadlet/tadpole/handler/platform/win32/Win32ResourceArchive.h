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

#ifndef TOADLET_TADPOLE_HANDLER_WIN32RESOURCEARCHIVE_H
#define TOADLET_TADPOLE_HANDLER_WIN32RESOURCEARCHIVE_H

#include <toadlet/egg/BaseResource.h>
#include <toadlet/egg/Map.h>
#include <toadlet/egg/io/Archive.h>
#include <toadlet/tadpole/TextureManager.h>

namespace toadlet{
namespace tadpole{
namespace handler{

class TOADLET_API Win32ResourceArchive:public Archive,public BaseResource{
	TOADLET_BASERESOURCE_PASSTHROUGH(Archive);
public:
	TOADLET_SHARED_POINTERS(Win32ResourceArchive);

	Win32ResourceArchive(TextureManager *textureManager);
	virtual ~Win32ResourceArchive();

	void destroy();

	bool open(void *module);
	void setMap(Map<String,int>::ptr idMap);

	Stream::ptr openStream(const String &name);
	Resource::ptr openResource(const String &name);

	Collection<String>::ptr getEntries();

	void resourceFound(const String &name);

protected:
	void *findResourceName(const String &name);

	void *mModule;
	Collection<String>::ptr mEntries;
	Map<String,int>::ptr mIDMap;

	TextureManager *mTextureManager;
};

}
}
}

#endif
