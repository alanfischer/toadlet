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

#ifndef TOADLET_TADPOLE_TPKGINPUTSTREAMFACTORY_H
#define TOADLET_TADPOLE_TPKGINPUTSTREAMFACTORY_H

#include <toadlet/egg/Map.h>
#include <toadlet/egg/io/InputStreamFactory.h>
#include <toadlet/egg/io/DataInputStream.h>
#include <toadlet/egg/io/MemoryInputStream.h>
#include <stdio.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API TPKGInputStreamFactory:public egg::io::InputStreamFactory{
public:
	TPKGInputStreamFactory();
	~TPKGInputStreamFactory();

	bool init(egg::io::MemoryInputStream::ptr memoryInputStream);
	bool init(egg::io::InputStream::ptr inputStream);

	egg::io::InputStream::ptr makeInputStream(const egg::String &name);

protected:
	class Index{
	public:
		uint32 position;
		uint32 length;
	};

	egg::Map<egg::String,Index> mIndex;
	int mDataOffset;
	egg::io::DataInputStream::ptr mInputStream;
	egg::io::MemoryInputStream::ptr mMemoryInputStream;
};

}
}

#endif
