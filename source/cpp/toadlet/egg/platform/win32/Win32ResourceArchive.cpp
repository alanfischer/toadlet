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

#include <toadlet/egg/Categories.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/io/MemoryStream.h>
#include "Win32ResourceArchive.h"
#include <windows.h>

#ifndef IS_INTRESOURCE
	#define IS_INTRESOURCE(_r) ((((ULONG_PTR)(_r)) >> 16) == 0)
#endif

namespace toadlet{
namespace egg{

BOOL CALLBACK resourceFoundCallback(HMODULE module,LPCTSTR type,LPTSTR name,LONG_PTR param){
	Win32ResourceArchive *archive=(Win32ResourceArchive*)param;
	if(IS_INTRESOURCE(name)){
		archive->resourceFound(String("#")+(int)name);
	}
	else{
		archive->resourceFound(name);
	}
	return TRUE;
}

Win32ResourceArchive::Win32ResourceArchive():
	mModule(0)
{}

void Win32ResourceArchive::destroy(){
	mModule=0;

	BaseResource::destroy();
}

bool Win32ResourceArchive::open(void *module){
	mModule=module;

	return findResources(RT_RCDATA);
}

void Win32ResourceArchive::setMap(const Map<String,int> &idMap){
	mIDMap=idMap;
}

Stream::ptr Win32ResourceArchive::openStream(const String &name){
	LPTSTR resName=(LPTSTR)findResourceName(name);

	HRSRC src=FindResource((HMODULE)mModule,IS_INTRESOURCE(resName)?resName:"\""+String(resName)+"\"",RT_RCDATA);
	if(src==NULL){
		//Error::nullPointer(Categories::TOADLET_EGG,"error in FindResource");
		return NULL;
	}
	
	HGLOBAL handle=LoadResource((HMODULE)mModule,src);
	if(handle==NULL){
		Error::nullPointer(Categories::TOADLET_EGG,"error in LoadResource");
		return NULL;
	}
	
	LPVOID data=LockResource(handle);
	DWORD length=SizeofResource((HMODULE)mModule,src);
	
	MemoryStream::ptr stream=new MemoryStream((tbyte*)data,length,length,false);
	return stream;
}

const Collection<String> &Win32ResourceArchive::getEntries(){
	return mEntries;
}

void Win32ResourceArchive::resourceFound(const String &name){
	mEntries.push_back(name);
}

void *Win32ResourceArchive::findResourceName(const String &name){
	if(mModule==NULL){
		Error::unknown("Win32ResourceArchive is not open");
		return NULL;
	}

	LPCTSTR resName=name;
	Map<String,int>::iterator result=mIDMap.find(name);
	if(result!=mIDMap.end()){
		resName=MAKEINTRESOURCE(result->second);
	}
	return (void*)resName;
}

bool Win32ResourceArchive::findResources(char *type){
	return EnumResourceNames((HMODULE)mModule,type,&resourceFoundCallback,(LONG_PTR)this)>0;
}

}
}
