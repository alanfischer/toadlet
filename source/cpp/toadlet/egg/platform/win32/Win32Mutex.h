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

#ifndef TOADLET_EGG_WIN32MUTEX_H
#define TOADLET_EGG_WIN32MUTEX_H

#include <toadlet/Types.h>
#include <toadlet/egg/SharedPointer.h>

namespace toadlet{
namespace egg{

class Win32Event;

class TOADLET_API Win32Mutex{
public:
	TOADLET_SPTR(Win32Mutex);

	Win32Mutex();
	~Win32Mutex();

	bool tryLock();
	bool lock();
	bool unlock();

protected:
	void *mMutex;

	friend class Win32WaitCondition;
};

}
}

#endif

