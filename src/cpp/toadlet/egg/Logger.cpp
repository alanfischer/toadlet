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
#if defined(TOADLET_PLATFORM_WIN32)
	#include <windows.h>
#else
	#include <time.h>
#endif

// Choose output method
#if defined(TOADLET_PLATFORM_WIN32)
	#define TOADLET_USE_OUTPUTDEBUGSTRING_LOGGING
#endif

#if !defined(TOADLET_PLATFORM_WINCE)
	#define TOADLET_USE_STDERR_LOGGING
#endif

// Include appropriate files
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
	mStoreLogString=false;

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
	if(categoryName.wc_str()==NULL || data.wc_str()==NULL) return; /// @todo: This apparently happens when a GLTexture is being destroyed after everything else.  Need to look into it.

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
	    char timeString[128];
	    #if defined(TOADLET_PLATFORM_WIN32)
			SYSTEMTIME currentTime;
			GetLocalTime(&currentTime);
			sprintf(timeString,"%04d-%02d-%02d %02d:%02d:%02d :",currentTime.wYear,currentTime.wMonth,currentTime.wDay,currentTime.wHour,currentTime.wMinute,currentTime.wSecond);
		#else
			time_t currentTime;
			time(&currentTime);
			struct tm *ts=localtime(&currentTime);
			strftime(timeString,sizeof(timeString),"%Y-%m-%d %H:%M:%S :",ts);
		#endif

		const char *levelString=NULL;
		switch(level){
			case Level_DISABLED:
				levelString="LOGGER:  ";
			break;
			case Level_ERROR:
				levelString="ERROR:   ";
			break;
			case Level_WARNING:
				levelString="WARNING: ";
			break;
			case Level_ALERT:
				levelString="ALERT:   ";
			break;
			case Level_DEBUG:
				levelString="DEBUG:   ";
			break;
			case Level_EXCESS:
				levelString="EXCESS:  ";
			break;
			default:
				levelString="UNKNOWN: ";
			break;
		}
		
		String line=String()+timeString+levelString+data+(char)10;

		if(mOutputLogString){
			#if defined(TOADLET_USE_OUTPUTDEBUGSTRING_LOGGING)
				int len=line.length();
				// If we go above a certain amount, windows apparently just starts ignoring messages
				if(len>=8192){
					OutputDebugString(TEXT("WARNING: Excessive string length, may be truncated and near future messages dropped\n"));
				}
				int i=0;
				while(i<len){
					int newi=i+1023; // OutputDebugString truncates anything beyond 1023
					if(newi>len){
						newi=len;
					}
					OutputDebugString(line.substr(i,newi-i));
					i=newi;
				}
			#endif

			#if defined(TOADLET_USE_STDERR_LOGGING)
				fputs(line,stderr);
			#endif

//			if(level==Level_ERROR){
//				#if defined(TOADLET_PLATFORM_WIN32)
//					MessageBox(NULL,data,TEXT("Logger::error"),MB_OK);
//				#endif
//			}
		}

		if(mStoreLogString){
			mLogString+=line;
		}
	}
}

Logger::Category *Logger::addCategory(const String &categoryName){
	Category *category=new Category(categoryName);
	lock();
		mCategoryNameCategoryMap.add(categoryName,category);
	unlock();
	return category;
}

Logger::Category *Logger::getCategory(const String &categoryName){
	if(categoryName.wc_str()==NULL) return NULL; /// @todo: This apparently happens when a GLTexture is being destroyed after everything else.  Need to look into it.

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
