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

#include <stdio.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/LoggerListener.h>
#include <toadlet/egg/System.h>
#include <toadlet/egg/Version.h>

#if !defined(TOADLET_THREADSAFE) 
#elif defined(TOADLET_PLATFORM_WIN32)
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN 1
	#endif
	#include <windows.h>
#else
	#include <pthread.h>
#endif

namespace toadlet{
namespace egg{

Logger::Logger(bool startSilent):
	mReportingLevel(Level_MAX),
	mStoreLogEntry(false)
{
	#if !defined(TOADLET_THREADSAFE) 
	#elif defined(TOADLET_PLATFORM_WIN32)
		mMutex=CreateMutex(NULL,0,NULL);
	#else
		mMutex=new pthread_mutex_t();
		pthread_mutexattr_t attrib;
		pthread_mutexattr_init(&attrib);
		pthread_mutexattr_settype(&attrib,PTHREAD_MUTEX_RECURSIVE);
		int result=pthread_mutex_init((pthread_mutex_t*)mMutex,&attrib);
		pthread_mutexattr_destroy(&attrib);
	#endif

	if(startSilent==false){
		char line[128];
		sprintf(line,"creating toadlet.egg.Logger:%s",Version::STRING);
		addCompleteLogEntry(NULL,Level_DISABLED,line);
	}
}

Logger::~Logger(){
	int i;
	for(i=0;i<mCategories.size();++i){
		delete mCategories[i];
	}
	for(i=0;i<mLogEntries.size();++i){
		delete mLogEntries[i];
	}

	#if !defined(TOADLET_THREADSAFE)
	#elif defined(TOADLET_PLATFORM_WIN32)
		CloseHandle(mMutex);
	#else
		pthread_mutex_destroy((pthread_mutex_t*)mMutex);
		delete (pthread_mutex_t*)mMutex;
	#endif
}

void Logger::setMasterReportingLevel(Level level){
	mReportingLevel=level;

	lock();
		char line[128];
		sprintf(line,"Master Reporting Level is %d",level);
		addCompleteLogEntry(NULL,Level_DISABLED,line);
	unlock();
}

void Logger::setCategoryReportingLevel(const char *categoryName,Level level){
	Category *category=getCategory(categoryName);

	if(category!=NULL){
		lock();
			category->reportingLevel=level;
		unlock();
	}
}

Logger::Level Logger::getCategoryReportingLevel(const char *categoryName){
	Level reportingLevel=Level_MAX;

	const Category *category=getCategory(categoryName);
	lock();
		if(category!=NULL){
			reportingLevel=category->reportingLevel;
		}
	unlock();

	return reportingLevel;
}

Logger::Level Logger::getMasterCategoryReportingLevel(const char *categoryName){
	Logger::Level level=getCategoryReportingLevel(categoryName);
	if(level>mReportingLevel){
		level=mReportingLevel;
	}
	return level;
}

void Logger::addLoggerListener(LoggerListener *listener){
	lock();
		mLoggerListeners.add(listener);
	unlock();
}

void Logger::removeLoggerListener(LoggerListener *listener){
	lock();
		mLoggerListeners.remove(listener);
	unlock();
}

void Logger::setStoreLogEntry(bool storeLogEntry){
	lock();
		mStoreLogEntry=storeLogEntry;
	unlock();
}

void Logger::addLogEntry(Category *category,Level level,const char *text){
	lock();
		if((category==NULL || category->reportingLevel>=level) && mReportingLevel>=level){
			addCompleteLogEntry(category,level,text);
		}
	unlock();
}

void Logger::flush(){
	lock();
		int i;
		for(i=mLoggerListeners.size()-1;i>=0;--i){
			mLoggerListeners[i]->flush();
		}
	unlock();
}

void Logger::clearLogEntries(){
	lock();
		int i;
		for(i=0;i<mLogEntries.size();++i){
			delete mLogEntries[i];
		}
		mLogEntries.clear();
	unlock();
}

int Logger::getNumLogEntries(){
	int size=0;
	lock();
		size=mLogEntries.size();
	unlock();
	return size;
}

Logger::Entry *Logger::getLogEntry(int i){
	Entry *entry=NULL;
	lock();
		entry=mLogEntries[i];
	unlock();
	return entry;
}

void Logger::addCompleteLogEntry(Category *category,Level level,const char *text){
	int i;

	uint64 time=System::mtime();

	for(i=mLoggerListeners.size()-1;i>=0;--i){
		mLoggerListeners[i]->addLogEntry(category,level,time,text);
	}

	if(mStoreLogEntry){
		mLogEntries.add(new Entry(category,level,time,text));
	}
}

Logger::Category *Logger::addCategory(const char *categoryName){
	return getCategory(categoryName);
}

Logger::Category *Logger::getCategory(const char *categoryName){
	if(categoryName==(char*)NULL) return NULL;

	Category *category=NULL;
	lock();
		int i;
		for(i=0;i<mCategories.size();++i){
			if(mCategories[i]->name==categoryName){
				category=mCategories[i];
				break;
			}
		}
		if(category==NULL){
			category=new Category(categoryName);
			mCategories.add(category);
		}
	unlock();

	return category;
}

void Logger::lock(){
	#if !defined(TOADLET_THREADSAFE)
	#elif defined(TOADLET_PLATFORM_WIN32)
		WaitForSingleObject(mMutex,INFINITE);
	#else
		pthread_mutex_lock((pthread_mutex_t*)mMutex);
	#endif
}

void Logger::unlock(){
	#if !defined(TOADLET_THREADSAFE)
	#elif defined(TOADLET_PLATFORM_WIN32)
		ReleaseMutex(mMutex);
	#else
		pthread_mutex_unlock((pthread_mutex_t*)mMutex);
	#endif
}

}
}
