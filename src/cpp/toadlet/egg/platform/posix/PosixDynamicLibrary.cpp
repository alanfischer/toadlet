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

#include <toadlet/egg/platform/posix/PosixDynamicLibrary.h>
#include <toadlet/egg/Error.h>

namespace toadlet{
namespace egg{

PosixDynamicLibrary::PosixDynamicLibrary(){
	mLibrary=NULL;
}

PosixDynamicLibrary::~PosixDynamicLibrary(){
	if(mLibrary!=NULL){
		dlclose(mLibrary);
	}
}

bool PosixDynamicLibrary::load(const String &name,const String &directory,const String &prefix,const String &extension){
	if(mLibrary!=NULL){
		dlclose(mLibrary);
	}

	mLibrary=dlopen(directory+prefix+name+extension,RTLD_LAZY);
	const char *err=dlerror();
	if(err){
		mLibrary=NULL;

		Error::libraryNotFound(Categories::TOADLET_EGG,
			"library not found");
		return false;
	}
	return true;
}

void *PosixDynamicLibrary::getSymbol(const String &name){
	void *symbol=dlsym(mLibrary,name);
	const char *err=dlerror();
	if(err){
		Error::symbolNotFound(Categories::TOADLET_EGG,
			"symbol not found");
		return NULL;
	}
	return symbol;
}

}
}
