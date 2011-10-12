#ifndef TOADLET_EGG_HTTPLOGGERLISTENER_H
#define TOADLET_EGG_HTTPLOGGERLISTENER_H

#include <toadlet/egg/String.h>
#include <toadlet/egg/Thread.h>
#include <toadlet/egg/Mutex.h>
#include <toadlet/egg/WaitCondition.h>
#include <toadlet/egg/io/Socket.h>

class HTTPLoggerListener:public LoggerListener,public Runnable{
public:
	HTTPLoggerListener(String server,String path);
	virtual ~HTTPLoggerListener();

	void addLogEntry(Logger::Category *category,Logger::Level level,uint64 time,const String &text);

	void flush();

	void sendEntry(Logger::Category *category,Logger::Level level,uint64 time,const String &text)

	void run();

protected:
	String urlEncode(const String &s);

	String char2hex(char dec);

	String mServer;
	String mPath;
	Socket::ptr mSocket;
	Collection<Logger::Entry> mEntries;
	Mutex::ptr mMutex;
	WaitCondition::ptr mCondition;
	Thread::ptr mThread;
	bool mStop;
};

#endif
