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

#ifndef TOADLET_EGG_ERRORER_H
#define TOADLET_EGG_ERRORER_H

#include <toadlet/egg/Categories.h>
#include <toadlet/egg/Exception.h>
#include <toadlet/egg/ErrorHandler.h>
#include <toadlet/egg/String.h>
#include <toadlet/egg/StackTraceListener.h>

namespace toadlet{
namespace egg{

class TOADLET_API Errorer:public StackTraceListener{
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
		Type_LIBRARY_NOT_FOUND,
		Type_SYMBOL_NOT_FOUND,
		Type_SOCKET,

		// Knot errors
		Type_KNOT_START=3000,
		Type_ALREADY_CONNECTED,
		Type_INCORRECT_PROTOCOL_NAME,
		Type_INCORRECT_PROTOCOL_VERSION,
	};

	Errorer();
	virtual ~Errorer();

	/// @todo: Add threaded support to this, to make the errors thread specific
	void setError(int error);
	void setError(int error,const char *description);
	void setError(int error,const String &description);
	void setException(const Exception &ex);

	int getError();
	// The result of getErrorDescription() is only valid if getError() returned a value other than ERROR_NONE
	const char *getDescription();
	const Exception &getException();

	void installHandler();
	void uninstallHandler();

	void startTrace();
	void traceFrame(const char *description);
	void endTrace();

protected:
	static const int MAX_DESCRIPTION_LENGTH=1024;
	int mLastError;
	char mLastDescription[MAX_DESCRIPTION_LENGTH+1];
	Exception mException;
	ErrorHandler mErrorHandler;
};

}
}

#endif
