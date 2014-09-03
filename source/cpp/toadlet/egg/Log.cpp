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

#include <toadlet/egg/Log.h>
#include <toadlet/egg/LoggerListener.h>
#include <toadlet/egg/System.h>
#include <time.h>
#include <stdio.h>

#if defined(TOADLET_PLATFORM_WIN32)
	#include <windows.h>
#elif defined(TOADLET_PLATFORM_OSX)
	#include <asl.h>
#elif defined(TOADLET_PLATFORM_ANDROID)
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

	String getTimeString(uint64 time){return System::mtimeToString(time);}

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
		mParent->addLogEntry(category==NULL?String():category->name,level,text);
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
		typedef Map<String,aslclient> CategoryNameClientMap;

		virtual ~ASLListener(){
			while(mCategoryNameClientMap.begin()!=mCategoryNameClientMap.end()){
				aslclient client=mCategoryNameClientMap.begin()->second;
				asl_close(client);
				mCategoryNameClientMap.erase(mCategoryNameClientMap.begin());
			}
		}

		void addLogEntry(Logger::Category *category,Logger::Level level,uint64 time,const char *text){
			aslclient client=NULL;
			if(category!=NULL){
				CategoryNameClientMap::iterator it=mCategoryNameClientMap.find(category->name);
				if(it!=mCategoryNameClientMap.end()){
					client=it->second;
				}
				else{
					client=asl_open(category->name,category->name,0);
					mCategoryNameClientMap.add(category->name,client);
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
		CategoryNameClientMap mCategoryNameClientMap;
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
Collection<LoggerListener*> Log::mListeners;
Map<int,Logger*> Log::mThreadLoggers;
bool Log::mPerThread=false;

Logger *Log::getInstance(){
	if(mTheLogger==NULL){
		initialize();
	}

	Logger *logger=mTheLogger;
	if(mPerThread){
		int thread=System::threadID();
		logger->lock();
			Logger *logger=mThreadLoggers[thread];
			if(logger==NULL){
				LoggerListener *listener=new ParentListener(mTheLogger);
				mListeners.add(listener);

				logger=new Logger(true);
				logger->addLoggerListener(listener);
				mThreadLoggers[thread]=logger;
			}
		logger->unlock();
	}
	return logger;
}

void Log::initialize(bool startSilent,bool perThread,const char *options){
	if(mTheLogger==NULL){
		mTheLogger=new Logger(startSilent);

		#if defined(TOADLET_PLATFORM_WIN32)
			mListeners.add(new ConsoleListener());
			mListeners.add(new OutputDebugStringListener());
		#elif defined(TOADLET_PLATFORM_OSX)
			mListeners.add(new StandardListener());
			mListeners.add(new ASLListener());
		#elif defined(TOADLET_PLATFORM_ANDROID)
			mListeners.add(new StandardListener());
			mListeners.add(new AndroidListener());
		#elif defined(TOADLET_PLATFORM_EMSCRIPTEN)
			mListeners.add(new StandardListener());
		#else
			mListeners.add(new ANSIStandardListener());
		#endif

		#if !defined(TOADLET_PLATFORM_EMSCRIPTEN)
			if(options!=NULL){
				mListeners.add(new SOSLoggerListener(options));
			}
		#endif

		int i;
		for(i=0;i<mListeners.size();++i){
			mTheLogger->addLoggerListener(mListeners[i]);
		}

		mPerThread=perThread;
	}
}

void Log::destroy(){
	if(mTheLogger!=NULL){
		int i;
		for(i=0;i<mListeners.size();++i){
			mTheLogger->removeLoggerListener(mListeners[i]);
			delete mListeners[i];
		}
		mListeners.clear();

		delete mTheLogger;
		mTheLogger=NULL;
	}
	for(Map<int,Logger*>::iterator it=mThreadLoggers.begin();it!=mThreadLoggers.end();++it){
		delete it->second;
	}
	mThreadLoggers.clear();
}

}
}
