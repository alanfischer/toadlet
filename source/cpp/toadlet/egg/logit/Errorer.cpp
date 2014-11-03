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

#include "Errorer.h"

namespace logit{

Errorer::Errorer(Logger *logger):
	mLastError(Type_NONE),
	mLastDescription(NULL),
	mLastDescriptionLength(0),
	mLogger(logger)
{
	mLastDescriptionLength=1024;
	mLastDescription=new char[mLastDescriptionLength];
	memset(mLastDescription,0,mLastDescriptionLength);
}

Errorer::~Errorer(){
	delete mLastDescription;
}

void Errorer::setError(int error,const char *category,const char *description,bool report){
	mLastError=error;
	if(description==NULL){mLastDescription[0]=0;}
	else{
		int length=strlen(description);
		if(length>mLastDescriptionLength){
			char *lastDescription=new char[length+1];
			if(lastDescription!=NULL){
				delete mLastDescription;
				mLastDescriptionLength=length+1;
				mLastDescription=lastDescription;
			}
		}
		strncpy(mLastDescription,description,mLastDescriptionLength);
	}

	if(report && mLogger!=NULL){
		mLogger->addLogEntry(category,Logger::Level_ERROR,description);
	}
}

void Errorer::setException(const Exception &ex,bool report){
	setError(ex.getError(),ex.getCategory(),ex.getDescription(),report);
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

void Errorer::startTrace(){
	if(mLogger!=NULL){
		mLogger->addLogEntry(Logger::Level_ERROR,"Backtrace starting");
	}
}

void Errorer::traceFrame(const char *description){
	if(mLogger!=NULL){
		mLogger->addLogEntry(Logger::Level_ERROR,description);
	}
}

void Errorer::endTrace(){
	if(mLogger!=NULL){
		mLogger->addLogEntry(Logger::Level_ERROR,"Backtrace ended");
		mLogger->flush();
	}
}

}
