#ifndef WIN32ERRORHANDLER_H
#define WIN32ERRORHANDLER_H

#include <toadlet/Types.h>
#include <toadlet/egg/StackTraceListener.h>
#include <windows.h>

namespace toadlet{
namespace egg{

class TOADLET_API Win32ErrorHandler{
public:
	Win32ErrorHandler();
	~Win32ErrorHandler();

	virtual void setStackTraceListener(StackTraceListener *listener){mListener=listener;}
	virtual StackTraceListener *getStackTraceListener(){return mListener;}

	virtual void installHandler();
	virtual void uninstallHandler();

	virtual void errorHandled();

	static Win32ErrorHandler *instance;

protected:
	static LONG __stdcall exceptionFilter(LPEXCEPTION_POINTERS e);

	bool mInstalled;
	StackTraceListener *mListener;

	LPTOP_LEVEL_EXCEPTION_FILTER mOldFilter;

	const static int MAX_STACKFRAMES=128;
//	STACKFRAME64 mStackFrames[MAX_STACKFRAMES];
};

}
}

#endif

