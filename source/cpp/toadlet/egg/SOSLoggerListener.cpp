#include "SOSLoggerListener.h"

#include <stdio.h>

namespace toadlet{
namespace egg{
	
const char *const _sos_level_names[] = {
	"Off",
	"Error",
	"Warning",
	"Info",
	"Debug",
	"Trace",
	"Trace"
};

SOSLoggerListener::SOSLoggerListener(String serverAddress){
	// Turn off net errors when in use, to avoid recursion
	Logger::getInstance()->setCategoryReportingLevel(Categories::TOADLET_EGG_NET,Logger::Level_DISABLED);

	mServerAddress=serverAddress;
	mMutex=Mutex::ptr(new Mutex());
	mCondition=WaitCondition::ptr(new WaitCondition());

	mStop=false;
	mThread=Thread::ptr(new Thread(this));
	mThread->start();
	
	mTermination = 0;
	
	mSocket = Socket::ptr(Socket::createTCPSocket());
	
	bool result = mSocket->connect(mServerAddress,4444);
	
	if (result == false) {
		printf("SOSLoggerListener could not connect!\n");
	}
}

SOSLoggerListener::~SOSLoggerListener(){
	flush();
	
	mSocket->close();

	mStop=true;
	mThread->join();
}

void SOSLoggerListener::addLogEntry(Logger::Category *category,Logger::Level level,uint64 time,const String &text){
	mMutex->lock();
	mEntries.add(Logger::Entry(category,level,time,text));
	mMutex->unlock();
}

void SOSLoggerListener::flush(){
	mMutex->lock();
	mCondition->wait(mMutex);
	mMutex->unlock();
}

void SOSLoggerListener::sendEntry(Logger::Category *category,Logger::Level level,uint64 time,const String &text){
	if (mSocket->connected() == false || level <= 0) {
		return;
	}
	
	String formattedMessage = String("!SOS<showMessage key='") + _sos_level_names[level] + ("'>");
	
	String messagePrefix = String();
	messagePrefix += String("[") + _sos_level_names[level] + String("] ");
	
	if (category != NULL) {
		// Occasionally, we use __FILE__ as the category name. If we do, we don't
		// care about the full file path. Just the file name.
		if (category->name.startsWith("/")) { // Posix file paths
			int index = category->name.rfind('/');
			messagePrefix += category->name.substr(index + 1, category->name.length() - index - 1);
		}
		else if (category->name.find('\\') > -1) { // Windows file paths
			int index = category->name.rfind('\\');
			messagePrefix += category->name.substr(index + 1, category->name.length() - index - 1);
		}
		else {
			messagePrefix += category->name;
		}
		
		messagePrefix += String(": ");
	}
	
	formattedMessage += messagePrefix + text;
	
	formattedMessage += String("</showMessage>");
	
	mSocket->send((tbyte*)formattedMessage.c_str(), formattedMessage.length());
	mSocket->send(&mTermination, 1);
}

void SOSLoggerListener::run(){
	Logger::Entry entry;
	bool hasEntry=false;

	while(mStop==false){
		mMutex->lock();
		if(mEntries.size()>0){
			entry=mEntries.at(0);
			mEntries.removeAt(0);
			hasEntry=true;
		}
		else{
			mCondition->notify();
			hasEntry=false;
		}
		mMutex->unlock();

		if(hasEntry){
			sendEntry(entry.category,entry.level,entry.time,entry.text);
		}
	}
}

}
}
