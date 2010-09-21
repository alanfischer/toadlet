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

bool PosixErrorHandler::installHandler(){
	if(mAction.sa_sigaction!=NULL) return true;

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
	
	return true;
}

bool PosixErrorHandler::uninstallHandler(){
	if(mAction.sa_sigaction==NULL) return true;

	int i;
	for(i=0;mSignals[i]>0;++i){
		sigaction(mSignals[i],&mOldActions[i],NULL);
	}

	memset(&mAction,0,sizeof(mAction));

	#if defined(TOADLET_PLATFORM_OSX)
		PosixErrorHandler_uninstallNSHandler();
	#endif
	
	return true;
}

void PosixErrorHandler::signalHandler(int sig,siginfo_t *info,void *context){
	void **stackFrames=instance->mStackFrames;
	int frameCount=backtrace(stackFrames,MAX_STACKFRAMES);
	instance->handleFrames(stackFrames,frameCount);
	instance->errorHandled();
}

void PosixErrorHandler::handleFrames(void **frames,int count){
	char **strings=backtrace_symbols(frames,count);

	if(mListener!=NULL){
		mListener->startTrace();
	}

	int i;
	for(i=0;i<count;++i){
		if(mListener!=NULL){
			mListener->traceFrame(strings[i]);
		}
	}

	if(mListener!=NULL){
		mListener->endTrace();
	}

	free(strings);
}
	
void PosixErrorHandler::errorHandled(){
	abort();
}

}
}
