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

#include <toadlet/egg/platform/win32/Win32Mutex.h>
#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>

namespace toadlet{
namespace egg{

Win32Mutex::Win32Mutex(){
	mMutex=CreateMutex(NULL,0,NULL);
}

Win32Mutex::~Win32Mutex(){
	CloseHandle(mMutex);
	mMutex=0;
}

bool Win32Mutex::tryLock(){
	int result=WaitForSingleObject(mMutex,0);
	return (result==WAIT_OBJECT_0);
}

bool Win32Mutex::lock(){
	int result=WaitForSingleObject(mMutex,INFINITE);
	return (result==WAIT_OBJECT_0);
}

bool Win32Mutex::unlock(){
 	int result=ReleaseMutex(mMutex);
	return (result==TRUE);
}

}
}

