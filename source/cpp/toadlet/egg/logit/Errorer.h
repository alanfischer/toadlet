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

#ifndef LOGIT_ERRORER_H
#define LOGIT_ERRORER_H

#include "StackTraceListener.h"
#include "Exception.h"
#include "Logger.h"

namespace logit{

class LOGIT_API Errorer:public StackTraceListener{
public:
	enum Type{
		// General errors
		Type_UNKNOWN=-1,
		Type_NONE=0,
		Type_ASSERT,
		Type_INVALID_PARAMETERS,
		Type_NULL_POINTER,
		Type_UNIMPLEMENTED,
		Type_OVERFLOW,
		Type_INSUFFICIENT_MEMORY,

		// Egg errors
		Type_EGG_START=1000,
		Type_FILE_NOT_FOUND,
		Type_LIBRARY_NOT_FOUND,
		Type_SYMBOL_NOT_FOUND,
		Type_SOCKET,
	};

	Errorer(Logger *logger);
	virtual ~Errorer();

	void setError(int error,const char *category=NULL,const char *description=NULL,bool report=false);
	void setException(const Exception &ex,bool report=false);

	int getError();
	const char *getDescription() const{return mLastDescription;}
	const Exception &getException() const{return mException;}

	void startTrace();
	void traceFrame(const char *description);
	void endTrace();

protected:
	int mLastError;
	char *mLastDescription;
	int mLastDescriptionLength;
	Exception mException;
	Logger *mLogger;
};

}

#endif
