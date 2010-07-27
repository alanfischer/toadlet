#include <toadlet/egg/platform/posix/PosixErrorHandler.h>

#if defined(TOADLET_PLATFORM_OSX)
	extern "C" PosixErrorHandler_installNSHandler();
	extern "C" PosixErrorHandler_uninstallNSHandler();
#endif

int PosixErrorHandler::mSignals[]={
	SIGQUIT,
	SIGILL ,
	SIGTRAP,
	SIGABRT,
	SIGEMT ,
	SIGFPE ,
	SIGBUS ,
	SIGSEGV,
	SIGSYS ,
	SIGPIPE,
	SIGALRM,
	SIGXCPU,
	SIGXFSZ,
	0,
}

PosixErrorHandler::PosixErrorHandler(){
	memset(&mAction,0,sizeof(mAction));
	mListener=NULL;
}

PosixErrorHandler::~PosixErrorHandler(){
	uninstallHandler();
}

void PosixErrorHandler::installHandler(){
	if(mAction.sa_sigaction!=NULL) return;

	mAction.sa_sigaction=sigHandler;
	mAction.sa_flags=SA_SIGINFO;
	sigemptyset(&mAction.sa_mask);

	int i;
	for(i=0;Signals[i]>0;++i){
		sigaction(mSignals[i],&mAction,mOldActions[i]);
	}

	#if defined(TOADLET_PLATFORM_OSX)
		PosixErrorHandler_installNSHandler();
	#endif
}

void PosixErrorHandler::uninstallHandler(){
	if(mAction.sa_sigaction==NULL) return;

	int i;
	for(i=0;Signals[i]>0;++i){
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
	handleFrames(backtraceFrames,frameCount);
}

void PosixErrorHandler::handleFrames(void **frames,int count){
	char **strings=backtrace_symbols(frames,count);
	if(mListener!=NULL){
		mListener->backtrace(strings,count);
	}
	free(strings);
}

