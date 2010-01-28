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

#ifndef TOADLET_EGG_IO_ZIPARCHIVE_H
#define TOADLET_EGG_IO_ZIPARCHIVE_H

#include <toadlet/egg/BaseResource.h>
#include <toadlet/egg/Map.h>
#include <toadlet/egg/io/Archive.h>
#include <toadlet/egg/io/DataStream.h>
#include <toadlet/egg/io/MemoryStream.h>
#include <zzip/zzip.h>
#include <zzip/plugin.h>

namespace toadlet{
namespace egg{
namespace io{

class TOADLET_API ZIPArchive:public Archive,public BaseResource{
	TOADLET_BASERESOURCE_PASSTHROUGH(Archive);
public:
	TOADLET_SHARED_POINTERS(ZIPArchive);

	ZIPArchive();
	virtual ~ZIPArchive();

	void destroy();

	bool isResourceArchive() const{return false;}

	bool open(MemoryStream::ptr memoryStream);
	bool open(Stream::ptr stream);

	Stream::ptr openStream(const String &name);
	Resource::ptr openResource(const String &name){return NULL;}

protected:
	zzip_plugin_io_handlers mIO;
	DataStream::ptr mStream;
};

}
}
}

#endif
