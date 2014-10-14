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

#include <toadlet/egg/platform/posix/PosixWaitCondition.h>
#include <toadlet/egg/platform/posix/PosixMutex.h>
#include <toadlet/egg/Categories.h>
#include <toadlet/egg/Error.h>

namespace toadlet{
namespace egg{

PosixWaitCondition::PosixWaitCondition()
	//mCondition
{
	int result=pthread_cond_init(&mCondition,0);
	if(result<0){
		Error::unknown(Categories::TOADLET_EGG,
			"error in pthread_cond_init");
	}
}

PosixWaitCondition::~PosixWaitCondition(){
	pthread_cond_destroy(&mCondition);
}

bool PosixWaitCondition::wait(Mutex *mutex,int time){
	timespec ts;
	ts.tv_sec=time/1000;
	ts.tv_nsec=(time%1000)*1000;
	return pthread_cond_timedwait(&mCondition,&((PosixMutex*)mutex)->mMutex,&ts)==0;
}

bool PosixWaitCondition::wait(Mutex *mutex){
	return pthread_cond_wait(&mCondition,&((PosixMutex*)mutex)->mMutex)==0;
}

void PosixWaitCondition::notify(){
	pthread_cond_signal(&mCondition);
}

}
}

