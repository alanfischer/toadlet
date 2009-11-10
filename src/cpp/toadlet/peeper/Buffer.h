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

namespace toadlet{
namespace peeper{

class TOADLET_API Buffer{
public:
	TOADLET_SHARED_POINTERS(Buffer);

	enum UsageFlags{
		UsageFlags_NONE=		0,
		UsageFlags_STATIC=		1<<0,	// Buffer data is never changed
		UsageFlags_STREAM=		1<<1,	// Buffer data changes once per frame
		UsageFlags_DYNAMIC=		1<<2,	// Buffer data changes frequently
	};

	enum AccessType{
		AccessType_NO_ACCESS,			// Buffer data is inaccessable
		AccessType_READ_ONLY,			// Buffer data is only readable
		AccessType_WRITE_ONLY,			// Buffer data is only writeable
		AccessType_READ_WRITE,
	};

	virtual ~Buffer(){}

	virtual int getUsageFlags() const=0;
	virtual AccessType getAccessType() const=0;
	virtual int getDataSize() const=0;

	virtual uint8 *lock(AccessType accessType)=0;
	virtual bool unlock()=0;
};

}
}

#endif
