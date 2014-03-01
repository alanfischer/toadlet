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
#include <toadlet/egg/Atomic.h>

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

int Object::retain(){
	return Atomic::increment(mSharedCount,mSharedData);
}

int Object::release(){
	int count=Atomic::decrement(mSharedCount,mSharedData);
	if(count<=0){
		destroy();
		delete this;
	}
	return count;
}

int Object::releaseNoDestroy(){
	int count=Atomic::decrement(mSharedCount,mSharedData);
	if(count<=0){
		delete this;
	}
	return count;
}

}
}
