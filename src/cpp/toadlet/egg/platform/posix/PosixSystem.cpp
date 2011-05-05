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

#include <toadlet/egg/platform/posix/PosixSystem.h>
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/sysctl.h> // sysctlbyname()

namespace toadlet{
namespace egg{

void PosixSystem::usleep(uint64 microseconds){
	::usleep(microseconds);
}

void PosixSystem::msleep(uint64 milliseconds){
	if(milliseconds>=1000){
		::sleep(milliseconds/1000);
	}

	::usleep((milliseconds%1000)*1000);
}

uint64 PosixSystem::utime(){
	struct timeval now;
	gettimeofday(&now,0);
	return now.tv_usec;
}

uint64 PosixSystem::mtime(){
	struct timeval now;
	gettimeofday(&now,0);
	return ((uint64)now.tv_sec)*1000+((uint64)(now.tv_usec))/1000;
}

String PosixSystem::mtimeToString(uint64 time){
	char timeString[128];
	time_t tt=time/=1000;
	struct tm *ts=gmtime(&tt);
	strftime(timeString,sizeof(timeString),"%Y-%m-%d %H:%M:%S :",ts);
	return timeString;
}

int PosixSystem::threadID(){
	return (intptr_t)(int*)(pthread_self());
}

bool PosixSystem::getSystemCaps(SystemCaps &caps){
	int result[4];
	int infoType = 1;
	// Thanks to Tyler Streeter for the following
	int yes=0;
	size_t s=sizeof(yes);
	caps.sseVersion=0;
	sysctlbyname("hw.optional.sse",&yes,&s,NULL,0);		if(yes) caps.sseVersion=1;
	sysctlbyname("hw.optional.sse2",&yes,&s,NULL,0);	if(yes) caps.sseVersion=2;
	sysctlbyname("hw.optional.sse3",&yes,&s, NULL,0);	if(yes) caps.sseVersion=3;
	sysctlbyname("hw.optional.supplementalsse3",&yes,&s,NULL,0); if(yes) caps.sseVersion=3;
	sysctlbyname("hw.optional.sse4_1",&yes,&s,NULL,0);	if(yes) caps.sseVersion=4;
	sysctlbyname("hw.optional.sse4_2",&yes,&s,NULL,0);	if(yes) caps.sseVersion=4;

	return true;
}

bool PosixSystem::absolutePath(const String &path){
	int length=path.length();
	return	(length>0 && path.at(0)=='/');
}

String PosixSystem::getEnv(const String &name){
	return getenv(name);
}

}
}
