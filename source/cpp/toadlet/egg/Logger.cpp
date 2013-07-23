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
#include <time.h>

// Choose output method
#if defined(TOADLET_PLATFORM_WIN32)
	#define TOADLET_USE_OUTPUTDEBUGSTRING_LOGGING
	#define TOADLET_USE_WINDOWS_CONSOLE
	#include <windows.h>
#endif

#if defined(TOADLET_PLATFORM_ANDROID)
	#define TOADLET_USE_ANDROID_LOGGING
	#include <android/log.h>
#endif

#if !defined(TOADLET_PLATFORM_WIN32)
	#define TOADLET_USE_STDERR_LOGGING
	#include <stdio.h>
#endif

namespace toadlet{
namespace egg{

Logger::Logger(){
	mLoggedMessage=false;
	mReportingLevel=Level_MAX;
	mOutputLogEntry=true;
	mStoreLogEntry=false;
}

Logger::~Logger(){
	#if defined(TOADLET_PLATFORM_OSX)
		while(mCategoryNameClientMap.begin()!=mCategoryNameClientMap.end()){
			aslclient client=mCategoryNameClientMap.begin()->second;
			asl_close(client);
			mCategoryNameClientMap.erase(mCategoryNameClientMap.begin());
		}
	#endif

	while(mCategoryNameCategoryMap.begin()!=mCategoryNameCategoryMap.end()){
		Category *category=mCategoryNameCategoryMap.begin()->second;
		delete category;
		mCategoryNameCategoryMap.erase(mCategoryNameCategoryMap.begin());
	}

	int i;
	for(i=0;i<mLogEntries.size();++i){
		delete mLogEntries[i];
	}
}

void Logger::setMasterReportingLevel(Level level){
	mReportingLevel=level;

	addLogEntry(Categories::TOADLET_EGG_LOGGER,Level_DISABLED,String("Master Reporting Level is ")+level);
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

void Logger::setOutputLogEntry(bool outputLogEntry){
	lock();
		mOutputLogEntry=outputLogEntry;
	unlock();
}

void Logger::setStoreLogEntry(bool storeLogEntry){
	lock();
		mStoreLogEntry=storeLogEntry;
	unlock();
}

void Logger::addLogEntry(const String &categoryName,Level level,const String &text){
	Category *category=getCategory(categoryName);
	lock();
		if((category==NULL || category->reportingLevel>=level) && mReportingLevel>=level){
			if(mLoggedMessage==false){
				mLoggedMessage=true;

				String line="creating "+Categories::TOADLET_EGG_LOGGER+":"+Version::STRING;
				
				int loggerLevel=Level_DISABLED;
				unlock();
					loggerLevel=getCategoryReportingLevel(Categories::TOADLET_EGG_LOGGER);
				lock();
				if(loggerLevel>Level_DISABLED){
					addCompleteLogEntry(NULL,Level_DISABLED,line);
				}
			}

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

void Logger::addCompleteLogEntry(Category *category,Level level,const String &text){
	int i;

	uint64 time=System::mtime();

	for(i=mLoggerListeners.size()-1;i>=0;--i){
		mLoggerListeners[i]->addLogEntry(category,level,time,text);
	}

	if(mOutputLogEntry){
		String timeString=System::mtimeToString(time);

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
		
		String line=String()+timeString+": "+levelString+text+(char)10;

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

		#if defined(TOADLET_USE_WINDOWS_CONSOLE)
			int textColor=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE;
			int levelColor=textColor;
			switch(level){
				case Level_ERROR:
					levelColor=FOREGROUND_RED;
				break;
				case Level_WARNING:
					levelColor=FOREGROUND_RED|FOREGROUND_GREEN;
				break;
				case Level_ALERT:
					levelColor=FOREGROUND_GREEN;
				break;
				default:
				break;
			}

			String stime=timeString+": ";
			String slevel=levelString;
			String stext=text+(char)10;

			HANDLE hout=GetStdHandle(STD_ERROR_HANDLE);
			WriteConsole(hout,(const TCHAR*)stime,stime.length(),NULL,NULL);
			SetConsoleTextAttribute(hout,levelColor);
			WriteConsole(hout,(const TCHAR*)slevel,slevel.length(),NULL,NULL);
			SetConsoleTextAttribute(hout,textColor);
			WriteConsole(hout,(const TCHAR*)stext,stext.length(),NULL,NULL);
		#endif

		#if defined(TOADLET_USE_ANDROID_LOGGING)
			int priority=0;
			switch(level){
				case Level_EXCESS:
					priority=ANDROID_LOG_VERBOSE;
				break;
				case Level_DEBUG:
					priority=ANDROID_LOG_DEBUG;
				break;
				case Level_ALERT:
					priority=ANDROID_LOG_INFO;
				break;
				case Level_WARNING:
					priority=ANDROID_LOG_WARN;
				break;
				case Level_ERROR:
					priority=ANDROID_LOG_ERROR;
				break;
				default:
				break;
			}

			__android_log_write(priority,category!=NULL?category->name:"toadlet",text);
		#endif

		#if defined(TOADLET_PLATFORM_OSX)
			aslclient client=NULL;
			if(category!=NULL){
				CategoryNameClientMap::iterator it=mCategoryNameClientMap.find(category->name);
				client=it!=mCategoryNameClientMap.end()?it->second:NULL;
			}

			int asllevel=ASL_LEVEL_NOTICE;
			switch(level){
				case Level_EXCESS:
					asllevel=ASL_LEVEL_DEBUG;
				break;
				case Level_DEBUG:
					asllevel=ASL_LEVEL_INFO;
				break;
				case Level_ALERT:
					asllevel=ASL_LEVEL_NOTICE;
				break;
				case Level_WARNING:
					asllevel=ASL_LEVEL_WARNING;
				break;
				case Level_ERROR:
					asllevel=ASL_LEVEL_ERR;
				break;
				default:
				break;
			}
		
			asl_log(client,NULL,asllevel,"%s",line.c_str());
		#endif

		#if defined(TOADLET_USE_STDERR_LOGGING)
			fputs(line,stderr);
		#endif
	}

	if(mStoreLogEntry){
		mLogEntries.add(new Entry(category,level,time,text));
	}
}

Logger::Category *Logger::addCategory(const String &categoryName){
	return getCategory(categoryName);
}

Logger::Category *Logger::getCategory(const String &categoryName){
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
			#if defined(TOADLET_PLATFORM_OSX)
				mCategoryNameClientMap.add(categoryName,asl_open(categoryName,categoryName,0));
			#endif
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
