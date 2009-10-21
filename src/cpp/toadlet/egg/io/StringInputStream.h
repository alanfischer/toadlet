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

#ifndef TOADLET_EGG_IO_STRINGINPUTSTREAMS_H
#define TOADLET_EGG_IO_STRINGINPUTSTREAMS_H

#include <toadlet/egg/io/InputStream.h>
#include <toadlet/egg/String.h>

namespace toadlet{
namespace egg{
namespace io{

class TOADLET_API StringInputStream:public InputStream{
public:
	TOADLET_SHARED_POINTERS(StringInputStream,InputStream);

	StringInputStream(const String &string);
	virtual ~StringInputStream();

	void setString(const String &string);
	
	int read(char *buffer,int length);
	bool reset();
	bool seek(int offs);
	int available();
	void close();

protected:
	String mString;
	int mIndex;
};

}
}
}

#endif

