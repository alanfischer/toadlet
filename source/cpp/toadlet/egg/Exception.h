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

#ifndef TOADLET_EGG_EXCEPTION_H
#define TOADLET_EGG_EXCEPTION_H

#include <toadlet/egg/Types.h>
#include <toadlet/egg/String.h>

#if defined(TOADLET_EXCEPTIONS)
	#include <stdexcept>
#endif

namespace toadlet{
namespace egg{

class TOADLET_API Exception
	#if defined(TOADLET_EXCEPTIONS)
		:public std::exception
	#endif
{
public:
	Exception(int type=0);
	Exception(const char *description);
	Exception(int type,const char *description);
	Exception(const Exception &ex);
	virtual ~Exception() throw();

	int getError() const throw();
	const char *getDescription() const throw();

	#if defined(TOADLET_EXCEPTIONS)
		const char* what() const throw(){return getDescription();}
	#endif

protected:
	int mError;
	String mDescription;
};

}
}

#endif
