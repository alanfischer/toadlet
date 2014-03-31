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

#include <toadlet/egg/Errorer.h>

#if defined(TOADLET_EXCEPTIONS)
	#define TOADLET_MAKE_ERROR_FUNCTION(name,type) \
		static toadlet::egg::Exception name(const char *text){return name((char*)NULL,true,text,Throw_YES);} \
		static toadlet::egg::Exception name(const char *text,Throw throwit){return name((char*)NULL,true,text,throwit);} \
		static toadlet::egg::Exception name(const char *categoryName,const char *text,Throw throwit=Throw_YES){return name(categoryName,true,text,throwit);} \
		static toadlet::egg::Exception name(const char *categoryName,bool report,const char *text,Throw throwit=Throw_YES){ \
			if(report){errorLog(categoryName,text);} \
			toadlet::egg::Exception ex(type,text); \
			Error::getInstance()->setException(ex); \
			if(throwit==Throw_YES){ \
				throw ex; \
			} \
			return ex; \
		}
#else
	#define TOADLET_MAKE_ERROR_FUNCTION(name,type) \
		static toadlet::egg::Exception name(const char *text){return name((char*)NULL,true,text,Throw_YES);} \
		static toadlet::egg::Exception name(const char *text,Throw throwit){return name((char*)NULL,true,text,throwit);} \
		static toadlet::egg::Exception name(const char *categoryName,const char *text,Throw throwit=Throw_YES){return name(categoryName,true,text,throwit);} \
		static toadlet::egg::Exception name(const char *categoryName,bool report,const char *text,Throw throwit=Throw_YES){ \
			if(report){errorLog(categoryName,text);} \
			toadlet::egg::Exception ex(type,text); \
			Error::getInstance()->setException(ex); \
			return ex; \
		}
#endif

namespace toadlet{
namespace egg{

class TOADLET_API Error{
public:
	enum Throw{
		Throw_NO,
		Throw_YES
	};

	static Errorer *getInstance();
	static void destroy();
    
	TOADLET_MAKE_ERROR_FUNCTION(unknown,Errorer::Type_UNKNOWN)
	TOADLET_MAKE_ERROR_FUNCTION(tassert,Errorer::Type_ASSERT) // Name assert may already be in use
	TOADLET_MAKE_ERROR_FUNCTION(invalidParameters,Errorer::Type_INVALID_PARAMETERS)
	TOADLET_MAKE_ERROR_FUNCTION(nullPointer,Errorer::Type_NULL_POINTER)
	TOADLET_MAKE_ERROR_FUNCTION(unimplemented,Errorer::Type_UNIMPLEMENTED)
	TOADLET_MAKE_ERROR_FUNCTION(overflow,Errorer::Type_OVERFLOW)
	TOADLET_MAKE_ERROR_FUNCTION(insufficientMemory,Errorer::Type_INSUFFICIENT_MEMORY)
	TOADLET_MAKE_ERROR_FUNCTION(fileNotFound,Errorer::Type_FILE_NOT_FOUND)
	TOADLET_MAKE_ERROR_FUNCTION(libraryNotFound,Errorer::Type_LIBRARY_NOT_FOUND)
	TOADLET_MAKE_ERROR_FUNCTION(symbolNotFound,Errorer::Type_SYMBOL_NOT_FOUND)
	TOADLET_MAKE_ERROR_FUNCTION(socket,Errorer::Type_SOCKET)

protected:
	static void errorLog(const char *categoryName,const char *description);

	static Errorer *mTheErrorer;
};

}
}

#endif
