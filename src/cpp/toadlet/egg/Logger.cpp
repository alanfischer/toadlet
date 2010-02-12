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

// Choose output method
#if defined(TOADLET_PLATFORM_WIN32)
	#define TOADLET_USE_OUTPUTDEBUGSTRING_LOGGING
#endif

#if !defined(TOADLET_PLATFORM_WINCE)
	#define TOADLET_USE_STDERR_LOGGING
#endif	

// Include appropriate files
#if defined(TOADLET_USE_OUTPUTDEBUGSTRING_LOGGING)
	#include <windows.h>
#endif

#if defined(TOADLET_USE_STDERR_LOGGING)
	#include <stdio.h>
#endif

namespace toadlet{
namespace egg{

Logger *Logger::mTheLogger=NULL;

Logger *Logger::getInstance(){
	if(mTheLogger==NULL){
		mTheLogger=new Logger();
	}

	return mTheLogger;
}

void Logger::destroy(){
	if(mTheLogger!=NULL){
		delete mTheLogger;
		mTheLogger=NULL;
	}
}

Logger::Logger(){
	mLoggedMessage=false;
	mReportingLevel=Level_MAX;
	mOutputLogString=true;
	mStoreLogString=true;

	addCategory(Categories::TOADLET_EGG_LOGGER);
	addCategory(Categories::TOADLET_EGG_NET);
	setCategoryReportingLevel(Categories::TOADLET_EGG_NET,Logger::Level_DISABLED); // Don't log socket errors
	addCategory(Categories::TOADLET_EGG);
	addCategory(Categories::TOADLET_FLICK);
	addCategory(Categories::TOADLET_HOP);
	addCategory(Categories::TOADLET_KNOT);
	addCategory(Categories::TOADLET_PEEPER);
	addCategory(Categories::TOADLET_RIBBIT);
	addCategory(Categories::TOADLET_TADPOLE);
	addCategory(Categories::TOADLET_PAD);
}

Logger::~Logger(){
	while(mCategoryNameCategoryMap.begin()!=mCategoryNameCategoryMap.end()){
		Category *category=mCategoryNameCategoryMap.begin()->second;
		delete category;
		mCategoryNameCategoryMap.erase(mCategoryNameCategoryMap.begin());
	}
}

void Logger::setMasterReportingLevel(Level level){
	mReportingLevel=level;

	addLogString(Categories::TOADLET_EGG_LOGGER,Level_DISABLED,String("Master Reporting Level is ")+level);
}

void Logger::setCategoryReportingLevel(const String &categoryName,Level level){
	Category *category=getCategory(categoryName);

	if(category!=NULL){
		lock();
			category->reportingLevel=level;
		unlock();
	}
}

Logger::Level Logger::getCategoryReportingLevel(const String &categoryName){
	Level reportingLevel=Level_MAX;

	const Category *category=getCategory(categoryName);
	lock();
		if(category!=NULL){
			reportingLevel=category->reportingLevel;
		}
	unlock();

	return reportingLevel;
}

Logger::Level Logger::getMasterCategoryReportingLevel(const String &categoryName){
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

void Logger::setOutputLogString(bool outputLogString){
	lock();
		mOutputLogString=outputLogString;
	unlock();
}

void Logger::setStoreLogString(bool storeLogString){
	lock();
		mStoreLogString=storeLogString;
	unlock();
}

void Logger::addLogString(const String &categoryName,Level level,const String &data){
	Category *category=getCategory(categoryName);
	lock();
		if((category==NULL || category->reportingLevel>=level) && mReportingLevel>=level){
			if(mLoggedMessage==false){
				mLoggedMessage=true;

				String line="Started toadlet::egg::Logger";
				
				int loggerLevel=Level_DISABLED;
				unlock();
					loggerLevel=getCategoryReportingLevel(Categories::TOADLET_EGG_LOGGER);
				lock();
				if(loggerLevel>Level_DISABLED){
					addCompleteLogString(NULL,Level_DISABLED,line);
				}
			}

			addCompleteLogString(category,level,data);
		}
	unlock();
}

void Logger::addLogString(Level level,const String &string){
	addLogString((char*)NULL,level,string);
}

String Logger::getLogString(){
	String string;

	lock();
		string=mLogString;
	unlock();

	return string;
}

void Logger::addCompleteLogString(Category *category,Level level,const String &data){
	int i;

	for(i=mLoggerListeners.size()-1;i>=0;--i){
		mLoggerListeners[i]->addLogString(category,level,data);
	}

	if(mOutputLogString || mStoreLogString){
		String prefix;

		switch(level){
			case Level_DISABLED:
				prefix="LOGGER:  ";
			break;
			case Level_ERROR:
				prefix="ERROR:   ";
			break;
			case Level_WARNING:
				prefix="WARNING: ";
			break;
			case Level_ALERT:
				prefix="ALERT:   ";
			break;
			case Level_DEBUG:
				prefix="DEBUG:   ";
			break;
			case Level_EXCESS:
				prefix="EXCESS:  ";
			break;
			default:
				prefix="UNKNOWN: ";
			break;
		}

		String line=prefix+data+(char)10;

		if(mOutputLogString){
			#if defined(TOADLET_USE_OUTPUTDEBUGSTRING_LOGGING)
				int len=line.length();
				// If we go above a certain amount, windows apparently just starts ignoring messages
				if(len>=8192){
					OutputDebugStringW(L"WARNING: Excessive string length, may be truncated and near future messages dropped\n");
				}
				int i=0;
				while(i<len){
					int newi=i+1023; // OutputDebugString truncates anything beyond 1023
					if(newi>len){
						newi=len;
					}
					OutputDebugStringW(line.substr(i,newi-i));
					i=newi;
				}
			#endif

			#if defined(TOADLET_USE_STDERR_LOGGING)
				fprintf(stderr,line);
			#endif
		}

		if(mStoreLogString){
			mLogString+=line;
		}
	}
}

Logger::Category *Logger::addCategory(const String &categoryName){
	Category *category=new Category(categoryName);
	lock();
		mCategoryNameCategoryMap[categoryName]=category;
	unlock();
	return category;
}

Logger::Category *Logger::getCategory(const String &categoryName){
	Category *category=NULL;
	lock();
		CategoryNameCategoryMap::iterator it=mCategoryNameCategoryMap.find(categoryName);
		if(it!=mCategoryNameCategoryMap.end()){
			category=it->second;
		}
	unlock();

	return category;
}

void Logger::lock(){
	#if !defined(TOADLET_NO_THREADS)
		mMutex.lock();
	#endif
}

void Logger::unlock(){
	#if !defined(TOADLET_NO_THREADS)
		mMutex.unlock();
	#endif
}

}
}
