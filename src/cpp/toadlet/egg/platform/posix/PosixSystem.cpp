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
#include <signal.h>
#include <setjmp.h>

namespace toadlet{
namespace egg{

// The ebx register is used by PIC to store the GOT on i386 systems
#if defined (__i386__) && defined(__PIC__)
	#define TOADLET_CPUID(r,infoType) \
		__asm__ __volatile__ ("xchg{l}\t{%%}ebx, %1\n\t" \
		"cpuid\n\t" \
		"xchg{l}\t{%%}ebx, %1\n\t": \
		"=a" (r[0]), "=r" (r[1]), "=c" (r[2]), "=d" (r[3]): "a" (infoType));
#else
	#define TOADLET_CPUID(r,infoType) \
		__asm__ __volatile__ ("cpuid": \
		"=a" (r[0]), "=b" (r[1]), "=c" (r[2]), "=d" (r[3]): "a" (infoType));
#endif

static sigjmp_buf jmpbuf;
static volatile sig_atomic_t doJump=0;

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
	testSSE(caps);

	testNEON(caps);

	return true;
}

bool PosixSystem::absolutePath(const String &path){
	int length=path.length();
	return	(length>0 && path.at(0)=='/');
}

String PosixSystem::getEnv(const String &name){
	return getenv(name);
}

void PosixSystem::testSSE(SystemCaps &caps){
	#if defined(TOADLET_HAS_SSE)
		int result[4];
		int infoType=1;
		TOADLET_CPUID(result,infoType);
		if	(result[2]&(1<<20))	caps.sseVersion=4; // SSE4.2
		else if	(result[2]&(1<<19))	caps.sseVersion=4; // SSE4.1
		else if	(result[2]&(1<<0))	caps.sseVersion=3; // SSE3
		else if	(result[3]&(1<<26))	caps.sseVersion=2; // SSE2
		else if	(result[3]&(1<<25))	caps.sseVersion=1; // SSE
		else caps.sseVersion=0;
	#endif
}

void PosixSystem::testNEON(SystemCaps &caps){
	#if defined(TOADLET_HAS_NEON)
		caps.neonVersion=0;
		static void (*lastSignal)(int);
		lastSignal=signal(SIGILL,signalHandler);
		if(sigsetjmp(jmpbuf,1)){
			signal(SIGILL,lastSignal);
			return;
		}
		doJump=1;
		asm(
			"vadd.i16 q0,q0,q0\n\t"
			"bx lr"
		);
		doJump=0;
		signal(SIGILL,lastSignal);
		caps.neonVersion=1;
	#endif
}

void PosixSystem::signalHandler(int s){
    if(!doJump){
        signal(s,SIG_DFL);
        raise(s);
    }

    doJump=0;
    siglongjmp(jmpbuf,1);
}

}
}
