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

#include <toadlet/egg/Logger.h>
#include <toadlet/egg/LoggerListener.h>
#include <toadlet/egg/System.h>
#include <toadlet/egg/Version.h>

namespace toadlet{
namespace egg{

Logger::Logger(bool startSilent):
	mStoreLogEntry(false),
	mReportingLevel(Level_MAX)
{
	if(startSilent==false){
		String line=String("creating toadlet.egg.Logger:")+Version::STRING;
		addCompleteLogEntry(NULL,Level_DISABLED,line);
	}
}

Logger::~Logger(){
	while(mCategoryNameCategoryMap.begin()!=mCategoryNameCategoryMap.end()){
		Category *category=mCategoryNameCategoryMap.begin()->second;
		delete category;
		mCategoryNameCategoryMap.erase(mCategoryNameCategoryMap.begin());
	}

	clearLogEntries();
}

void Logger::setMasterReportingLevel(Level level){
	mReportingLevel=level;

	lock();
		addCompleteLogEntry(NULL,Level_DISABLED,String("Master Reporting Level is ")+level);
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
		CategoryNameCategoryMap::iterator it=mCategoryNameCategoryMap.find(categoryName);
		if(it!=mCategoryNameCategoryMap.end()){
			category=it->second;
		}
		if(category==NULL){
			category=new Category(categoryName);
			mCategoryNameCategoryMap.add(categoryName,category);
		}
	unlock();

	return category;
}

void Logger::lock(){
	#if defined(TOADLET_THREADSAFE)
		mMutex.lock();
	#endif
}

void Logger::unlock(){
	#if defined(TOADLET_THREADSAFE)
		mMutex.unlock();
	#endif
}

}
}
