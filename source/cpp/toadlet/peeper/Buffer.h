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

#ifndef TOADLET_PEEPER_BUFFER_H
#define TOADLET_PEEPER_BUFFER_H

#include <toadlet/egg/Resource.h>
#include <toadlet/peeper/Types.h>

namespace toadlet{
namespace peeper{

class Buffer:public Resource{
public:
	TOADLET_IPTR(Buffer);

	enum Usage{
		Usage_NONE=			0,
		Usage_BIT_STATIC=	1<<0,	// Data is never changed
		Usage_BIT_STREAM=	1<<1,	// Data changes once per frame or less
		Usage_BIT_DYNAMIC=	1<<2,	// Data changes multiple times per frame
		Usage_BIT_STAGING=	1<<3,	// Data is only used for loading, and is lockable
	};

	enum Access{
		Access_NONE=		0,		// Data is inaccessable
		Access_BIT_READ=	1<<0,	// Data is readable
		Access_BIT_WRITE=	1<<1,	// Data is writeable
		Access_READ_WRITE=	Access_BIT_READ|Access_BIT_WRITE,
	};

	virtual ~Buffer(){}

	virtual int getUsage() const=0;
	virtual int getAccess() const=0;
	virtual int getDataSize() const=0;

	virtual tbyte *lock(int lockAccess)=0;
	virtual bool unlock()=0;

	virtual bool update(tbyte *data,int start,int size)=0;
};

}
}

#endif
