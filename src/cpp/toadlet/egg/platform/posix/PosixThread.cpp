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

#include <toadlet/egg/platform/posix/PosixThread.h>
#include <toadlet/egg/Error.h>
#include <unistd.h>
#include <stdlib.h>
#if defined(TOADLET_PLATFORM_POSIX)
	#include <string.h>
#endif

namespace toadlet{
namespace egg{

PosixThread::PosixThread():
	mRunner(NULL),
	//mThread(NULL),
	mAlive(false)
{
	mRunner=this;
	memset(&mThread,0,sizeof(mThread));
}

PosixThread::PosixThread(Runnable *r):
	mRunner(NULL),
	//mThread(NULL),
	mAlive(false)
{
	mRunner=r;
	memset(&mThread,0,sizeof(mThread));
}

PosixThread::~PosixThread(){
	TOADLET_ASSERT(mAlive==false);
}

void PosixThread::start(){
	if(mAlive==false){
		mAlive=true;
		pthread_attr_t attrib;
		pthread_attr_init(&attrib);
#		if defined(TOADLET_PLATFORM_IRIX)
			pthread_attr_setscope(&attrib,PTHREAD_SCOPE_BOUND_NP);
#		else
			pthread_attr_setscope(&attrib,PTHREAD_SCOPE_SYSTEM);
#		endif
		int result=pthread_create(&mThread,&attrib,startThread,(void*)this);
		pthread_attr_destroy(&attrib);
		if(result<0){
			Error::unknown(Categories::TOADLET_EGG,
				"error in pthread_create");
		}
	}
}

void PosixThread::run(){
	if(mRunner!=NULL){
		mRunner->run();
	}
}

void PosixThread::startRun(){
	run();

	pthread_detach(mThread);
	mAlive=false;
}

void *PosixThread::startThread(void *thread){
	((PosixThread*)thread)->startRun();
	pthread_exit(NULL);
}

}
}

