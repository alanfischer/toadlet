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

#ifndef TOADLET_TADPOLE_ZIPSTREAM_H
#define TOADLET_TADPOLE_ZIPSTREAM_H

#include <toadlet/egg/io/Stream.h>
#include <toadlet/tadpole/Types.h>

namespace toadlet{
namespace tadpole{

/// @todo: Ideally the zlib usage would be replaced with zziplib, but zziplib does not support writing
class TOADLET_API ZIPStream:public Object,public Stream{
public:
	TOADLET_OBJECT(ZIPStream);

	enum OpenFlags{
		OpenFlags_UNKNOWN=	0,
		OpenFlags_READ=		1<<0,
		OpenFlags_WRITE=	1<<1,
	};

	// Constructor to open a stream that compresses or decompresses data to a parent stream
	ZIPStream(Stream::ptr stream,int openFlags=0);

	// Constructor to open a ZZIP_FILE in a ZZIP_DIR
	ZIPStream(void *dir,const String &name);

	virtual ~ZIPStream();

	void close();
	bool closed();

	bool readable(){return mStream!=NULL?mStream->readable():true;}
	int read(tbyte *buffer,int length);

	bool writeable(){return mStream!=NULL?mStream->writeable():false;}
	int write(const tbyte *buffer,int length);

	bool reset();
	int length();
	int position();
	bool seek(int offs);

protected:
	int mOpenFlags;
	Stream::ptr mStream;
	void *mZStream;
	void *mFile;
};

}
}

#endif
