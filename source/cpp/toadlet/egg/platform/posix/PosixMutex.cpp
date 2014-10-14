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

#include <toadlet/egg/platform/posix/PosixMutex.h>
#include <toadlet/egg/Categories.h>
#include <toadlet/egg/Error.h>

namespace toadlet{
namespace egg{

PosixMutex::PosixMutex()
	//mMutex
{
	pthread_mutexattr_t attrib;
	pthread_mutexattr_init(&attrib);
	pthread_mutexattr_settype(&attrib,PTHREAD_MUTEX_RECURSIVE);
	int result=pthread_mutex_init(&mMutex,&attrib);
	pthread_mutexattr_destroy(&attrib);
	if(result<0){
		Error::unknown(Categories::TOADLET_EGG,
			"error in pthread_mutex_init");
	}
}

PosixMutex::~PosixMutex(){
	pthread_mutex_destroy(&mMutex);
}

bool PosixMutex::tryLock(){
	return pthread_mutex_trylock(&mMutex)==0;
}

bool PosixMutex::lock(){
	return pthread_mutex_lock(&mMutex)==0;
}

bool PosixMutex::unlock(){
	return pthread_mutex_unlock(&mMutex)==0;
}

}
}

