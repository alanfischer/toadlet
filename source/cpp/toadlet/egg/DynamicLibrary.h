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

#ifndef TOADLET_EGG_DYNAMICLIBRARY_H
#define TOADLET_EGG_DYNAMICLIBRARY_H

#include <toadlet/Types.h>

#if defined(TOADLET_PLATFORM_WIN32)
	#include <toadlet/egg/platform/win32/Win32DynamicLibrary.h>
	namespace toadlet{
	namespace egg{
		typedef Win32DynamicLibrary DynamicLibrary;
	}
	}
#elif defined(TOADLET_PLATFORM_POSIX)
	#include <toadlet/egg/platform/posix/PosixDynamicLibrary.h>
	namespace toadlet{
	namespace egg{
		typedef PosixDynamicLibrary DynamicLibrary;
	}
	}
#else
	#error "DynamicLibrary not implemented on this platform"
#endif

#endif

