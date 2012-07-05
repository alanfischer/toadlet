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
	#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#include <stdio.h>
#include <intrin.h> // __cpuid()
#include <rpc.h>
#pragma comment(lib,"rpcrt4.lib")

namespace toadlet{
namespace egg{

const uint64 Win32System::DELTA_EPOC_MICROSECONDS=TOADLET_MAKE_UINT64(11644473600000000);

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
	FILETIME fileTime;
	GetSystemTimeAsFileTime(&fileTime);
	uint64 time=(*(uint64*)&fileTime);
	time=(time/10-DELTA_EPOC_MICROSECONDS)/1000;
	return time;
}

String Win32System::mtimeToString(uint64 time){
	char timeString[128];
	FILETIME fileTime;
	SYSTEMTIME systemTime;
	time=(time*1000+DELTA_EPOC_MICROSECONDS)*10;
	fileTime=(*(FILETIME*)&time);
	FileTimeToSystemTime(&fileTime,&systemTime);
	sprintf(timeString,"%04d-%02d-%02d %02d:%02d:%02d :",systemTime.wYear,systemTime.wMonth,systemTime.wDay,systemTime.wHour,systemTime.wMinute,systemTime.wSecond);
	return timeString;
}

int Win32System::threadID(){
	return GetCurrentThreadId();
}

bool Win32System::getSystemCaps(SystemCaps &caps){
	testSSE(caps);

	return true;
}

bool Win32System::absolutePath(const String &path){
	int length=path.length();
	return	(length>3 && path.at(1)==':' && (path.at(2)=='\\' || path.at(2)=='/')) ||
			(length>2 && ((path.at(0)=='/' && path.at(1)=='/') || (path.at(0)=='\\' && path.at(1)=='\\')));
}

String Win32System::getEnv(const String &name){
	TCHAR result[1024];
	GetEnvironmentVariable(name,result,sizeof(result)/sizeof(TCHAR));
	return result;
}

UUID Win32System::generateUUID(){
	::UUID uuid;
	UuidCreate(&uuid);
	return UUID((tbyte*)&uuid);
}

void Win32System::testSSE(SystemCaps &caps){
	int result[4];
	int infoType = 1;
	// Thanks to Tyler Streeter for the following
	__cpuid(result,infoType);
	if		(result[2]&(1<<20))	caps.sseVersion=4; // SSE4.2
	else if	(result[2]&(1<<19))	caps.sseVersion=4; // SSE4.1
	else if	(result[2]&(1<<0))	caps.sseVersion=3; // SSE3
	else if	(result[3]&(1<<26))	caps.sseVersion=2; // SSE2
	else if	(result[3]&(1<<25))	caps.sseVersion=1; // SSE
	else caps.sseVersion=0;
}

}
}
