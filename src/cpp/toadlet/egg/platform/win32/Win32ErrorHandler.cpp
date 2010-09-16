#include <toadlet/egg/platform/win32/Win32ErrorHandler.h>

//#include "StackWalker.h"
//#include "StackWalker.cpp"

namespace toadlet{
namespace egg{

Win32ErrorHandler *Win32ErrorHandler::instance=NULL;

Win32ErrorHandler::Win32ErrorHandler():
	mInstalled(false),
	mListener(NULL)
{
	instance=this;
}

Win32ErrorHandler::~Win32ErrorHandler(){ 
	uninstallHandler();

	instance=NULL;
}

void Win32ErrorHandler::installHandler(){
	if(mInstalled) return;

	mOldFilter=SetUnhandledExceptionFilter(exceptionFilter);
	mInstalled=true;
}

void Win32ErrorHandler::uninstallHandler(){
	if(mInstalled==false) return;

	SetUnhandledExceptionFilter(mOldFilter);
	mInstalled=false;
}
/*
class MyStackWalker : public StackWalker
{
public:
  MyStackWalker() : StackWalker() {}
  MyStackWalker(DWORD dwProcessId, HANDLE hProcess) : StackWalker(dwProcessId, hProcess) {}
  virtual void OnOutput(LPCSTR szText) { printf(szText); StackWalker::OnOutput(szText); }
};
*/
LONG Win32ErrorHandler::exceptionFilter(LPEXCEPTION_POINTERS e){
//	MyStackWalker walker;
//	walker.ShowCallstack(GetCurrentThread(),e->ContextRecord);
	instance->errorHandled();
	return EXCEPTION_EXECUTE_HANDLER;
}

void Win32ErrorHandler::errorHandled(){
	abort();
}

}
}
