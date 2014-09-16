#ifndef TOADLET_EGG_SOSLOGGERLISTENER_H
#define TOADLET_EGG_SOSLOGGERLISTENER_H

#include "LoggerListener.h"

namespace toadlet{
namespace egg{

class SOSLoggerListener:public LoggerListener{
public:
	SOSLoggerListener(const char *serverAddress);
	virtual ~SOSLoggerListener();

	void addLogEntry(Logger::Category *category,Logger::Level level,Logger::timestamp time,const char *text);
	void flush();

	void run();

protected:
	void sendEntry(Logger::Category *category,Logger::Level level,Logger::timestamp time,const char *text);
	const char *getSOSLevelName(Logger::Level level);

	char *mServerAddress;
	char *mMessageBuffer;
	int mMessageBufferLength;
	Logger::List<Logger::Entry*> mEntries;
	int mSocket;
	void *mMutex;
	void *mCondition;
	void *mThread;
	bool mStop;
};

}
}

#endif
