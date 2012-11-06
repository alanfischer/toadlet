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

#include <toadlet/egg/Errorer.h>
#include <toadlet/egg/Log.h>

namespace toadlet{
namespace egg{

Errorer::Errorer(){
	mLastError=Type_NONE;
	memset(mLastDescription,0,sizeof(mLastDescription));

	#if defined(TOADLET_EGG_ERRORHANDLER_H)
		mErrorHandler.setStackTraceListener(this);
	#endif
}

Errorer::~Errorer(){
}

void Errorer::setError(int error){
	mLastError=error;
	mLastDescription[0]=0;
}

void Errorer::setError(int error,const char *description){
	mLastError=error;
	int c=0;
	while(description[c]!=0 && c<MAX_DESCRIPTION_LENGTH){
		mLastDescription[c]=description[c];
		c++;
	}
	mLastDescription[c]=0;
}

void Errorer::setError(int error,const String &description){
	mLastError=error;
	int c=0;
	const char *p=description.c_str();
	while(p[c]!=0 && c<MAX_DESCRIPTION_LENGTH){
		mLastDescription[c]=p[c];
		c++;
	}
	mLastDescription[c]=0;
}

void Errorer::setException(const Exception &ex){
	setError(ex.getError(),ex.getDescription());
	mException=ex;
}

int Errorer::getError(){
	int error=mLastError;
	mLastError=Type_NONE;
	return error;
}

const char *Errorer::getDescription(){
	return mLastDescription;
}

const Exception &Errorer::getException(){
	return mException;
}

void Errorer::installHandler(){
	#if defined(TOADLET_EGG_ERRORHANDLER_H)
		mErrorHandler.installHandler();
	#endif
}
	
void Errorer::uninstallHandler(){
	#if defined(TOADLET_EGG_ERRORHANDLER_H)
		mErrorHandler.uninstallHandler();
	#endif
}

void Errorer::startTrace(){
	Log::getInstance()->addLogEntry(Logger::Level_ERROR,"Backtrace starting");
}

void Errorer::traceFrame(const char *description){
	Log::getInstance()->addLogEntry(Logger::Level_ERROR,description);
}

void Errorer::endTrace(){
	Log::getInstance()->addLogEntry(Logger::Level_ERROR,"Backtrace ended");
	Log::getInstance()->flush();
}

}
}
