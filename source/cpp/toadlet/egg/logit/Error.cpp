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

#include "Error.h"
#include "Log.h"

#if defined(LOGIT_PLATFORM_WIN32)
	#include "Win32ErrorHandler.h"
#elif defined(LOGIT_PLATFORM_POSIX)
	#include "PosixErrorHandler.h"
#endif

namespace logit{

Errorer *Error::mTheErrorer=NULL;
void *Error::mErrorHandler=NULL;

Errorer *Error::getInstance(){
	if(mTheErrorer==NULL){
		initialize();
	}

	return mTheErrorer;
}

void Error::initialize(bool handler){
	if(mTheErrorer==NULL){
		mTheErrorer=new Errorer(Log::getInstance());

		if(handler){
			#if defined(LOGIT_PLATFORM_WIN32)
				Win32ErrorHandler *errorHandler=new Win32ErrorHandler();
				errorHandler->setStackTraceListener(mTheErrorer);
				errorHandler->installHandler();
				mErrorHandler=errorHandler;
			#elif defined(LOGIT_PLATFORM_POSIX)
				PosixErrorHandler *errorHandler=new PosixErrorHandler();
				errorHandler->setStackTraceListener(mTheErrorer);
				errorHandler->installHandler();
				mErrorHandler=errorHandler;
			#endif
		}
	}
}

void Error::destroy(){
	if(mErrorHandler!=NULL){
		#if defined(LOGIT_PLATFORM_WIN32)
			Win32ErrorHandler *errorHandler=(Win32ErrorHandler*)mErrorHandler;
			errorHandler->uninstallHandler();
			delete errorHandler;
		#elif defined(LOGIT_PLATFORM_POSIX)
			PosixErrorHandler *errorHandler=(PosixErrorHandler*)mErrorHandler;
			errorHandler->uninstallHandler();
			delete errorHandler;
		#endif
		mErrorHandler=NULL;
	}

	if(mTheErrorer!=NULL){
		delete mTheErrorer;
		mTheErrorer=NULL;
	}
}

}
