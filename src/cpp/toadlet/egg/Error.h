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

#ifndef TOADLET_EGG_ERROR_H
#define TOADLET_EGG_ERROR_H

#include <toadlet/egg/Categories.h>
#include <toadlet/egg/String.h>

#if defined(TOADLET_EXCEPTIONS)
	#include <toadlet/egg/Exception.h>

	#define TOADLET_MAKE_ERROR_FUNCTION(name,type) \
		static void name(const String &description){name(NULL,true,description);} \
		static void name(const String &categoryName,const String &description){name(categoryName,true,description);} \
		static void name(const String &categoryName,bool report,const String &description){ \
			if(report){errorLog(categoryName,description);} \
			throw toadlet::egg::Exception(type,description); \
		}
#else
	#define TOADLET_MAKE_ERROR_FUNCTION(name,type) \
		static void name(const String &description){name(NULL,true,description);} \
		static void name(const String &categoryName,const String &description){name(categoryName,true,description);} \
		static void name(const String &categoryName,bool report,const String &description){ \
			if(report){errorLog(categoryName,description);} \
			Error::getInstance()->setError(type,description); \
		}
#endif

namespace toadlet{
namespace egg{

class TOADLET_API Error{
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

		// Egg errors
		Type_EGG_START=1000,
		Type_LIBRARY_NOT_FOUND,
		Type_SYMBOL_NOT_FOUND,
		Type_LOADING_IMAGE,
		Type_BLUETOOTH_DISABLED,

		// Flick errors
		Type_SEQUENCE=2000,

		// Knot errors
		Type_KNOT_START=3000,
		Type_ALREADY_CONNECTED,
		Type_INCORRECT_PROTOCOL_NAME,
		Type_INCORRECT_PROTOCOL_VERSION,
	};

	static Error *getInstance();
	static void destroy();

	// General
	TOADLET_MAKE_ERROR_FUNCTION(unknown,Type_UNKNOWN)
	TOADLET_MAKE_ERROR_FUNCTION(tassert,Type_ASSERT) // Name assert may already be in use
	TOADLET_MAKE_ERROR_FUNCTION(invalidParameters,Type_INVALID_PARAMETERS)
	TOADLET_MAKE_ERROR_FUNCTION(nullPointer,Type_NULL_POINTER)
	TOADLET_MAKE_ERROR_FUNCTION(unimplemented,Type_UNIMPLEMENTED)
	TOADLET_MAKE_ERROR_FUNCTION(overflow,Type_OVERFLOW)

	// Egg
	TOADLET_MAKE_ERROR_FUNCTION(libraryNotFound,Type_LIBRARY_NOT_FOUND)
	TOADLET_MAKE_ERROR_FUNCTION(symbolNotFound,Type_SYMBOL_NOT_FOUND)
	TOADLET_MAKE_ERROR_FUNCTION(loadingImage,Type_LOADING_IMAGE)
	TOADLET_MAKE_ERROR_FUNCTION(bluetoothDisabled,Type_BLUETOOTH_DISABLED)

	// Flick
	TOADLET_MAKE_ERROR_FUNCTION(sequence,Type_SEQUENCE)

	// Knot
	TOADLET_MAKE_ERROR_FUNCTION(alreadyConnected,Type_ALREADY_CONNECTED)
	TOADLET_MAKE_ERROR_FUNCTION(incorrectProtocolName,Type_INCORRECT_PROTOCOL_NAME)
	TOADLET_MAKE_ERROR_FUNCTION(incorrectProtocolVersion,Type_INCORRECT_PROTOCOL_VERSION)

	/// @todo: Add threaded support to this, to make the errors thread specific
	void setError(int error);
	void setError(int error,const char *description);
	void setError(int error,const String &description);

	int getError();

	// The result of getErrorDescription() is only valid if getError() returned
	// a value other than ERROR_NONE
	const char *getDescription();

protected:
	Error();

	static void errorLog(const String &categoryName,const String &description);

	static Error *mTheError;

	static const int MAX_DESCRIPTION_LENGTH=1024;
	int mLastError;
	char mLastDescription[MAX_DESCRIPTION_LENGTH+1];
};

}
}

#endif
