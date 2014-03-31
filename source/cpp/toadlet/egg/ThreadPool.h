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

#ifndef TOADLET_EGG_THREADPOOL_H
#define TOADLET_EGG_THREADPOOL_H

#include <toadlet/egg/Collection.h>
#include <toadlet/egg/Map.h>
#include <toadlet/egg/Object.h>
#include <toadlet/egg/Mutex.h>
#include <toadlet/egg/Thread.h>
#include <toadlet/egg/WaitCondition.h>

namespace toadlet{
namespace egg{

template<typename Type>
class TaskQueue:public Object{
public:
	TOADLET_OBJECT(TaskQueue);

	void add(const Type &type){
		mMutex.lock();
		{
			mCollection.add(type);
			mWait.notify();
		}
		mMutex.unlock();
	}
	
	Type take(){
		Type type;
		mMutex.lock();
		{
			if(mCollection.size()==0){
				mWait.wait(&mMutex);
			}
			if(mCollection.size()>0){
				type=mCollection.at(0);
				mCollection.removeAt(0);
			}
		}
		mMutex.unlock();
		return type;
	}

protected:
	Mutex mMutex;
	WaitCondition mWait;
	Collection<Type> mCollection;
};

class TaskGroup;

class TOADLET_API ThreadPool:public Object{
public:
	TOADLET_OBJECT(ThreadPool);

	ThreadPool(int poolSize);
	virtual ~ThreadPool();
	
	void update();

	void queueTasks(const Collection<Runner::ptr> &tasks);
	void blockForTasks(const Collection<Runner::ptr> &tasks);

	bool running() const{return mRunning;}

	TaskQueue<Pair<TaskGroup*,Runner::ptr> > *queue(){return &mQueue;}

protected:
	void resizePool(int size);

	bool mRunning;
	TaskQueue<Pair<TaskGroup*,Runner::ptr> > mQueue;
	Collection<Thread::ptr> mPool;
};

}
}

#endif
