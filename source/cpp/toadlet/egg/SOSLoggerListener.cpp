#include "SOSLoggerListener.h"
#include <stdio.h>

namespace toadlet{
namespace egg{

SOSLoggerListener::SOSLoggerListener(const char *serverAddress):
	mServerAddress(NULL),
	mMessageBuffer(NULL),
	mMessageBufferLength(0),
	mStop(false)
{
	mServerAddress=new char[strlen(serverAddress)+1];
	strcpy(mServerAddress,serverAddress);

	mMessageBufferLength=1024;
	mMessageBuffer=new char[mMessageBufferLength];

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

	for(Logger::List<Logger::Entry*>::iterator it=mEntries.begin();it!=mEntries.end();++it){
		delete *it;
	}

	delete[] mMessageBuffer;
	delete[] mServerAddress;
}

void SOSLoggerListener::addLogEntry(Logger::Category *category,Logger::Level level,uint64 time,const char *text){
	mMutex->lock();
	mEntries.push_back(new Logger::Entry(category,level,time,text));
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
	
	const char *startText="!SOS<showMessage key='";
	const char *levelText=getSOSLevelName(level);
	const char *preLevelText="'>[";
	const char *postLevelText="]";
	const char *categoryText="";
	const char *separatorText="";
	const char *endText="</showMessage>";
	if(category!=NULL){
		categoryText=category->name;
		separatorText=": ";
	}

	int length=strlen(startText) + strlen(levelText) + strlen(preLevelText) + strlen(levelText) + strlen(postLevelText) + strlen(categoryText) + strlen(separatorText) + strlen(text) + strlen(endText) + 1;
	if(mMessageBufferLength < length){
		delete[] mMessageBuffer;
		mMessageBufferLength=length;
		mMessageBuffer=new char[length];
	}

	sprintf(mMessageBuffer,"%s%s%s%s%s%s%s%s%s",startText,levelText,preLevelText,levelText,postLevelText,categoryText,separatorText,text,endText);
	
	try{
		mSocket->send((tbyte*)mMessageBuffer, mMessageBufferLength);
	}catch(...){}
}

void SOSLoggerListener::run(){
	bool result = false;
	try{
		mSocket->connect(mServerAddress,4444);
	}catch(...){}

	if (result == false) {
		fprintf(stderr,"SOSLoggerListener could not connect!");
		return;
	}

	Logger::Entry *entry=NULL;

	while(mStop==false){
		mMutex->lock();
		if(mEntries.begin()!=mEntries.end()){
			entry=*mEntries.begin();
			mEntries.remove(entry);
		}
		else{
			mCondition->notify();
		}
		mMutex->unlock();

		if(entry){
			sendEntry(entry->category,entry->level,entry->time,entry->text);
			delete entry;
		}
	}
}

const char *SOSLoggerListener::getSOSLevelName(Logger::Level level){
	switch(level){
		case Logger::Level_DISABLED:
			return "Off";
		break;
		case Logger::Level_ERROR:
			return "Error";
		break;
		case Logger::Level_WARNING:
			return "Warning";
		break;
		case Logger::Level_ALERT:
			return "Info";
		break;
		case Logger::Level_DEBUG:
			return "Debug";
		break;
		case Logger::Level_EXCESS:
		default:
			return "Trace";
		break;
	}
}

}
}
