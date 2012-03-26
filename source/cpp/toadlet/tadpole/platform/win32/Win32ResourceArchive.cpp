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
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/io/MemoryStream.h>
#include <toadlet/tadpole/platform/win32/Win32ResourceArchive.h>
#include <windows.h>

#ifndef IS_INTRESOURCE
	#define IS_INTRESOURCE(_r) ((((ULONG_PTR)(_r)) >> 16) == 0)
#endif

namespace toadlet{
namespace tadpole{

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

Win32ResourceArchive::Win32ResourceArchive(TextureManager *textureManager):
	mModule(0)
{
	mTextureManager=textureManager;
}

Win32ResourceArchive::~Win32ResourceArchive(){
	destroy();
}

void Win32ResourceArchive::destroy(){
	mModule=0;

	BaseResource::destroy();
}

bool Win32ResourceArchive::open(void *module){
	mModule=module;

	mEntries=Collection<String>::ptr(new Collection<String>());
	BOOL result=FALSE;
	#if !defined(TOADLET_PLATFORM_WINCE)
		result=EnumResourceNames((HMODULE)mModule,RT_RCDATA,&resourceFoundCallback,(LONG_PTR)this);

		if(mTextureManager!=NULL){
			result=EnumResourceNames((HMODULE)mModule,RT_BITMAP,&resourceFoundCallback,(LONG_PTR)this);
		}
	#endif
	
	return result>0;
}

void Win32ResourceArchive::setMap(Map<String,int>::ptr idMap){
	mIDMap=idMap;
}

Stream::ptr Win32ResourceArchive::openStream(const String &name){
	LPTSTR resName=(LPTSTR)findResourceName(name);

	HRSRC src=FindResource((HMODULE)mModule,IS_INTRESOURCE(resName)?resName:"\""+String(resName)+"\"",RT_RCDATA);
	if(src==NULL) return NULL;
	
	HGLOBAL handle=LoadResource((HMODULE)mModule,src);
	if(handle==NULL) return NULL;
	
	LPVOID data=LockResource(handle);
	DWORD length=SizeofResource((HMODULE)mModule,src);
	
	MemoryStream::ptr stream(new MemoryStream((tbyte*)data,length,length,false));
	return stream;
}

Resource::ptr Win32ResourceArchive::openResource(const String &name){
	if(mTextureManager==NULL){
		return NULL;
	}

	LPTSTR resName=(LPTSTR)findResourceName(name);

	HBITMAP hbitmap=LoadBitmap((HMODULE)mModule,IS_INTRESOURCE(resName)?resName:"\""+String(resName)+"\"");
	if(hbitmap==NULL) return NULL;
	
	BITMAP bitmap={0};
	int result=GetObject(hbitmap,sizeof(bitmap),&bitmap);

	int textureWidth=bitmap.bmWidth;
	int textureHeight=bitmap.bmWidth;
	int texturePixelFormat=0;
	switch(bitmap.bmBitsPixel){
		case(16):
			texturePixelFormat=TextureFormat::Format_BGR_5_6_5;
		break;
		case(24):
			texturePixelFormat=TextureFormat::Format_BGR_8;
		break;
		case(32):
			texturePixelFormat=TextureFormat::Format_BGRA_8;
		break;
	}

	if(texturePixelFormat==0){
		DeleteObject(hbitmap);
		return NULL;
	}
	
	TextureFormat::ptr textureFormat(new TextureFormat(TextureFormat::Dimension_D2,texturePixelFormat,textureWidth,textureHeight,0,0));
	tbyte *textureData=new tbyte[textureFormat->getDataSize()];

	int textureStride=textureFormat->getXPitch();
	/// @todo: Figure out bitmapStride
	int bitmapStride=textureStride;//((imageWidth*2+sizeof(DWORD)-1)>>2)<<2; // stride is in DWORDs

	tbyte *bitmapData=new tbyte[bitmapStride*textureHeight];
	if(textureData==NULL || bitmapData==NULL){
		delete[] textureData;
		DeleteObject(hbitmap);
		return NULL;
	}

	GetBitmapBits(hbitmap,bitmapStride*textureHeight,bitmapData);

	// Flip the bitmap and copy it into the image
	int i;
	for(i=0;i<textureHeight;++i){
		memcpy(textureData+textureStride*(textureHeight-i-1),bitmapData+bitmapStride*i,bitmapStride);
	}

	Texture::ptr texture=mTextureManager->createTexture(textureFormat,textureData);

	delete[] bitmapData;
	delete[] textureData;
	DeleteObject(hbitmap);

	return texture;
}

Collection<String>::ptr Win32ResourceArchive::getEntries(){
	return mEntries;
}

void Win32ResourceArchive::resourceFound(const String &name){
	mEntries->add(name);
}

void *Win32ResourceArchive::findResourceName(const String &name){
	if(mModule==NULL){
		Error::unknown("Win32ResourceArchive is not open");
		return NULL;
	}

	LPCTSTR resName=name;
	if(mIDMap!=NULL){
		Map<String,int>::iterator result=mIDMap->find(name);
		if(result!=mIDMap->end()){
			resName=MAKEINTRESOURCE(result->second);
		}
	}
	return (void*)resName;
}

}
}
