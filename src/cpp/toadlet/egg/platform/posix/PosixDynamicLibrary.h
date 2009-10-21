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

#ifndef TOADLET_EGG_POSIXDYNAMICLIBRARY_H
#define TOADLET_EGG_POSIXDYNAMICLIBRARY_H

#include <toadlet/egg/String.h>
#include <toadlet/egg/WeakPointer.h>
#include <dlfcn.h>

#define TOADLET_EGG_DYNAMICLIBRARY_PREFIX "lib"
#if defined(TOADLET_PLATFORM_OSX)
#	define TOADLET_EGG_DYNAMICLIBRARY_EXTENSION ".dylib"
#else
#	define TOADLET_EGG_DYNAMICLIBRARY_EXTENSION ".so"
#endif

namespace toadlet{
namespace egg{

class PosixDynamicLibrary{
public:
	TOADLET_SHARED_POINTERS(PosixDynamicLibrary,PosixDynamicLibrary);

	PosixDynamicLibrary();
	~PosixDynamicLibrary();

	bool load(const String &name,const String &directory=NULL,const String &prefix=TOADLET_EGG_DYNAMICLIBRARY_PREFIX,const String &extension=TOADLET_EGG_DYNAMICLIBRARY_EXTENSION);
	void *getSymbol(const String &name);

protected:
	void *mLibrary;
};

}
}

#endif
