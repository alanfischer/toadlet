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

#include "Logger.h"
#include "LoggerListener.h"
#include "Log.h"
#include <stdio.h>

namespace logit{

Logger::Logger(bool startSilent):
	mReportingLevel(Level_MAX),
	mStoreLogEntry(false),
	mThread(NULL),
	mMutex(NULL)
{
	mMutex=Log::createMutex();

	if(startSilent==false){
		addCompleteLogEntry(NULL,Level_DISABLED,"creating logit.Logger");
	}
}

Logger::~Logger(){
	for(List<Category*>::iterator it=mCategories.begin();it!=mCategories.end();++it){
		delete it;
	}
	for(List<Entry*>::iterator it=mLogEntries.begin();it!=mLogEntries.end();++it){
		delete it;
	}

	Log::destroyMutex(mMutex);
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
		mLoggerListeners.push_back(listener);
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
		for(List<LoggerListener*>::iterator it=mLoggerListeners.begin();it!=mLoggerListeners.end();++it){
			(*it)->flush();
		}
	unlock();
}

void Logger::clearLogEntries(){
	lock();
		for(List<Entry*>::iterator it=mLogEntries.begin();it!=mLogEntries.end();++it){
			delete it;
		}
		mLogEntries.clear();
	unlock();
}

void Logger::addCompleteLogEntry(Category *category,Level level,const char *text){
	timestamp time=Log::mtime();

	for(List<LoggerListener*>::iterator it=mLoggerListeners.begin();it!=mLoggerListeners.end();++it){
		(*it)->addLogEntry(category,level,time,text);
	}

	if(mStoreLogEntry){
		mLogEntries.push_back(new Entry(category,level,time,text));
	}
}

Logger::Category *Logger::getCategory(const char *categoryName){
	if(categoryName==(char*)NULL) return NULL;

	Category *category=NULL;
	lock();
		for(List<Category*>::iterator it=mCategories.begin();it!=mCategories.end();++it){
			if(strcmp((*it)->name,categoryName)==0){
				category=it;
				break;
			}
		}
		if(category==NULL){
			category=new Category(categoryName);
			mCategories.push_back(category);
		}
	unlock();

	return category;
}

void Logger::lock(){
	Log::lock(mMutex);
}

void Logger::unlock(){
	Log::unlock(mMutex);
}

}
