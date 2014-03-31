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

#include <toadlet/egg/ThreadPool.h>
#include <toadlet/egg/Exception.h>

namespace toadlet{
namespace egg{

class TaskGroup{
public:
	TaskGroup();

	void addTask();
	void removeTask();

	void addException(const Exception &ex);
	Exception getException();

	void waitForAll();

protected:
	int mTaskCount;
	Mutex mMutex;
	WaitCondition mWait;
	Exception mException;
};

TaskGroup::TaskGroup():
	mTaskCount(0)
	//mMutex,
	//mWait
{}

void TaskGroup::removeTask(){
	mMutex.lock();

	mTaskCount--;
	mWait.notify();

	mMutex.unlock();
}

void TaskGroup::addTask(){
	mMutex.lock();

	mTaskCount++;

	mMutex.unlock();
}

void TaskGroup::addException(const Exception &ex){
	mMutex.lock();

	if(mException.getError()!=0){
		mException=ex;
	}

	mMutex.unlock();
}

Exception TaskGroup::getException(){
	Exception ex;

	mMutex.lock();

	ex=mException;

	mMutex.unlock();

	return ex;
}

void TaskGroup::waitForAll(){
	bool done=false;

	while(!done){
		mMutex.lock();

		if(mTaskCount==0){
			done=true;
		}
		else{
			mWait.wait(&mMutex);
		}

		mMutex.unlock();
	}
}


class TaskThread:public Thread{
public:
	TOADLET_OBJECT(TaskThread);
	
	TaskThread(ThreadPool *pool);
	
	void run();
	
protected:
	ThreadPool *mPool;
};

TaskThread::TaskThread(ThreadPool *pool):
	mPool(pool)
{}

void TaskThread::run(){
	while(mPool->running()){
		Pair<TaskGroup*,Runner::ptr> item=mPool->queue()->take();
		if(item.second){
			TOADLET_TRY
				item.second->run();
			TOADLET_CATCH(const Exception &ex){
				item.first->addException(ex);
			}
			item.first->removeTask();
		}
	}
}


ThreadPool::ThreadPool(int poolSize):
	mRunning(true)
	//mQueue,
	//mPool
{
	resizePool(poolSize);
}

ThreadPool::~ThreadPool() {
	mRunning=false;

	for(int i=0;i<mPool.size();++i){
		mQueue.add(Pair<TaskGroup*,Runner::ptr>(NULL,NULL)); // Add dummy tasks to force the pool threads to finish
	}

	for(int i=0;i<mPool.size();++i){
		mPool[i]->join();
	}
}

void ThreadPool::blockForTasks(const Collection<Runner::ptr> &tasks){
	Exception exception;

	if(mPool.size()==0){
		for(int i=0;i<tasks.size();++i){
			TOADLET_TRY
				tasks[i]->run();
			TOADLET_CATCH(const Exception &ex){
				exception=ex;
			}
		}
	}
	else{
		TaskGroup group;
	
		for(int i=0;i<tasks.size();++i){
			group.addTask();
			mQueue.add(Pair<TaskGroup*,Runner::ptr>(&group,tasks[i]));
		}
	
		group.waitForAll();

		exception=group.getException();
	}

	if(exception.getError()!=0){
		throw exception;
	}
}

void ThreadPool::update(){
	/// TODO: Handle issuing responses
}

void ThreadPool::resizePool(int poolSize){
	mPool.resize(poolSize);

	int i;
	for(i=0;i<mPool.size();++i){
		mPool[i]=new TaskThread(this);
		mPool[i]->start();
	}
}

}
}
