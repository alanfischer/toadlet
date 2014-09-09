#ifndef TOADLET_EGG_FILELOGGERLISTENER_H
#define TOADLET_EGG_FILELOGGERLISTENER_H

#include "BaseLoggerListener.h"
#include <stdio.h>

namespace toadlet{
namespace egg{

class TOADLET_API FileLoggerListener:public BaseLoggerListener{
public:
	FileLoggerListener(bool date,const char *directory,const char *prefix);
	virtual ~FileLoggerListener();

	void addLogEntry(Logger::Category *category,Logger::Level level,uint64 logtime,const char *text);
	void flush();

protected:
	bool mUseDate;
	char *mDirectory;
	char *mPrefix;
	time_t mDateLogTime;
	FILE *mDateLogStream;
};

}
}

#endif