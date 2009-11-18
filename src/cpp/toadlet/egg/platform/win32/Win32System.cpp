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

#include <toadlet/egg/platform/win32/Win32System.h>
#ifndef WIN32_LEAN_AND_MEAN
#	define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>

namespace toadlet{
namespace egg{

void Win32System::usleep(uint64 microseconds){
	// Poorly implemented
	Sleep(microseconds/1000);
}

void Win32System::msleep(uint64 milliseconds){
	Sleep(milliseconds);
}

uint64 Win32System::utime(){
	LARGE_INTEGER frequency,counter;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&counter);
	return counter.QuadPart/(frequency.QuadPart/1000000);
}

uint64 Win32System::mtime(){
	return GetTickCount();
}

bool Win32System::absolutePath(const String &path){
	int length=path.length();
	return	(length>3 && path.at(1)==':' && (path.at(2)=='\\' || path.at(2)=='/')) ||
			(length>2 && ((path.at(0)=='/' && path.at(1)=='/') || (path.at(0)=='\\' && path.at(1)=='\\')));
}

#if !defined(TOADLET_PLATFORM_WINCE)
	String Win32System::getEnv(const String &name){
		TCHAR result[1024];
		GetEnvironmentVariable(name,result,sizeof(result)/sizeof(TCHAR));
		return result;
	}
#endif

}
}
