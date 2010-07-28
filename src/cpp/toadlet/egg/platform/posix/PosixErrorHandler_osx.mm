#include <toadlet/egg/platform/posix/PosixErrorHandler.h>
#import <Foundation/Foundation.h>

namespace toadlet{
namespace egg{

NSUncaughtExceptionHandler *mOldHandler=NULL;

void PosixErrorHandler_exceptionHandler(NSException *exception){
	NSArray *callStackArray=[exception callStackReturnAddresses];
	int frameCount=[callStackArray count];
	void *backtraceFrames[frameCount];

	int i=0;
	for(i=0;i<frameCount;i++){
		backtraceFrames[i]=(void*)[[callStackArray objectAtIndex:i] unsignedIntegerValue];
	}
	
	PosixErrorHandler::instance->handleFrames(backtraceFrames,frameCount);
	PosixErrorHandler::instance->errorHandled();
}

void PosixErrorHandler_installNSHandler(){
	mOldHandler=NSGetUncaughtExceptionHandler();
	NSSetUncaughtExceptionHandler(PosixErrorHandler_exceptionHandler);
}
	
void PosixErrorHandler_uninstallNSHandler(){
	NSSetUncaughtExceptionHandler(mOldHandler);
	mOldHandler=NULL;
}

}
}

