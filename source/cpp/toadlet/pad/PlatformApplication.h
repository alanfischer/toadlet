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

#ifndef TOADLET_PAD_PLATFORMAPPLICATION_H
#define TOADLET_PAD_PLATFORMAPPLICATION_H

#include <toadlet/Types.h>

#if defined(TOADLET_PLATFORM_ANDROID)
/*	#if ANDROID_NDK_API_LEVEL>=9
		#include <toadlet/pad/platform/android/AndroidApplication.h>
		namespace toadlet{
		namespace pad{
			typedef AndroidApplication PlatformApplication;
		}
		}
*/
#if 0
	#else
		#include <toadlet/pad/platform/android/JApplication.h>
		namespace toadlet{
		namespace pad{
			typedef JApplication PlatformApplication;
		}
		}
	#endif
#elif defined(TOADLET_PLATFORM_WIN32)
	#include <toadlet/pad/platform/win32/Win32Application.h>
	namespace toadlet{
	namespace pad{
		typedef Win32Application PlatformApplication;
	}
	}
#elif defined(TOADLET_PLATFORM_OSX)
	#include <toadlet/pad/platform/osx/OSXApplication.h>
	namespace toadlet{
	namespace pad{
		typedef OSXApplication PlatformApplication;
	}
	}
#elif defined(TOADLET_PLATFORM_POSIX)
	#include <toadlet/pad/platform/x11/X11Application.h>
	namespace toadlet{
	namespace pad{
		typedef X11Application PlatformApplication;
	}
	}
#else
	#error "PlatformApplication not implemented on this platform"
#endif

#endif
