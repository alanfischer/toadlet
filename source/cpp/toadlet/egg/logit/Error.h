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

#ifndef LOGIT_ERROR_H
#define LOGIT_ERROR_H

#include "Errorer.h"

#if defined(LOGIT_EXCEPTIONS)
	#define LOGIT_MAKE_ERROR_FUNCTION(name,type) \
		static logit::Exception name(const char *text){return name((char*)NULL,true,text,Throw_YES);} \
		static logit::Exception name(const char *text,Throw throwit){return name((char*)NULL,true,text,throwit);} \
		static logit::Exception name(const char *category,const char *text,Throw throwit=Throw_YES){return name(category,true,text,throwit);} \
		static logit::Exception name(const char *category,bool report,const char *text,Throw throwit=Throw_YES){ \
			Errorer *instance=getInstance(); \
			logit::Exception ex(type,category,text); \
			instance->setException(ex,report); \
			if(throwit==Throw_YES){ \
				throw ex; \
			} \
			return ex; \
		}
#else
	#define LOGIT_MAKE_ERROR_FUNCTION(name,type) \
		static logit::Exception name(const char *text){return name((char*)NULL,true,text,Throw_YES);} \
		static logit::Exception name(const char *text,Throw throwit){return name((char*)NULL,true,text,throwit);} \
		static logit::Exception name(const char *category,const char *text,Throw throwit=Throw_YES){return name(category,true,text,throwit);} \
		static logit::Exception name(const char *category,bool report,const char *text,Throw throwit=Throw_YES){ \
			Errorer *instance=getInstance(); \
			logit::Exception ex(type,category,text); \
			instance->setException(ex,report); \
			return ex; \
		}
#endif

namespace logit{

class LOGIT_API Error{
public:
	enum Throw{
		Throw_NO,
		Throw_YES
	};

	static void initialize(bool handler=true);
	static Errorer *getInstance();
	static void destroy();
    
	LOGIT_MAKE_ERROR_FUNCTION(unknown,Errorer::Type_UNKNOWN)
	LOGIT_MAKE_ERROR_FUNCTION(tassert,Errorer::Type_ASSERT) // Name assert may already be in use
	LOGIT_MAKE_ERROR_FUNCTION(invalidParameters,Errorer::Type_INVALID_PARAMETERS)
	LOGIT_MAKE_ERROR_FUNCTION(nullPointer,Errorer::Type_NULL_POINTER)
	LOGIT_MAKE_ERROR_FUNCTION(unimplemented,Errorer::Type_UNIMPLEMENTED)
	LOGIT_MAKE_ERROR_FUNCTION(overflow,Errorer::Type_OVERFLOW)
	LOGIT_MAKE_ERROR_FUNCTION(insufficientMemory,Errorer::Type_INSUFFICIENT_MEMORY)
	LOGIT_MAKE_ERROR_FUNCTION(fileNotFound,Errorer::Type_FILE_NOT_FOUND)
	LOGIT_MAKE_ERROR_FUNCTION(libraryNotFound,Errorer::Type_LIBRARY_NOT_FOUND)
	LOGIT_MAKE_ERROR_FUNCTION(symbolNotFound,Errorer::Type_SYMBOL_NOT_FOUND)
	LOGIT_MAKE_ERROR_FUNCTION(socket,Errorer::Type_SOCKET)

protected:
	static Errorer *mTheErrorer;
	static void *mErrorHandler;
};

}

#endif
