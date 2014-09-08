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

#include "Log.h"
#include "LoggerListener.h"
#include <toadlet/egg/String.h>
#include <time.h>
#include <stdio.h>

#if defined(TOADLET_PLATFORM_WIN32)
	#include <windows.h>
#else
	#include <pthread.h>
	#include <stdint.h>
#endif

#if defined(TOADLET_PLATFORM_OSX)
	#include <asl.h>
#endif

#if defined(TOADLET_PLATFORM_ANDROID)
	#include <android/log.h>
#endif

#if !defined(TOADLET_PLATFORM_EMSCRIPTEN)
	#include <toadlet/egg/SOSLoggerListener.h>
#endif

namespace toadlet{
namespace egg{

class BaseLoggerListener:public LoggerListener{
public:
	BaseLoggerListener(){}

	String getTimeString(uint64 time){
		char timeString[128];
		time_t tt=time/1000;
		struct tm *ts=gmtime(&tt);
		strftime(timeString,sizeof(timeString),"%Y-%m-%d %H:%M:%S",ts);
		return timeString;
	}

	const char *getLevelString(Logger::Level level){
		switch(level){
			case Logger::Level_DISABLED:
				return "LOGGER:  ";
			break;
			case Logger::Level_ERROR:
				return "ERROR:   ";
			break;
			case Logger::Level_WARNING:
				return "WARNING: ";
			break;
			case Logger::Level_ALERT:
				return "ALERT:   ";
			break;
			case Logger::Level_DEBUG:
				return "DEBUG:   ";
			break;
			case Logger::Level_EXCESS:
				return "EXCESS:  ";
			break;
			default:
				return "UNKNOWN: ";
			break;
		}
	}

	void flush(){}
};

class ParentListener:public BaseLoggerListener{
public:
	ParentListener(Logger *parent){
		mParent=parent;
	}

	void addLogEntry(Logger::Category *category,Logger::Level level,uint64 time,const char *text){
		mParent->addLogEntry(category==NULL?NULL:category->name,level,text);
	}

protected:
	Logger *mParent;
};

#if defined(TOADLET_PLATFORM_WIN32)
	class OutputDebugStringListener:public BaseLoggerListener{
	public:
		void addLogEntry(Logger::Category *category,Logger::Level level,uint64 time,const char *text){
			String timeString=getTimeString(time);
			String levelString=getLevelString(level);

			String line=String()+timeString+": "+levelString+text+(char)10;
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
		}
	};
	class ConsoleListener:public BaseLoggerListener{
	public:
		void addLogEntry(Logger::Category *category,Logger::Level level,uint64 time,const char *text){
			String timeString=getTimeString(time)+": ";
			String levelString=getLevelString(level);
			String textString=String()+text+(char)10;

			int textColor=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE;
			int levelColor=textColor;
			HANDLE outHandle=GetStdHandle(STD_OUTPUT_HANDLE);
			switch(level){
				case Logger::Level_ERROR:
					levelColor=FOREGROUND_RED;
					outHandle=GetStdHandle(STD_ERROR_HANDLE);
				break;
				case Logger::Level_WARNING:
					levelColor=FOREGROUND_RED|FOREGROUND_GREEN;
				break;
				case Logger::Level_ALERT:
					levelColor=FOREGROUND_GREEN;
				break;
				default:
				break;
			}

			WriteConsole(outHandle,(const TCHAR*)timeString,timeString.length(),NULL,NULL);
			SetConsoleTextAttribute(outHandle,levelColor);
			WriteConsole(outHandle,(const TCHAR*)levelString,levelString.length(),NULL,NULL);
			SetConsoleTextAttribute(outHandle,textColor);
			WriteConsole(outHandle,(const TCHAR*)textString,textString.length(),NULL,NULL);
		}
	};
#elif defined(TOADLET_PLATFORM_OSX)
	class ASLListener:public BaseLoggerListener{
	public:
		virtual ~ASLListener(){
			for(Logger::List<aslclient>::iterator it=mClients.begin();it!=mClients.end();++it){
				asl_close(*it);
			}
		}

		void addLogEntry(Logger::Category *category,Logger::Level level,uint64 time,const char *text){
			aslclient client=NULL;
			if(category!=NULL){
				if(category->data==NULL){
					client=asl_open(category->name,category->name,0);
					category->data=client;
					mClients.push_back(client);
				}
				else{
					client=(aslclient)category->data;
				}
			}

			String timeString=getTimeString(time);
			String levelString=getLevelString(level);

			int asllevel=ASL_LEVEL_NOTICE;
			switch(level){
				case Logger::Level_ERROR:
					asllevel=ASL_LEVEL_ERR;
				break;
				case Logger::Level_WARNING:
					asllevel=ASL_LEVEL_WARNING;
				break;
				case Logger::Level_ALERT:
					asllevel=ASL_LEVEL_NOTICE;
				break;
				case Logger::Level_DEBUG:
					asllevel=ASL_LEVEL_INFO;
				break;
				case Logger::Level_EXCESS:
					asllevel=ASL_LEVEL_DEBUG;
				break;
				default:
				break;
			}
		
			String line=String()+timeString+": "+levelString+text+(char)10;
			asl_log(client,NULL,asllevel,"%s",line.c_str());
		}

