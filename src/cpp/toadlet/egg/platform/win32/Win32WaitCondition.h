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

#ifndef TOADLET_EGG_WIN32WAITCONDITION_H
#define TOADLET_EGG_WIN32WAITCONDITION_H

#include <toadlet/egg/Mutex.h>
#ifndef WIN32_LEAN_AND_MEAN
#	define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>

namespace toadlet{
namespace egg{

class TOADLET_API Win32WaitCondition{
public:
	TOADLET_SHARED_POINTERS(Win32WaitCondition,Win32WaitCondition);

	Win32WaitCondition();
	~Win32WaitCondition();

	bool wait(Mutex *mutex,int time);
	bool wait(Mutex *mutex);

	void notify();

protected:
	HANDLE mEvent;
};

}
}

#endif

