#ifndef TOADLET_EGG_SOSLOGGERLISTENER_H
#define TOADLET_EGG_SOSLOGGERLISTENER_H

#include <toadlet/egg/Logger.h>
#include <toadlet/egg/LoggerListener.h>
#include <toadlet/egg/Thread.h>
#include <toadlet/egg/Mutex.h>
#include <toadlet/egg/WaitCondition.h>
#include <toadlet/egg/Socket.h>

namespace toadlet{
namespace egg{

class SOSLoggerListener:public LoggerListener,public Runnable{
public:
	SOSLoggerListener(const char *serverAddress);
	virtual ~SOSLoggerListener();

	void addLogEntry(Logger::Category *category,Logger::Level level,uint64 time,const char *text);

	void flush();

	void run();

protected:
	void sendEntry(Logger::Category *category,Logger::Level level,uint64 time,const char *text);

	const char *getSOSLevelName(Logger::Level level);

	char *mServerAddress;
	char *mMessageBuffer;
	int mMessageBufferLength;
	Logger::List<Logger::Entry*> mEntries;
	Socket::ptr mSocket;
	Mutex::ptr mMutex;
	WaitCondition::ptr mCondition;
	Thread::ptr mThread;
	bool mStop;
};

}
}

#endif
