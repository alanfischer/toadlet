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

namespace toadlet{
namespace egg{

PosixThread::PosixThread():
	mRunnable(NULL)
	//mRunner,
	//mThread(NULL),
	//mAlive
{
	memset(&mThread,0,sizeof(mThread));
	mRunnable=this;
}

PosixThread::PosixThread(Runnable *r):
	//mSelf,
	mRunnable(NULL)
	//mRunner,
	//mThread(NULL),
	//mAlive
{
	memset(&mThread,0,sizeof(mThread));
	mRunnable=r;
}

PosixThread::PosixThread(Runner *r):
	//mSelf,
	mRunnable(NULL),
	mRunner(r)
	//mThread(NULL),
	//mAlive
{
	memset(&mThread,0,sizeof(mThread));
	mRunnable=r;
}

PosixThread::~PosixThread(){
	pthread_t empty;
	memset(&empty,0,sizeof(empty));
	if(memcmp(&mThread,&empty,sizeof(mThread))!=0){
		pthread_detach(mThread);
	}
}

void PosixThread::start(){
	if(mAlive==NULL){
		mAlive=this;		
		pthread_attr_t attrib;
		pthread_attr_init(&attrib);
		#if defined(TOADLET_PLATFORM_IRIX)
			pthread_attr_setscope(&attrib,PTHREAD_SCOPE_BOUND_NP);
		#else
			pthread_attr_setscope(&attrib,PTHREAD_SCOPE_SYSTEM);
		#endif
		int result=pthread_create(&mThread,&attrib,startThread,(void*)this);
		pthread_attr_destroy(&attrib);
		if(result<0){
			Error::unknown(Categories::TOADLET_EGG,
				"error in pthread_create");
		}
	}
}

bool PosixThread::join(){
	if(pthread_join(mThread,NULL)==0){
		memset(&mThread,0,sizeof(mThread));
		return true;
	}
	else{
		return false;
	}
}

void PosixThread::run(){
	if(mRunnable!=NULL){
		mRunnable->run();
	}
}

void PosixThread::startRun(){
	run();

	mRunnable=NULL;
	mRunner=NULL;
	mAlive=NULL;
}

void *PosixThread::startThread(void *thread){
	((PosixThread*)thread)->startRun();
	pthread_exit(NULL);
}

}
}

