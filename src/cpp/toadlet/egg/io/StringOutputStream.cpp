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

#include <toadlet/egg/io/StringOutputStream.h>
#include <string.h> // memcpy

namespace toadlet{
namespace egg{
namespace io{

StringOutputStream::StringOutputStream(){
}

StringOutputStream::~StringOutputStream(){
}

const String &StringOutputStream::getString(){
	return mString;
}

int StringOutputStream::write(const char *data,int length){
	int index=0;
	const int bufferSize=3;
	char buffer[bufferSize+1];

	while(index<length){
		int size=length-index;
		if(size>bufferSize){
			size=bufferSize;
		}

		memcpy(buffer,data+index,size);
		buffer[size]=0;

		mString+=buffer;

		index+=size;
	}

	return index;
}

void StringOutputStream::close(){
}

}
}
}

