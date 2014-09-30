#include "FileLoggerListener.h"
#include <time.h>

namespace toadlet{
namespace egg{

FileLoggerListener::FileLoggerListener(bool date,const char *directory,const char *prefix):
	mUseDate(date),
	mDateLogTime(0),
	mDateLogStream(NULL)
{
	mDirectory=new char[strlen(directory)+1];
	strcpy(mDirectory,directory);
	mPrefix=new char[strlen(prefix)+1];
	strcpy(mPrefix,prefix);
}

FileLoggerListener::~FileLoggerListener(){
	if(mDateLogStream!=NULL){
		fclose(mDateLogStream);
	}

	delete[] mDirectory;
	delete[] mPrefix;
}

void FileLoggerListener::addLogEntry(Logger::Category *category,Logger::Level level,Logger::timestamp logtime,const char *text){
	// Get the current date
	time_t currentTime = time(0);
	struct tm currentTm=*localtime(&currentTime);
	struct tm dateLogTm=*localtime(&mDateLogTime);
	if(mDateLogStream==NULL || (mUseDate && (currentTm.tm_year!=dateLogTm.tm_year || currentTm.tm_yday!=dateLogTm.tm_yday))){
		mDateLogTime=currentTime;

		char logName[1024];
		strcpy(logName,mDirectory);
		strcat(logName,"/");
		strcat(logName,mPrefix);
		
		if(mUseDate){
			char buf[96];
			strftime(buf,sizeof(buf),"_%Y-%m-%d",&currentTm);
			strcat(logName,buf);
		}

		strcat(logName,".log");

		const char *flags;
		if(mUseDate){
			flags="w+";
		}
		else{
			flags="w";
		}

		mDateLogStream=fopen(logName,flags);
	}

	if(mDateLogStream!=NULL){
		const char *timeString=getTimeString(logtime);
		const char *levelString=getLevelString(level);

		fputs(timeString,mDateLogStream);
		fputs(": ",mDateLogStream);
		fputs(levelString,mDateLogStream);
		fputs(text,mDateLogStream);
		fputs(newLine,mDateLogStream);
	}
}

void FileLoggerListener::flush(){
	if(mDateLogStream){
		fflush(mDateLogStream);
	}
}

}
}
