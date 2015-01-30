#include "FileLoggerListener.h"
#include <time.h>

namespace logit{

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

	const char *textColor="\x1b[39m";
	const char *levelColor=textColor;
	switch(level){
		case Logger::Level_ERROR:
			levelColor="\x1b[31m";
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

	if(mDateLogStream!=NULL){
		const char *timeString=getTimeString(logtime);
		const char *levelString=getLevelString(level);

		fputs(timeString,mDateLogStream);
		fputs(": ",mDateLogStream);
		fputs(levelColor,mDateLogStream);
		fputs(levelString,mDateLogStream);
		fputs(textColor,mDateLogStream);
		fputs(": ",mDateLogStream);
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
