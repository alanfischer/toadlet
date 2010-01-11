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

#ifndef TOADLET_EGG_IO_MEMORYSTREAM_H
#define TOADLET_EGG_IO_MEMORYSTREAM_H

#include <toadlet/egg/io/Stream.h>
#include <toadlet/egg/Collection.h>

namespace toadlet{
namespace egg{
namespace io{

// This does not properly support both reading & writing of the same stream
// TODO: Unit test me
class TOADLET_API MemoryStream:public Stream{
public:
	TOADLET_SHARED_POINTERS(MemoryStream);

	MemoryStream(char *data,int length,bool ownsData);
	MemoryStream(unsigned char *data,int length,bool ownsData);
	MemoryStream(Stream::ptr stream); // Read all data from stream
	MemoryStream();
	virtual ~MemoryStream();

	virtual void close(){}

	virtual bool isReadable(){return true;}
	virtual int read(char *buffer,int length);

	virtual bool isWriteable(){return true;}
	virtual int write(const char *buffer,int length);

	virtual bool reset();
	virtual int length(){return mLength;}
	virtual int position(){return mPosition;}
	virtual bool seek(int offs);

	char *getCurrentDataPointer();
	char *getOriginalDataPointer();

protected:
	char *mData;
	Collection<char> *mInternalData;
	int mLength;
	int mPosition;

	bool mOwnsData;
};

}
}
}

#endif

