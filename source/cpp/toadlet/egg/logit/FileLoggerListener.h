#ifndef LOGIT_FILELOGGERLISTENER_H
#define LOGIT_FILELOGGERLISTENER_H

#include "BaseLoggerListener.h"
#include <stdio.h>

namespace logit{

class LOGIT_API FileLoggerListener:public BaseLoggerListener{
public:
	FileLoggerListener(bool date,const char *directory,const char *prefix);
	virtual ~FileLoggerListener();

	void addLogEntry(Logger::Category *category,Logger::Level level,Logger::timestamp logtime,const char *text);
	void flush();

protected:
	bool mUseDate;
	char *mDirectory;
	char *mPrefix;
	time_t mDateLogTime;
	FILE *mDateLogStream;
};

}

#endif