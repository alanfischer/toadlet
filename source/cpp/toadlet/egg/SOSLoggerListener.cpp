#include "SOSLoggerListener.h"
#include <toadlet/egg/Log.h>
#include <toadlet/egg/Error.h>

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

SOSLoggerListener::SOSLoggerListener(String serverAddress):
	mStop(false),
	mTermination(0)
{
	// Turn off egg errors when in use, to avoid recursion
	Log::getInstance()->setCategoryReportingLevel(Categories::TOADLET_EGG,Logger::Level_DISABLED);

	mServerAddress=serverAddress;
	mMutex=Mutex::ptr(new Mutex());
	mCondition=WaitCondition::ptr(new WaitCondition());

	mSocket=Socket::createTCPSocket();

	mThread=new Thread(this);
	mThread->start();	
}

SOSLoggerListener::~SOSLoggerListener(){
	flush();
	
	mSocket->close();

	mStop=true;
	mThread->join();
}

void SOSLoggerListener::addLogEntry(Logger::Category *category,Logger::Level level,uint64 time,const char *text){
	mMutex->lock();
	mEntries.add(Logger::Entry(category,level,time,text));
	mMutex->unlock();
}

void SOSLoggerListener::flush(){
	mMutex->lock();
	mCondition->wait(mMutex);
	mMutex->unlock();
}

void SOSLoggerListener::sendEntry(Logger::Category *category,Logger::Level level,uint64 time,const char *text){
	if (level <= 0) {
		return;
	}
	
	String formattedMessage = String("!SOS<showMessage key='") + _sos_level_names[level] + ("'>");
	
	String messagePrefix = String("[") + _sos_level_names[level] + String("] ");
	
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
	bool result = mSocket->connect(mServerAddress,4444);
	if (result == false) {
		Error::unknown("SOSLoggerListener could not connect!");
		return;
	}

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
