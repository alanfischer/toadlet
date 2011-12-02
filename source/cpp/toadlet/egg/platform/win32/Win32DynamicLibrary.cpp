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

#include <toadlet/egg/platform/win32/Win32DynamicLibrary.h>
#include <toadlet/egg/Error.h>

namespace toadlet{
namespace egg{

Win32DynamicLibrary::Win32DynamicLibrary(){
	mLibrary=0;
}

Win32DynamicLibrary::~Win32DynamicLibrary(){
	if(mLibrary!=0){
		FreeLibrary(mLibrary);
	}
}

bool Win32DynamicLibrary::load(const String &name,const String &directory,const String &prefix,const String &extension){
	if(mLibrary!=0){
		FreeLibrary(mLibrary);
	}

	// NOTE: We don't use directory in the Win32 version, since windows can't search for shared libraries that way
	mLibrary=LoadLibrary(prefix+name+extension);
	if(mLibrary==0){
		Error::libraryNotFound(Categories::TOADLET_EGG,
			"library not found");
		return false;
	}
	else{
		return true;
	}
}

void *Win32DynamicLibrary::getSymbol(const String &name){
	void *symbol=GetProcAddress(mLibrary,name);
	if(symbol==NULL){
		Error::symbolNotFound(Categories::TOADLET_EGG,
			"symbol not found");
		return NULL;
	}

	return symbol;
}

}
}
