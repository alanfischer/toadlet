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

#include "Win32TextureResourceArchive.h"
#include <windows.h>

namespace toadlet{
namespace tadpole{

Win32TextureResourceArchive::Win32TextureResourceArchive(TextureManager *textureManager){
	mTextureManager=textureManager;
}

bool Win32TextureResourceArchive::open(void *module){
	if(Win32ResourceArchive::open(module) && mTextureManager!=NULL){
		return findResources(RT_BITMAP);
	}
	return false;
}

Resource::ptr Win32TextureResourceArchive::openResource(const String &name){
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
	
	TextureFormat::ptr textureFormat=new TextureFormat(TextureFormat::Dimension_D2,texturePixelFormat,textureWidth,textureHeight,0,0);
	tbyte *textureData=new tbyte[textureFormat->getDataSize()];

	int textureStride=textureFormat->getXPitch();
	int bitmapHeight=bitmap.bmHeight;
	int bitmapStride=((bitmap.bmWidth*2+sizeof(DWORD)-1)>>2)<<2; // stride is in DWORDs

	tbyte *bitmapData=new tbyte[bitmapStride*bitmapHeight];
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

}
}
