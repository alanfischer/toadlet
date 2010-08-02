#include <toadlet/egg/platform/posix/PosixErrorHandler.h>
#include <stdlib.h>
#include <execinfo.h>
#include <string.h> //memset

#include <stdio.h>

namespace toadlet{
namespace egg{

#if defined(TOADLET_PLATFORM_OSX)
	extern "C" PosixErrorHandler_installNSHandler();
	extern "C" PosixErrorHandler_uninstallNSHandler();
#endif

int PosixErrorHandler::mSignals[]={
	SIGQUIT,
	SIGILL ,
	SIGTRAP,
//	SIGABRT,
//	SIGEMT ,
	SIGFPE ,
	SIGBUS ,
	SIGSEGV,
	SIGSYS ,
	SIGPIPE,
	SIGALRM,
	SIGXCPU,
	SIGXFSZ,
	0,
};

PosixErrorHandler *PosixErrorHandler::instance=NULL;

PosixErrorHandler::PosixErrorHandler():
	mListener(NULL)
{
	memset(&mAction,0,sizeof(mAction));
	instance=this;
}

PosixErrorHandler::~PosixErrorHandler(){
	uninstallHandler();
	instance=NULL;
}

void PosixErrorHandler::installHandler(){
	if(mAction.sa_sigaction!=NULL) return;

	mAction.sa_sigaction=signalHandler;
	mAction.sa_flags=SA_SIGINFO;
	sigemptyset(&mAction.sa_mask);

	int i;
	for(i=0;mSignals[i]>0;++i){
		sigaction(mSignals[i],&mAction,&mOldActions[i]);
	}

	#if defined(TOADLET_PLATFORM_OSX)
		PosixErrorHandler_installNSHandler();
	#endif
}

void PosixErrorHandler::uninstallHandler(){
	if(mAction.sa_sigaction==NULL) return;

	int i;
	for(i=0;mSignals[i]>0;++i){
		sigaction(mSignals[i],&mOldActions[i],NULL);
	}

	memset(&mAction,0,sizeof(mAction));

	#if defined(TOADLET_PLATFORM_OSX)
		PosixErrorHandler_uninstallNSHandler();
	#endif
}

void PosixErrorHandler::signalHandler(int sig,siginfo_t *info,void *context){
	void *backtraceFrames[128];
	int frameCount=backtrace(backtraceFrames,128);
	instance->handleFrames(backtraceFrames,frameCount);
	instance->errorHandled();
}

void PosixErrorHandler::handleFrames(void **frames,int count){
	char **strings=backtrace_symbols(frames,count);
	if(mListener!=NULL){
		mListener->backtrace(strings,count);
	}
	free(strings);
}
	
void PosixErrorHandler::errorHandled(){
	abort();
}

}
}
