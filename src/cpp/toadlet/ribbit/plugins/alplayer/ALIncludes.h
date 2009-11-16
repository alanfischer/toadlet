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

#ifndef TOADLET_PEEPER_ALINCLUDES_H
#define TOADLET_PEEPER_ALINCLUDES_H

#include <toadlet/egg/Logger.h>
#include <toadlet/egg/MathConversion.h>
#include <toadlet/ribbit/Types.h>

#if defined(TOADLET_PLATFORM_OSX)
	extern "C" {
		#include <OpenAL/al.h>
		#include <OpenAL/alc.h>
	}
#elif defined(TOADLET_PLATFORM_WIN32) || defined(TOADLET_PLATFORM_POSIX)
	#define TOADLET_HAS_ALEM
	#include "alem/alem.h"
#else
	#error "Unknown platform"
#endif

#ifdef TOADLET_LITTLE_ENDIAN
	#define TOADLET_NATIVE_FORMAT
#endif

#if defined(TOADLET_DEBUG)
	#define TOADLET_CHECK_ALERROR(function) \
		{ int error=alGetError(); \
		if(error!=AL_NO_ERROR) \
			toadlet::egg::Logger::log(toadlet::egg::Categories::TOADLET_RIBBIT,toadlet::egg::Logger::Level_ALERT, \
				toadlet::egg::String("AL Error in ") + function + ": error=" + error + " string=" + alGetString(error)); }

#else
	#define TOADLET_CHECK_ALERROR(function)
#endif

#endif
