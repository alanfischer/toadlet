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

#include <toadlet/egg/io/StringInputStream.h>

namespace toadlet{
namespace egg{
namespace io{

StringInputStream::StringInputStream(const String &string){
	mString=string;
	mIndex=0;
}

StringInputStream::~StringInputStream(){
}

void StringInputStream::setString(const String &string){
	mString=string;
	mIndex=0;
}

int StringInputStream::read(char *buffer,int length){
	int i;
	for(i=0;i<length && mIndex<mString.length();++i,++mIndex){
		buffer[i]=mString[mIndex];
	}

	return i;
}

bool StringInputStream::reset(){
	mIndex=0;
	return true;
}

bool StringInputStream::seek(int offs){
	mIndex=offs;
	return true;
}

int StringInputStream::available(){
	return mString.length()-mIndex;
}

void StringInputStream::close(){
}

}
}
}

