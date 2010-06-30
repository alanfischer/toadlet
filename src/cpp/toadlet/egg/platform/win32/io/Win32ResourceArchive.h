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

#ifndef TOADLET_EGG_IO_WIN32RESOURCEARCHIVE_H
#define TOADLET_EGG_IO_WIN32RESOURCEARCHIVE_H

#include <toadlet/egg/BaseResource.h>
#include <toadlet/egg/Map.h>
#include <toadlet/egg/io/Archive.h>

namespace toadlet{
namespace egg{
namespace io{

class TOADLET_API Win32ResourceArchive:public Archive,public BaseResource{
	TOADLET_BASERESOURCE_PASSTHROUGH(Archive);
public:
	TOADLET_SHARED_POINTERS(Win32ResourceArchive);

	Win32ResourceArchive();
	virtual ~Win32ResourceArchive();

	void destroy();

	/// @todo: Move this to tadpole/handlers, and have it be a ResourceArchive if we loaded HBITMAP/HICON resources directly into Textures.
	bool isResourceArchive() const{return false;}

	bool open(void *module);
	void setMap(Map<String,int>::ptr idMap);
	/// @todo: See if we can replace this with changing our resource names from numbers to their actual string, then it would work as expected without the ID lookup
	void buildMapFromStringTable(int startID);

	Stream::ptr openStream(const String &name);
	Resource::ptr openResource(const String &name){return NULL;}

	Collection<String>::ptr getEntries();

	void resourceFound(const String &name);

protected:
	void *findResourceName(const String &name);

	void *mModule;
	Collection<String>::ptr mEntries;
	Map<String,int>::ptr mIDMap;
};

}
}
}

#endif
