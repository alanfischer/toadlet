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

#include <toadlet/tadpole/FontManager.h>
#if defined(TOADLET_PLATFORM_WIN32)
	#include <windows.h>
	#include <shlobj.h>
#endif

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace tadpole{

FontManager::FontManager(Archive *archive):ResourceManager(archive){}

Resource::ptr FontManager::manage(const Resource::ptr &resource,const egg::String &name){
	if(name!=(char*)NULL){
		ResourceManager::manage(resource,name+String(":")+shared_static_cast<Font>(resource)->getPointSize());
	}
	else{
		ResourceManager::manage(resource);
	}

	return resource;
}

Font::ptr FontManager::getDefaultFont(){
	if(mDefaultFont==NULL){
		String file;
		#if defined(TOADLET_PLATFORM_WIN32)
			TCHAR path[MAX_PATH];
			SHGetSpecialFolderPath(0,path,CSIDL_FONTS,false);
			file=path+String("\\Arial.ttf");
		#elif defined(TOADLET_PLATFORM_POSIX)
			file="/usr/share/fonts/TTF/DejaVuSerif.ttf";
		#endif
		mDefaultFont=findFont(file,24);
	}
	return mDefaultFont;
}

void FontManager::unmanage(Resource *resource){
	return ResourceManager::unmanage(resource);
}

}
}

