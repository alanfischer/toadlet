#include "SOSLoggerListener.h"
#include "Log.h"
#include <stdio.h>

#if defined(LOGIT_PLATFORM_WIN32)
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN 1
	#endif
	#include <windows.h>
	#include <winsock.h>

	#if defined(LOGIT_PLATFORM_WINCE)
		#pragma comment(lib,"ws2.lib")
	#else
		#pragma comment(lib,"wsock32.lib")
	#endif
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netinet/tcp.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <pthread.h>
	#include <unistd.h>
#endif

namespace logit{

#if defined(LOGIT_PLATFORM_WIN32)
unsigned long WINAPI startSOSThread(void *thread){
#else
void *startSOSThread(void *thread){
#endif
	((SOSLoggerListener*)thread)->run();

	#if defined(LOGIT_PLATFORM_WIN32)
		ExitThread(0);
	#else
		pthread_exit(NULL);
	#endif

	return 0;
}

SOSLoggerListener::SOSLoggerListener(const char *serverAddress):
	mServerAddress(NULL),
	mMessageBuffer(NULL),
	mMessageBufferLength(0),
	mMutex(NULL),
	mCondition(NULL),
	mStop(false)
{
	mServerAddress=new char[strlen(serverAddress)+1];
	strcpy(mServerAddress,serverAddress);

	mMessageBufferLength=1024;
	mMessageBuffer=new char[mMessageBufferLength];

	mSocket=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);

	mMutex=Log::createMutex();
	mCondition=Log::createCondition();

	#if defined(LOGIT_PLATFORM_WIN32)
		mThread=CreateThread(NULL,0,&startSOSThread,this,0,0);
		ResumeThread(mThread);
	#else
		pthread_create((pthread_t*)&mThread,NULL,&startSOSThread,(void*)this);
	#endif
}

SOSLoggerListener::~SOSLoggerListener(){
	flush();
	
	#if defined(LOGIT_PLATFORM_WIN32)
		shutdown(mSocket,2);
		::closesocket(mSocket);
	#else
		shutdown(mSocket,SHUT_RDWR);
		::close(mSocket);
	#endif

	mStop=true;

	#if defined(LOGIT_PLATFORM_WIN32)
		WaitForSingleObject(mThread,INFINITE);
		CloseHandle(mThread);
	#else
		pthread_join((pthread_t)mThread,NULL);
		pthread_detach((pthread_t)mThread);
	#endif

	for(Logger::List<Logger::Entry*>::iterator it=mEntries.begin();it!=mEntries.end();++it){
		delete *it;
	}

	delete[] mMessageBuffer;
	delete[] mServerAddress;

	Log::destroyCondition(mCondition);
	Log::destroyMutex(mMutex);
}

void SOSLoggerListener::addLogEntry(Logger::Category *category,Logger::Level level,Logger::timestamp time,const char *text){
	Log::lock(mMutex);
	mEntries.push_back(new Logger::Entry(category,level,time,text));
	Log::unlock(mMutex);
}

void SOSLoggerListener::flush(){
	Log::lock(mMutex);
	Log::wait(mCondition,mMutex);
	Log::unlock(mMutex);
}

void SOSLoggerListener::sendEntry(Logger::Category *category,Logger::Level level,Logger::timestamp time,const char *text){
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
	send(mSocket,mMessageBuffer,length,0);
}

void SOSLoggerListener::run(){
	struct sockaddr_in address={0};
	address.sin_family=AF_INET;
	address.sin_port=htons(4444);
	struct hostent *he=NULL;
	
	he=gethostbyname(mServerAddress);
	if(he!=NULL){
		char **list=he->h_addr_list;
		if((*list)!=NULL){
			address.sin_addr.s_addr=*(unsigned int*)(*list);
		}
	}

	if(connect(mSocket,(struct sockaddr*)&address,sizeof(address))<0){
		fprintf(stderr,"SOSLoggerListener: unable to connect to %s",mServerAddress);
		return;
	}

	Logger::Entry *entry=NULL;

	while(mStop==false){
		Log::lock(mMutex);
		if(mEntries.begin()!=mEntries.end()){
			entry=*mEntries.begin();
			mEntries.remove(mEntries.begin());
		}
		else{
			Log::notify(mCondition);
		}
		Log::unlock(mMutex);

		if(entry){
			sendEntry(entry->category,entry->level,entry->time,entry->text);
			delete entry;
			entry=NULL;
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