	protected:
		Logger::List<aslclient> mClients;
	};
#elif defined(TOADLET_PLATFORM_ANDROID)
	class AndroidListener:public BaseLoggerListener{
	public:
		void addLogEntry(Logger::Category *category,Logger::Level level,uint64 time,const char *text){
			int priority=0;
			switch(level){
				case Logger::Level_ERROR:
					priority=ANDROID_LOG_ERROR;
				break;
				case Logger::Level_WARNING:
					priority=ANDROID_LOG_WARN;
				break;
				case Logger::Level_ALERT:
					priority=ANDROID_LOG_INFO;
				break;
				case Logger::Level_DEBUG:
					priority=ANDROID_LOG_DEBUG;
				break;
				case Logger::Level_EXCESS:
					priority=ANDROID_LOG_VERBOSE;
				break;
				default:
				break;
			}

			__android_log_write(priority,category!=NULL?category->name:"toadlet",text);
		}
	};
#endif

class ANSIStandardListener:public BaseLoggerListener{
public:
	void addLogEntry(Logger::Category *category,Logger::Level level,uint64 time,const char *text){
		String timeString=getTimeString(time)+": ";
		String levelString=getLevelString(level);
		String textString=String()+text+(char)10;

		String textColor="\x1b[39m";
		String levelColor=textColor;
		FILE *outFile=stdout;
		switch(level){
			case Logger::Level_ERROR:
				levelColor="\x1b[31m";
				outFile=stderr;
			break;
			case Logger::Level_WARNING:
				levelColor="\x1b[33m";
			break;
			case Logger::Level_ALERT:
				levelColor="\x1b[32m";
			break;
			default:
			break;
		}

		fputs(timeString,outFile);
		fputs(levelColor,outFile);
		fputs(levelString,outFile);
		fputs(textColor,outFile);
		fputs(textString,outFile);
	}
};

class StandardListener:public BaseLoggerListener{
public:
	void addLogEntry(Logger::Category *category,Logger::Level level,uint64 time,const char *text){
		String timeString=getTimeString(time)+": ";
		String levelString=getLevelString(level);
		String textString=String()+text+(char)10;

		FILE *outFile=stdout;
		if(level==Logger::Level_ERROR){
			outFile=stderr;
		}

		fputs(timeString,outFile);
		fputs(levelString,outFile);
		fputs(textString,outFile);
	}
};

Logger *Log::mTheLogger=NULL;
bool Log::mPerThread=false;
Logger::List<LoggerListener*> Log::mListeners;
Logger::List<Logger*> Log::mThreadLoggers;

Logger *Log::getInstance(){
	if(mTheLogger==NULL){
		initialize();
	}

	Logger *logger=mTheLogger;
	if(mPerThread){
		int id=threadID();
		mTheLogger->lock();
			Logger::List<Logger*>::iterator it=mThreadLoggers.begin();
			while(it!=mThreadLoggers.end() && (*it)->getThreadID()==id){
				++it;
			}
			if(it!=mThreadLoggers.end()){
				logger=(*it);
			}
			else{
				LoggerListener *listener=new ParentListener(mTheLogger);
				mListeners.push_back(listener);

				logger=new Logger(true);
				logger->setThreadID(id);
				logger->addLoggerListener(listener);
				mThreadLoggers.push_back(logger);
			}
		mTheLogger->unlock();
	}
	return logger;
}

void Log::initialize(bool startSilent,bool perThread,const char *options){
	mPerThread=perThread;

	if(mTheLogger==NULL){
		mTheLogger=new Logger(startSilent);

		#if defined(TOADLET_PLATFORM_WIN32)
			mListeners.push_back(new ConsoleListener());
			mListeners.push_back(new OutputDebugStringListener());
		#elif defined(TOADLET_PLATFORM_OSX)
			mListeners.push_back(new StandardListener());
			mListeners.push_back(new ASLListener());
		#elif defined(TOADLET_PLATFORM_ANDROID)
			mListeners.push_back(new StandardListener());
			mListeners.push_back(new AndroidListener());
		#elif defined(TOADLET_PLATFORM_EMSCRIPTEN)
			mListeners.push_back(new StandardListener());
		#else
			mListeners.push_back(new ANSIStandardListener());
		#endif

		#if !defined(TOADLET_PLATFORM_EMSCRIPTEN)
			if(options!=NULL){
				mListeners.push_back(new SOSLoggerListener(options));
			}
		#endif

		for(Logger::List<LoggerListener*>::iterator it=mListeners.begin();it!=mListeners.end();++it){
			mTheLogger->addLoggerListener(it);
		}
	}
}

void Log::destroy(){
	if(mTheLogger!=NULL){
		for(Logger::List<LoggerListener*>::iterator it=mListeners.begin();it!=mListeners.end();++it){
			mTheLogger->removeLoggerListener(it);
			delete it;
		}
		mListeners.clear();

		delete mTheLogger;
		mTheLogger=NULL;
	}
	for(Logger::List<Logger*>::iterator it=mThreadLoggers.begin();it!=mThreadLoggers.end();++it){
		delete it;
	}
	mThreadLoggers.clear();
}

int Log::threadID(){
	#if defined(TOADLET_PLATFORM_WIN32)
		return GetCurrentThreadId();
	#else
		return (intptr_t)(int*)(pthread_self());
	#endif
}

}
}
