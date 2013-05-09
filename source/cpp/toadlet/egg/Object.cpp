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

#include <toadlet/egg/Object.h>

#if defined(TOADLET_THREADSAFE) && defined(TOADLET_PLATFORM_WIN32)
	#define TOADLET_COUNT_WIN32
#elif defined(TOADLET_THREADSAFE) && defined(TOADLET_PLATFORM_OSX)
	#define TOADLET_COUNT_OSX
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

// mSharedCount will always be aligned on TOADLET_ALIGNED_SIZE, due to the Object's new operator

namespace toadlet{
namespace egg{

void *Object::operator new(size_t size){
	return TOADLET_ALIGNED_MALLOC(size,TOADLET_ALIGNED_SIZE);
}

void Object::operator delete(void *p){
	TOADLET_ALIGNED_FREE(p);
}

Object::Object():
	mSharedCount(0),
	mSharedData(NULL)
{
	#if defined(TOADLET_COUNT_MUTEX)
		mSharedData=new Mutex();
	#endif
}

Object::~Object(){
	#if defined(TOADLET_COUNT_MUTEX)
		delete (Mutex*)mSharedData;
	#endif
}

#if defined(TOADLET_COUNT_WIN32)

int Object::retain(){
	int count=InterlockedIncrement((LONG*)&mSharedCount);
	return count;
}

int Object::release(){
	int count=InterlockedDecrement((LONG*)&mSharedCount);
	if(count<=0){
		destroy();
		delete this;
	}
	return count;
}
	
#elif defined(TOADLET_COUNT_OSX)
	
int Object::retain(){
	int count=OSAtomicIncrement32((int32*)&mSharedCount);
	return count;
}
	
int Object::release(){
	int count=OSAtomicDecrement32((int32*)&mSharedCount);
	if(count<=0){
		destroy();
		delete this;
	}
	return count;
}

#elif defined(TOADLET_COUNT_MUTEX)

int Object::retain(){
	int count=0;
	((Mutex*)mSharedData)->lock();
		count=++mSharedCount;
	((Mutex*)mSharedData)->unlock();
	return count;
}

int Object::release(){
	int count=0;
	((Mutex*)mSharedData)->lock();
		count=--mSharedCount;
	((Mutex*)mSharedData)->unlock();
	if(count<=0){
		destroy();
		delete this;
	}
	return count;
}

#else

int Object::retain(){
	int count=++mSharedCount;
	return count;
}

int Object::release(){
	int count=--mSharedCount;
	if(count<=0){
		destroy();
		delete this;
	}
	return count;
}

#endif



}
}
