#ifndef POSIXERRORHANDLER_H
#define POSIXERRORHANDLER_H

#include <toadlet/egg/StackTraceListener.h>
#include <signal.h>

namespace toadlet{
namespace egg{

class PosixErrorHandler{
public:
	PosixErrorHandler();
	~PosixErrorHandler();

	virtual void setStackTraceListener(StackTraceListener *listener){mListener=listener;}
	virtual StackTraceListener *getStackTraceListener(){return mListener;}

	virtual void installHandler();
	virtual void uninstallHandler();

	virtual void handleFrames(void **frames,int count);
	virtual void errorHandled();

	static PosixErrorHandler *instance;

protected:
	static void signalHandler(int sig,siginfo_t *info,void *context);

	static int mSignals[NSIG];

	StackTraceListener *mListener;

	struct sigaction mAction;
	struct sigaction mOldActions[NSIG];
};

}
}

#endif

