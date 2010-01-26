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

#include <toadlet/egg/platform/win32/Win32Thread.h>
#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>

namespace toadlet{
namespace egg{

unsigned long WINAPI startThread(void *thread){
	((Win32Thread*)thread)->internal_startRun();
	return 0;
}

Win32Thread::Win32Thread():
	mRunner(NULL),
	mThread(0)
{}

Win32Thread::Win32Thread(Runnable *r):
	mRunner(NULL),
	mThread(0)
{
	mRunner=r;
}

Win32Thread::~Win32Thread(){
	TOADLET_ASSERT(mThread==0);
}

void Win32Thread::start(){
	if(mThread==0){
		mThread=CreateThread(NULL,0,&startThread,this,0,0);
		SetThreadPriority(mThread,THREAD_PRIORITY_NORMAL);
		ResumeThread(mThread);
	}
}

bool Win32Thread::join(){
	return join(INFINITE);
}

bool Win32Thread::join(uint64 milliseconds){
	if(mThread!=0 && WaitForSingleObject(mThread,milliseconds)==WAIT_OBJECT_0){
		return true;
	}
	else{
		return false;
	}
}

void Win32Thread::run(){
	if(mRunner!=NULL){
		mRunner->run();
	}
}

void Win32Thread::internal_startRun(){
	run();

	if(mThread!=0){
		CloseHandle(mThread);
		mThread=0;
	}
}

}
}

