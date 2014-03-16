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

#ifndef TOADLET_EGG_WIN32RESOURCEARCHIVE_H
#define TOADLET_EGG_WIN32RESOURCEARCHIVE_H

#include <toadlet/egg/BaseResource.h>
#include <toadlet/egg/Map.h>
#include <toadlet/egg/io/BaseArchive.h>

namespace toadlet{
namespace egg{

class TOADLET_API Win32ResourceArchive:public BaseArchive{
public:
	TOADLET_OBJECT(Win32ResourceArchive);

	Win32ResourceArchive();

	void destroy();

	virtual bool open(void *module);
	void setMap(const Map<String,int> &idMap);

	Stream::ptr openStream(const String &name);

	const Collection<String> &getEntries();

	void resourceFound(const String &name);

protected:
	void *findResourceName(const String &name);
	bool findResources(char *type);

	void *mModule;
	Collection<String> mEntries;
	Map<String,int> mIDMap;
};

}
}

#endif
