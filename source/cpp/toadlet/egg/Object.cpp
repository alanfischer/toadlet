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

#if defined(TOADLET_COUNT_MUTEX)
	#include <toadlet/egg/Mutex.h>
#elif defined(TOADLET_COUNT_WIN32)
	#include <windows.h>
#elif defined(TOADLET_COUNT_OSX)
	#include <libkern/OSAtomic.h>
#endif

namespace toadlet{
namespace egg{

#if defined(TOADLET_COUNT_MUTEX)
	class SharedData{
	public:
		SharedData():count(0){}

		int count;
		Mutex mutex;
	};
#endif

void *Object::operator new(size_t size){
	return TOADLET_ALIGNED_MALLOC(size,TOADLET_ALIGNED_SIZE);
}

void Object::operator delete(void *p){
	TOADLET_ALIGNED_FREE(p);
}

Object::Object():
	mSharedData(NULL)
{
	#if defined(TOADLET_COUNT_MUTEX)
		mSharedData=new SharedData();
	#elif defined(TOADLET_COUNT_WIN32) || defined(TOADLET_COUNT_OSX)
		mSharedData=TOADLET_ALIGNED_MALLOC(sizeof(int32),TOADLET_ALIGNED_SIZE);
		*(int32*)mSharedData=0;
	#else
		mSharedData=new int(0);
	#endif
}

Object::~Object(){
	#if defined(TOADLET_COUNT_MUTEX)
		delete mSharedData;
	#elif defined(TOADLET_COUNT_WIN32) || defined(TOADLET_COUNT_OSX)
		TOADLET_ALIGNED_FREE(mSharedData);
	#else
		delete mSharedData;
	#endif
}

#if defined(TOADLET_COUNT_MUTEX)

int Object::retain(){
	int count=0;
	((SharedData*)mSharedData)->mutex.lock();
		count=++((SharedData*)mSharedData)->count;
	((SharedData*)mSharedData)->mutex.unlock();
	return count;
}

int Object::release(){
	int count=0;
	((SharedData*)mSharedData)->mutex.lock();
		count=--((SharedData*)mSharedData)->count;
	((SharedData*)mSharedData)->mutex.unlock();
	if(count<=0){
		destroy();
		delete this;
	}
	return count;
}

#elif defined(TOADLET_COUNT_WIN32)

int Object::retain(){
	int count=InterlockedIncrement((int32*)mSharedData);
	return count;
}

int Object::release(){
	int count=InterlockedDecrement((int32*)mSharedData);
	if(count<=0){
		destroy();
		delete this;
	}
	return count;
}
	
#elif defined(TOADLET_COUNT_OSX)
	
int Object::retain(){
	int count=OSAtomicIncrement32((int32*)mSharedData);
	return count;
}
	
int Object::release(){
	int count=OSAtomicDecrement32((int32*)mSharedData);
	if(count<=0){
		destroy();
		delete this;
	}
	return count;
}

#else

int Object::retain(){
	int count=++*(int*)mSharedData;
	return count;
}

int Object::release(){
	int count=--*(int*)mSharedData;
	if(count<=0){
		destroy();
		delete this;
	}
	return count;
}

#endif



}
}
