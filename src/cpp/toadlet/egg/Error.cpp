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

#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <stdlib.h>
#if defined(TOADLET_PLATFORM_POSIX)
	#include <string.h>
#endif

namespace toadlet{
namespace egg{

Error *Error::mTheError=NULL;

Error *Error::getInstance(){
	if(mTheError==NULL){
		mTheError=new Error();
	}

	return mTheError;
}

void Error::destroy(){
	if(mTheError!=NULL){
		delete mTheError;
		mTheError=NULL;
	}
}

Error::Error(){
	mLastError=Type_NONE;
	memset(mLastDescription,0,sizeof(mLastDescription));
}

void Error::setError(int error){
	mLastError=error;
	mLastDescription[0]=0;
}

void Error::setError(int error,const char *description){
	mLastError=error;
	int c=0;
	while(description[c]!=0 && c<MAX_DESCRIPTION_LENGTH){
		mLastDescription[c]=description[c];
		c++;
	}
	mLastDescription[c]=0;
}

void Error::setError(int error,const String &description){
	mLastError=error;
	int c=0;
	const char *p=description.c_str();
	while(p[c]!=0 && c<MAX_DESCRIPTION_LENGTH){
		mLastDescription[c]=p[c];
		c++;
	}
	mLastDescription[c]=0;
}

int Error::getError(){
	int error=mLastError;
	mLastError=Type_NONE;
	return error;
}

const char *Error::getDescription(){
	return mLastDescription;
}

void Error::errorLog(const String &categoryName,const String &description){
	Logger::getInstance()->addLogString(categoryName,Logger::Level_ERROR,description);
}

}
}
