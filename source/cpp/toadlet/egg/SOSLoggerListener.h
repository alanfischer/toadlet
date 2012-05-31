#ifndef TOADLET_EGG_SOSLOGGERLISTENER_H
#define TOADLET_EGG_SOSLOGGERLISTENER_H

#include <toadlet/egg/Logger.h>
#include <toadlet/egg/LoggerListener.h>
#include <toadlet/egg/Thread.h>
#include <toadlet/egg/Mutex.h>
#include <toadlet/egg/WaitCondition.h>
#include <toadlet/egg/net/Socket.h>

namespace toadlet{
namespace egg{

class SOSLoggerListener:public LoggerListener,public Runnable{
public:
	SOSLoggerListener(String serverAddress);
	virtual ~SOSLoggerListener();

	void addLogEntry(Logger::Category *category,Logger::Level level,uint64 time,const String &text);

	void flush();

	void sendEntry(Logger::Category *category,Logger::Level level,uint64 time,const String &text);

	void run();

protected:
	String mServerAddress;
	Socket::ptr mSocket;
	Collection<Logger::Entry> mEntries;
	Mutex::ptr mMutex;
	WaitCondition::ptr mCondition;
	Thread::ptr mThread;
	bool mStop;
	tbyte mTermination;
};

}
}

#endif
