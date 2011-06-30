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
#include <Foundation/NSBundle.h>

namespace toadlet{
namespace tadpole{
namespace handler{

class TOADLET_API OSXBundleArchive:public Archive,public BaseResource{
	TOADLET_BASERESOURCE_PASSTHROUGH(Archive);
public:
	TOADLET_SHARED_POINTERS(OSXBundleArchive);

	OSXBundleArchive();
	virtual ~OSXBundleArchive();

	void destroy();

	bool open(NSBundle *bundle);

	Stream::ptr openStream(const String &name);
	Resource::ptr openResource(const String &name){return NULL;}

	Collection<String>::ptr getEntries();

protected:
	NSBundle *mBundle;
	Collection<String>::ptr mEntries;
};

}
}
}

#endif
