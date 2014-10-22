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

#include "PosixErrorHandler.h"
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

extern "C" void PosixErrorHandler_installNSHandler(){
	mOldHandler=NSGetUncaughtExceptionHandler();
	NSSetUncaughtExceptionHandler(PosixErrorHandler_exceptionHandler);
}
	
extern "C" void PosixErrorHandler_uninstallNSHandler(){
	NSSetUncaughtExceptionHandler(mOldHandler);
	mOldHandler=NULL;
}

}
}

