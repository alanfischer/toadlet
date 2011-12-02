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

#ifndef TOADLET_EGG_POSIXSYSTEM_H
#define TOADLET_EGG_POSIXSYSTEM_H

#include <toadlet/Types.h>
#include <toadlet/egg/String.h>
#include <toadlet/egg/SystemCaps.h>

namespace toadlet{
namespace egg{

class PosixSystem{
public:
	static void usleep(uint64 microseconds);
	static void msleep(uint64 milliseconds);

	static uint64 utime();
	static uint64 mtime();

	static String mtimeToString(uint64 time);
	
	static int threadID();

	static bool getSystemCaps(SystemCaps &caps);

	static bool absolutePath(const String &path);

	static String getEnv(const String &name);
	
protected:
	static void testSSE(SystemCaps &caps);
	static void testNEON(SystemCaps &caps);
};

}
}

#endif
