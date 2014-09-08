#include "FileLoggerListener.h"
#include <toadlet/egg/Categories.h>
#include <toadlet/egg/Log.h>
#include <toadlet/egg/System.h>

#include <time.h>

namespace toadlet{
namespace egg{

FileLoggerListener::FileLoggerListener(bool date,const String &directory,const String &prefix):
	mUseDate(date),
	mDateLogTime(0)
{
	// Turn off egg errors when in use, to avoid recursion
	Log::getInstance()->setCategoryReportingLevel(Categories::TOADLET_EGG,Logger::Level_DISABLED);

	mDirectory=directory;
	mPrefix=prefix;
}

FileLoggerListener::~FileLoggerListener(){
}

void FileLoggerListener::addLogEntry(Logger::Category *category,Logger::Level level,uint64 logtime,const char *text){
	// Get the current date
	time_t currentTime = time(0);
	struct tm currentTm=*localtime(&currentTime);
	struct tm dateLogTm=*localtime(&mDateLogTime);
	if(mDateLogStream==NULL || (mUseDate && (currentTm.tm_year!=dateLogTm.tm_year || currentTm.tm_yday!=dateLogTm.tm_yday))){
		mDateLogTime=currentTime;

		char buf[96];
		strftime(buf,sizeof(buf),"_%Y-%m-%d",&currentTm);

		// Append the date to the log file name, preserving any given suffix
		String logName(mDirectory+"/"+mPrefix+".log");

		if(mUseDate){
			int pos=logName.rfind('.');
			if(pos!=-1){
				String suf=logName.substr(pos,-1);
				logName=logName.substr(0,pos);
				logName += buf;
				logName += suf;
			}
			else{
				logName += buf;
				logName += ".log";
			}
		}

		int flags=FileStream::Open_BIT_WRITE;
		
		if(mUseDate){
			flags|=FileStream::Open_BIT_APPEND;
		}

		mDateLogStream=new FileStream(logName,flags);
		if(mDateLogStream->writeable()==false){
			mDateLogStream=NULL;
		}
	}

	if(mDateLogStream!=NULL){
		String slevel;
		switch(level){
			case Logger::Level_ERROR:
				slevel="ERROR:  ";
			break;
			case Logger::Level_WARNING:
				slevel="WARNING:";
			break;
			case Logger::Level_ALERT:
				slevel="ALERT:  ";
			break;
			case Logger::Level_DEBUG:
				slevel="DEBUG:  ";
			break;
			case Logger::Level_EXCESS:
				slevel="EXCESS: ";
			break;
			default:
				slevel="        ";
			break;
		}

		String line=System::mtimeToString(logtime)+":"+slevel+text+"\n";

		mDateLogStream->write((tbyte*)line.c_str(),line.length());
	}
}

void FileLoggerListener::flush(){
	if(mDateLogStream){
		mDateLogStream->flush();
	}
}

}
}
