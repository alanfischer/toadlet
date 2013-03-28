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

#ifndef TOADLET_EGG_OBJECT_H
#define TOADLET_EGG_OBJECT_H

#include <toadlet/egg/Interface.h>
#if defined(TOADLET_THREADSAFE)
	#include <toadlet/egg/Mutex.h>
#endif

namespace toadlet{
namespace egg{

class TOADLET_API Object:public Interface{
public:
	static void *operator new(size_t size);
	static void operator delete(void *p);

	Object():mSharedCount(0){}
	virtual ~Object(){}

	virtual int retain(){
		int count=0;
		#if defined(TOADLET_THREADSAFE)
			mSharedMutex.lock();
		#endif
		count=++mSharedCount;
		#if defined(TOADLET_THREADSAFE)
			mSharedMutex.unlock();
		#endif
		return count;
	}
	
	virtual int release(){
		int count=0;
		#if defined(TOADLET_THREADSAFE)
			mSharedMutex.lock();
		#endif
		count=--mSharedCount;
		#if defined(TOADLET_THREADSAFE)
			mSharedMutex.unlock();
		#endif

		if(count<=0){
			destroy();
			delete this;
		}
		return count;
	}

	virtual void destroy(){}

protected:
	int mSharedCount;
	#if defined(TOADLET_THREADSAFE)
		Mutex mSharedMutex;
	#endif
};

// The same as DefaultIntrusiveSemantics, but casts to an Object so we can still reference forward declared items
class ObjectSemantics{
public:
	template<typename Type> static int retainReference(Type *type){return ((Object*)type)->retain();}
	template<typename Type> static int releaseReference(Type *type){return ((Object*)type)->release();}
};

}
}

#define TOADLET_OBJECT(Class)\
	TOADLET_INTERFACE(Class); \
	virtual int retain(){return toadlet::egg::Object::retain();} \
	virtual int release(){return toadlet::egg::Object::release();}

#endif

