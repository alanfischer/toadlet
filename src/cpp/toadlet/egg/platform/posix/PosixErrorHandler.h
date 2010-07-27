#ifndef POSIXERRORHANDLER_H
#define POSIXERRORHANDLER_H

#include <toadlet/egg/StackTraceListener.h>

class PosixErrorHandler{
public:
	PosixErrorHandler();
	~PosixErrorHandler();

	void setStackTraceListener(StackTraceListener *listener){mListener=listener;}
	StackTraceListener *getStackTraceListener(){return mListener;}

	void installHandler();

protected:
	static signalHandler(int sig,siginfo_t *info,void *context);

	static int Signals[SIGNUM];
	static StackTraceListener *mListener;

	struct sigaction mAction;
	struct sigaction mOldActions[SIGMAX];
}

#endif

