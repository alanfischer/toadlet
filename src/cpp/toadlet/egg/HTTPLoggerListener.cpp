#include "HTTPLoggerListener.h"

namespace toadlet{
namespace egg{

HTTPLoggerListener::HTTPLoggerListener(String server,String path){
	mMutex=Mutex::ptr(new Mutex());
	mCondition=WaitCondition::ptr(new WaitCondition());

	mStop=false;
	mThread=Thread::ptr(new Thread(this));
	mThread->start();
}

HTTPLoggerListener::~HTTPLoggerListener(){
	flush();

	mStop=true;
	mThread->join();
}

void HTTPLoggerListener::addLogEntry(Logger::Category *category,Logger::Level level,uint64 time,const String &text){
	mMutex->lock();
	mEntries.add(Logger::Entry(category,level,time,text));
	mMutex->unlock();
}

void HTTPLoggerListener::flush(){
	mMutex->lock();
	mCondition->wait(mMutex);
	mMutex->unlock();
}

void HTTPLoggerListener::sendEntry(Logger::Category *category,Logger::Level level,uint64 time,const String &text){
	// Convert things to the append.php format
	(int&)level-=1;
	time/=1000;

	String body=
		String("category=")+((category!=NULL)?category->name:"") +
		String("&time=")+time +
		String("&level=")+level +
		String("&data=")+urlEncode(text);

	String url=mPath+"?"+body;

	String header=
		String("GET ")+url+" HTTP/1.0\n" +
		String("Host: ")+mServer+"\n" +
		"\n";

	Socket::ptr socket(Socket::createTCPSocket());
	socket->connect(mServer,80);
	socket->send((tbyte*)header.c_str(),header.length());
	socket->close();
}

void HTTPLoggerListener::run(){
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

String HTTPLoggerListener::urlEncode(const String &s){
	String escaped;
	int max=s.length();
	int i;
	for(i=0;i<max;i++){
		if( (48<=s[i] && s[i]<=57) ||
			(65<=s[i] && s[i]<=90) ||
			(97<=s[i] && s[i]<=122) ||
			(s[i]=='~' || s[i]=='!' || s[i]=='*' || s[i]=='(' || s[i]==')' || s[i]=='\'')
		){
			escaped=escaped+s[i];
		}
		else{
			escaped=escaped+'%';
			escaped=escaped+char2hex(s[i]);
		 }
	 }
	 return escaped;
}

String HTTPLoggerListener::char2hex(char dec){
	char dig1=(dec&0xF0)>>4;
	char dig2=(dec&0x0F);
	if( 0<=dig1 && dig1<= 9) dig1+=48;
	if(10<=dig1 && dig1<=15) dig1+=97-10;
	if( 0<=dig2 && dig2<= 9) dig2+=48;
	if(10<=dig2 && dig2<=15) dig2+=97-10;

	String r;
	r=r+dig1;
	r=r+dig2;
	return r;
}

}
}
