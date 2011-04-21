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

#ifndef TOADLET_EGG_IO_FILESTREAM_H
#define TOADLET_EGG_IO_FILESTREAM_H

#include <toadlet/egg/io/Stream.h>
#include <toadlet/egg/String.h>
#include <stdio.h>

namespace toadlet{
namespace egg{
namespace io{

class TOADLET_API FileStream:public Stream{
public:
	TOADLET_SHARED_POINTERS(FileStream);

	enum Open{
		Open_BIT_UNKNOWN=	0,
		Open_BIT_READ=		1<<0,
		Open_BIT_WRITE=		1<<1,
		Open_BIT_CREATE=	1<<2,
		Open_BIT_BINARY=	1<<3,
		Open_READ_BINARY=	Open_BIT_READ|Open_BIT_BINARY,
		Open_WRITE_BINARY=	Open_BIT_WRITE|Open_BIT_BINARY,
		Open_READ_WRITE_BINARY=Open_BIT_READ|Open_BIT_WRITE|Open_BIT_BINARY,
	};
	
	FileStream(const String &filename,int open);
	virtual ~FileStream();

	virtual void close();
	virtual bool closed(){return mFile==NULL;}

	virtual bool readable(){return (mOpen&Open_BIT_READ)!=0;}
	virtual int read(tbyte *buffer,int length);

	virtual bool writeable(){return (mOpen&Open_BIT_WRITE)!=0;}
	virtual int write(const tbyte *buffer,int length);

	virtual bool reset();
	virtual int length();
	virtual int position();
	virtual bool seek(int offs);

	void setFILE(FILE *file){mFile=file;}
	FILE *getFILE(){return mFile;}

protected:
	int mOpen;
	FILE *mFile;
	bool mAutoClose;
};

}
}
}

#endif

