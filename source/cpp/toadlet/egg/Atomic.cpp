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

#include <toadlet/egg/Atomic.h>

#if defined(TOADLET_THREADSAFE) && defined(TOADLET_PLATFORM_WIN32)
	#define TOADLET_COUNT_WIN32
#elif defined(TOADLET_THREADSAFE) && defined(TOADLET_PLATFORM_OSX)
	#define TOADLET_COUNT_OSX
#elif defined(TOADLET_THREADSAFE) && defined(TOADLET_COMPILER_GCC)
	#define TOADLET_COUNT_GCC
#elif defined(TOADLET_THREADSAFE)
	#define TOADLET_COUNT_MUTEX
#endif

#if defined(TOADLET_COUNT_WIN32)
	#include <windows.h>
#elif defined(TOADLET_COUNT_OSX)
	#include <libkern/OSAtomic.h>
#elif defined(TOADLET_COUNT_MUTEX)
	#include <toadlet/egg/Mutex.h>
#endif

namespace toadlet{
namespace egg{

#if defined(TOADLET_COUNT_WIN32)

int Atomic::increment(int &value,void *data){
	return InterlockedIncrement((LONG*)&value);
}

int Atomic::decrement(int &value,void *data){
	return InterlockedDecrement((LONG*)&value);
}

#elif defined(TOADLET_COUNT_OSX)

int Atomic::increment(int &value,void *data){
	return OSAtomicAdd32(1,(int32*)&value);
}
	
int Atomic::decrement(int &value,void *data){
	return OSAtomicAdd32(-1,(int32*)&value);
}

#elif defined(TOADLET_COUNT_GCC)

int Atomic::increment(int &value,void *data){
	return __sync_fetch_and_add(&value,1) + 1;
}
	
int Atomic::decrement(int &value,void *data){
	return __sync_fetch_and_sub(&value,1) - 1;
}

#elif defined(TOADLET_COUNT_MUTEX)

int Atomic::increment(int &value,void *data){
	int count=0;
	((Mutex*)data)->lock();
		count=++value;
	((Mutex*)data)->unlock();
	return count;
}

int Atomic::decrement(int &value,void *data){
	int count=0;
	((Mutex*)data)->lock();
		count=--value;
	((Mutex*)data)->unlock();
	return count;
}

#else

int Atomic::increment(int &value,void *data){
	return ++value;
}

int Atomic::decrement(int &value,void *data){
	return --value;
}

#endif

}
}
