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

#include <toadlet/egg/Error.h>
#include <toadlet/tadpole/FontManager.h>
#if defined(TOADLET_PLATFORM_WIN32)
	#include <windows.h>
	#include <shlobj.h>
#endif

namespace toadlet{
namespace tadpole{

FontManager::FontManager(Engine *engine):ResourceManager(engine){
	mDefaultCharacterSet=String("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789~!@#$%^&*()_+|{}:\"'<>?`-=\\/[];,. \t");
}

Resource::ptr FontManager::manage(const Resource::ptr &resource,const String &name){
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
		#elif defined(TOADLET_PLATFORM_IPHONE)
			file="/System/Library/Fonts/Cache/Arial.ttf";
		#elif defined(TOADLET_PLATFORM_OSX)
			file="/Library/Fonts/Arial.ttf";
		#elif defined(TOADLET_PLATFORM_POSIX)
			file="/usr/share/fonts/TTF/DejaVuSerif.ttf";
		#endif
		mDefaultFont=findFont(file,64);
	}
	return mDefaultFont;
}

void FontManager::unmanage(Resource *resource){
	return ResourceManager::unmanage(resource);
}

Resource::ptr FontManager::find(const String &name,ResourceData::ptr data){
	FontData *fontData=(FontData*)data.get();
	if(fontData==NULL){
		Error::nullPointer(Categories::TOADLET_TADPOLE,
			"invalid FontData");
		return NULL;
	}

//	if(fontData->characterSet==NULL){
		fontData->characterSet=mDefaultCharacterSet;
//	}

	return ResourceManager::find(name,data);
}

}
}

