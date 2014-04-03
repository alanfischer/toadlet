#ifndef TOADLET_EGG_FILELOGGERLISTENER_H
#define TOADLET_EGG_FILELOGGERLISTENER_H

#include <toadlet/egg/LoggerListener.h>
#include <toadlet/egg/io/FileStream.h>

namespace toadlet{
namespace egg{

class TOADLET_API FileLoggerListener:public LoggerListener{
public:
	FileLoggerListener(bool date,const String &directory,const String &prefix);
	virtual ~FileLoggerListener();

	void addLogEntry(Logger::Category *category,Logger::Level level,uint64 logtime,const char *text);
	void flush();

protected:
	bool mUseDate;
	String mDirectory;
	String mPrefix;
	time_t mDateLogTime;
	FileStream::ptr mDateLogStream;
};

}
}

#endif