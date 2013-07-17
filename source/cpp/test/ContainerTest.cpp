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

#include "ContainerTest.h"

bool ContainerTest::testCircularBuffer(){
	CircularBuffer<int> buffer(5);

	if(buffer.empty()==false || buffer.full()==true){
		Log::alert("buffer.empty or full error");
		return false;
	}

	buffer.push_back(0);
	buffer.push_back(1);
	buffer.push_back(2);
	buffer.push_back(3);
	buffer.push_back(4);

	if(buffer.empty()==true || buffer.full()==false){
		Log::alert("buffer.empty or full error");
		return false;
	}

	if(buffer.front()!=0){
		Log::alert("buffer.front not 0");
		return false;
	}

	if(buffer.back()!=4){
		Log::alert("buffer.back not 4");
		return false;
	}

	buffer.push_back(5);

	if(buffer.front()!=1){
		Log::alert("buffer.front not 1");
		return false;
	}

	return true;
}

bool ContainerTest::run(){
	bool result=false;

	Log::alert("Testing CircularBuffers");
	result=testCircularBuffer();
	Log::alert(result?"Succeeded":"Failed");

	return result;
}
