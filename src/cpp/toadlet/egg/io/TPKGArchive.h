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

#ifndef TOADLET_EGG_IO_TPKGARCHIVE_H
#define TOADLET_EGG_IO_TPKGARCHIVE_H

#include <toadlet/egg/BaseResource.h>
#include <toadlet/egg/Map.h>
#include <toadlet/egg/io/Archive.h>
#include <toadlet/egg/io/InputStreamFactory.h>
#include <toadlet/egg/io/DataInputStream.h>
#include <toadlet/egg/io/MemoryInputStream.h>

namespace toadlet{
namespace egg{
namespace io{

class TOADLET_API TPKGArchive:public Archive,public BaseResource{
public:
	TPKGArchive();
	~TPKGArchive();

	virtual void destroy();

	bool open(MemoryInputStream::ptr memoryInputStream);
	bool open(InputStream::ptr inputStream);

	InputStream::ptr makeInputStream(const String &name);

	TOADLET_BASERESOURCE_PASSTHROUGH(Archive);

protected:
	class Index{
	public:
		uint32 position;
		uint32 length;
	};

	Map<String,Index> mIndex;
	int mDataOffset;
	DataInputStream::ptr mInputStream;
	MemoryInputStream::ptr mMemoryInputStream;
};

}
}
}

#endif
