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
#include <toadlet/tadpole/handler/platform/win32/Win32FontHandler.h>
#include <windows.h>
#include <stdlib.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{
namespace handler{

static String defaultCharacterSet("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789~!@#$%^&*()_+|{}:\"'<>?`-=\\/[];,. \t");

Win32FontHandler::Win32FontHandler(TextureManager *textureManager){
	mTextureManager=textureManager;
}

Resource *Win32FontHandler::load(const ResourceHandlerData *handlerData){
	FontData *fontData=(FontData*)handlerData;
	if(fontData==NULL){
		Error::nullPointer(Categories::TOADLET_TADPOLE,
			"invalid FontData");
		return NULL;
	}

	const wchar_t *charArray=NULL;
	int numChars=0;
	if(fontData->characterSet!=(char*)NULL){
		charArray=fontData->characterSet.wc_str();
		numChars=fontData->characterSet.length();
	}
	else{
		charArray=defaultCharacterSet.wc_str();
		numChars=defaultCharacterSet.length();
	}

	int i,j;

	LOGFONT logFont={0};
	logFont.lfWidth=fontData->width/2;
	logFont.lfHeight=fontData->height;
	logFont.lfWeight=FW_BOLD;
	logFont.lfCharSet=DEFAULT_CHARSET;
	logFont.lfPitchAndFamily=VARIABLE_PITCH | FF_SWISS;

	HFONT fontHandle=CreateFontIndirect(&logFont);
	if(fontHandle==NULL){
		Error::unknown(Categories::TOADLET_TADPOLE,
			String("CreateFontIndirect failed.  GetLastError():")+(unsigned int)GetLastError());
		return NULL;
	}

	HDC dc=GetDC(NULL);
	HDC cdc=CreateCompatibleDC(dc);

	// We need to set the font first, then set the color.  Otherwise it seems on Win32, the font overwrites the color with black
	HGDIOBJ lastObject=SelectObject(cdc,fontHandle);
	// SelectObject will mess with GetLastError(), so only pay attention to GetLastError() if SelectObject failed
	if(lastObject==NULL || lastObject==(HGDIOBJ)(-1)){
		DeleteDC(cdc);
		ReleaseDC(NULL,dc);

		DeleteObject(fontHandle);

		Error::unknown(Categories::TOADLET_TADPOLE,
			String("SelectObject failed.  GetLastError():")+(unsigned int)GetLastError());
		return NULL;
	}

	// Pure white is transparent
	uint32 color=0x00FFFFFE;
	COLORREF colorref=0xFF000000 | (color&0x00FF0000)>>16 | (color&0x0000FF00) | (color&0x000000FF)<<16;
	COLORREF oldTextColor=SetTextColor(cdc,color);
	COLORREF oldBackColor=SetBkColor(cdc,0);

	Collection<RECT> sizes(numChars);
	memset(&sizes[0],0,sizeof(RECT)*numChars);

	for(i=0;i<numChars;++i){
		TCHAR c=charArray[i];
		DrawText(cdc,&c,1,&sizes[i],DT_LEFT|DT_TOP|DT_CALCRECT);
	}

	int charCountHeight=Math::toInt(Math::sqrt(Math::fromInt(numChars)));
	int charCountWidth=Math::intCeil(Math::div(Math::fromInt(numChars),Math::fromInt(charCountHeight)));

	int charmapWidth=0;
	int charmapHeight=charCountWidth*fontData->height;

	for(i=0;i<charCountHeight;++i){
		int w=0;
		for(j=0;j<charCountWidth;++j){
			w+=sizes[i].right;
		}
		if(w>charmapWidth){
			charmapWidth=w;
		}
	}

	int textureWidth=Math::nextPowerOf2(charmapWidth);
	int textureHeight=Math::nextPowerOf2(charmapHeight);

	// Allocate a BITMAPINFO with a total of 3 DWORDs at the end
	BITMAPINFO *bitmapInfo=(BITMAPINFO*)malloc(sizeof(BITMAPINFO) + 3*sizeof(DWORD) - sizeof(RGBQUAD));
	bitmapInfo->bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	bitmapInfo->bmiHeader.biWidth=textureWidth;
	bitmapInfo->bmiHeader.biHeight=textureHeight;
	bitmapInfo->bmiHeader.biPlanes=1;
	bitmapInfo->bmiHeader.biBitCount=16;
	bitmapInfo->bmiHeader.biCompression=BI_BITFIELDS;
	bitmapInfo->bmiHeader.biSizeImage=bitmapInfo->bmiHeader.biHeight*bitmapInfo->bmiHeader.biWidth*bitmapInfo->bmiHeader.biBitCount/8;
	bitmapInfo->bmiHeader.biXPelsPerMeter=0;
	bitmapInfo->bmiHeader.biYPelsPerMeter=0;
	bitmapInfo->bmiHeader.biClrUsed=0;
	bitmapInfo->bmiHeader.biClrImportant=0;
	*(DWORD*)(&bitmapInfo->bmiColors[0])=0xF800;
	*(DWORD*)(&bitmapInfo->bmiColors[1])=0x07E0;
	*(DWORD*)(&bitmapInfo->bmiColors[2])=0x001F;

	char *buffer=NULL;
	HBITMAP bitmap=CreateDIBSection(cdc,bitmapInfo,DIB_RGB_COLORS,(void**)&buffer,NULL,0);
	delete bitmapInfo;
	if(bitmap==NULL || buffer==NULL){
		DeleteDC(cdc);
		ReleaseDC(NULL,dc);

		DeleteObject(fontHandle);

		Error::unknown(Categories::TOADLET_TADPOLE,
			String("CreateDIBSection failed.  GetLastError():")+(unsigned int)GetLastError());
		return NULL;
	}

	HBITMAP oldBitmap=(HBITMAP)SelectObject(cdc,bitmap);
	// SelectObject will mess with GetLastError(), so only pay attention to GetLastError() if SelectObject failed
	if(oldBitmap==NULL || oldBitmap==(HGDIOBJ)(-1)){
		DeleteObject(bitmap);

		DeleteDC(cdc);
		ReleaseDC(NULL,dc);

		DeleteObject(fontHandle);

		Error::unknown(Categories::TOADLET_TADPOLE,
			String("SelectObject failed.  GetLastError():")+(unsigned int)GetLastError());
		return NULL;
	}

	Collection<Font::Glyph*> glyphs(numChars);

	int x=0,y=0;
	for(i=0;i<numChars;++i){
		TCHAR c=charArray[i];
		RECT rect={x,y,x+sizes[i].right,y+sizes[i].bottom};
		DrawText(cdc,&c,1,&rect,DT_LEFT|DT_TOP);

		Font::Glyph *glyph=new Font::Glyph();
		glyph->advancex=Math::fromInt(sizes[i].right);
		glyph->advancey=0;
		glyph->x=x;
		glyph->y=y;
		glyph->width=sizes[i].right;
		glyph->height=sizes[i].bottom;
		glyph->offsetx=0;
		glyph->offsety=0;
		glyphs[i]=glyph;

		if(i%charCountWidth==charCountWidth-1){
			x=0;
			y+=fontData->height;
		}
		else{
			x+=sizes[i].right;
		}
	}

	#if !defined(TOADLET_PLATFORM_WINCE)
		GdiFlush();
	#endif

	Image::ptr image(new Image(Image::Dimension_D2,Image::Format_RGB_5_6_5,textureWidth,textureHeight));
	uint8 *imageData=image->getData();

	int imageStride=textureWidth*image->getPixelSize();
	int bitmapStride=((textureWidth*2+sizeof(DWORD)-1)>>2)<<2; // stride is in DWORDs

	// Flip the bitmap and copy it into the image
	for(i=0;i<textureHeight;++i){
		memcpy(imageData+imageStride*(textureHeight-i-1),buffer+bitmapStride*i,bitmapStride);
	}

	Texture::ptr texture=mTextureManager->createTexture(image);
	texture->setAutoGenerateMipMaps(false);

	Font *font=new Font(fontData->width,fontData->height,fontData->height,0,shared_static_cast<Texture>(mTextureManager->load(texture)),charArray,&glyphs[0],glyphs.size());

	SelectObject(cdc,oldBitmap);
	DeleteObject(bitmap);

	DeleteDC(cdc);
	ReleaseDC(NULL,dc);

	DeleteObject(fontHandle);

	return font;
}

}
}
}
