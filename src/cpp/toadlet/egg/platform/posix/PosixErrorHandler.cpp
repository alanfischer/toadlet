/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright 2009, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer, Andrew Fischer
 *
 * This file is part of The Toadlet Engine.
 *
 * The Toadlet Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * The Toadlet Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with The Toadlet Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 ********** Copyright header - do not remove **********/

#include <toadlet/egg/platform/posix/PosixErrorHandler.h>
#include <stdlib.h>
#if !defined(TOADLET_PLATFORM_ANDROID)
	#include <execinfo.h>
#endif
#include <stdio.h>

namespace toadlet{
namespace egg{

#if defined(TOADLET_PLATFORM_OSX)
	extern "C" void PosixErrorHandler_installNSHandler();
	extern "C" void PosixErrorHandler_uninstallNSHandler();
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
	#if defined(TOADLET_PLATFORM_ANDROID)
		int frameCount=0;
	#else
		int frameCount=backtrace(stackFrames,MAX_STACKFRAMES);
	#endif
	instance->handleFrames(stackFrames,frameCount);
	instance->errorHandled();
}

void PosixErrorHandler::handleFrames(void **frames,int count){
	#if !defined(TOADLET_PLATFORM_ANDROID)
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
	#endif
}
	
void PosixErrorHandler::errorHandled(){
	abort();
}

}
}
