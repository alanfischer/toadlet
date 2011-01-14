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

/// @todo: This handler currently looks pretty poor.

#define _WIN32_WINNT 0x0500

#include <toadlet/egg/Error.h>
#include <toadlet/egg/io/MemoryStream.h>
#include <toadlet/tadpole/handler/platform/win32/Win32FontHandler.h>
#include <stdlib.h>

#if !defined(TOADLET_PLATFORM_WINCE)
	#include <OleCtl.h>
	#include <gdiplus.h>
	#pragma comment(lib,"gdiplus.lib")
#endif

using namespace toadlet::egg;
using namespace toadlet::egg::image;
using namespace toadlet::egg::io;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{
namespace handler{

Win32FontHandler::Win32FontHandler(TextureManager *textureManager):
	mTextureManager(NULL)
	#if !defined(TOADLET_PLATFORM_WINCE)
		,mToken(0)
	#endif
{
	mTextureManager=textureManager;
	#if !defined(TOADLET_PLATFORM_WINCE)
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		Gdiplus::GdiplusStartup(&mToken,&gdiplusStartupInput,NULL);
	#endif
}

bool Win32FontHandler::valid(){
	#if !defined(TOADLET_PLATFORM_WINCE)
		return true;
	#else
		return false;
	#endif
}

Win32FontHandler::~Win32FontHandler(){
	#if !defined(TOADLET_PLATFORM_WINCE)
		Gdiplus::GdiplusShutdown(mToken);
	#endif
}

Resource::ptr Win32FontHandler::load(Stream::ptr stream,const ResourceHandlerData *handlerData){
	int i=0,j=0;

	FontData *fontData=(FontData*)handlerData;
	if(fontData==NULL){
		Error::nullPointer(Categories::TOADLET_TADPOLE,
			"invalid FontData");
		return NULL;
	}

	const wchar_t *charArray=fontData->characterSet.wc_str();
	int numChars=numChars=fontData->characterSet.length();

	MemoryStream::ptr memoryStream(new MemoryStream(stream));

	String name;
	#if !defined(TOADLET_PLATFORM_WINCE)
		// This is hacky, but for now we grab the font name by loading it using Gdiplus
		// Then we need to actually load the font using regular Gdi.  Could be replaced with an all Gdiplus font loader.
		Gdiplus::PrivateFontCollection *collection=new Gdiplus::PrivateFontCollection();
		collection->AddMemoryFont(memoryStream->getOriginalDataPointer(),memoryStream->length());

		Gdiplus::FontFamily fontFamily;
		int numFamilys=0;
		collection->GetFamilies(1,&fontFamily,&numFamilys);
		WCHAR wname[1024];
		fontFamily.GetFamilyName(wname);
		name=wname;
		
		// We're done with the collection, so delete it
		delete collection;
		collection=NULL;

		DWORD amount=0;
		HANDLE handle=AddFontMemResourceEx(memoryStream->getOriginalDataPointer(),memoryStream->length(),0,&amount);
	#else
		/// @todo: Implement this
	#endif

	HDC dc=GetDC(NULL);
	HDC cdc=CreateCompatibleDC(dc);

	LOGFONT logFont={0};
	logFont.lfWidth=0;
	logFont.lfHeight=Math::fromInt(fontData->pointSize);
	logFont.lfWeight=FW_BOLD;
	logFont.lfCharSet=DEFAULT_CHARSET;
	logFont.lfPitchAndFamily=VARIABLE_PITCH;
	memcpy(logFont.lfFaceName,name.c_str(),name.length());

	HFONT fontHandle=CreateFontIndirect(&logFont);
	if(fontHandle==NULL){
		DeleteDC(cdc);
		ReleaseDC(NULL,dc);

		Error::unknown(Categories::TOADLET_TADPOLE,
			String("CreateFontIndirect failed.  GetLastError():")+(unsigned int)GetLastError());
		return NULL;
	}

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
	int pad=2;

	int charmapWidth=0;
	int charmapHeight=charCountWidth*Math::fromInt(fontData->pointSize+pad);

	for(i=0;i<charCountHeight;++i){
		int w=0;
		for(j=0;j<charCountWidth;++j){
			w+=sizes[i].right+pad;
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
		glyph->offsety=-fontData->pointSize; // It appears the win32 fonts are 1 point size lower than the TTF (standard) fonts, so raise it up.
		glyphs[i]=glyph;

		if(i%charCountWidth==charCountWidth-1){
			x=0;
			y+=fontData->pointSize;
		}
		else{
			x+=glyph->width+pad;
		}
	}

	#if !defined(TOADLET_PLATFORM_WINCE)
		RemoveFontMemResourceEx(handle);
		GdiFlush();
	#endif

	Image::ptr image(Image::createAndReallocate(Image::Dimension_D2,Image::Format_A_8,textureWidth,textureHeight));
	if(image==NULL){
		return NULL;
	}
	
	tbyte *imageData=image->getData();
	int imageStride=image->getRowPitch();
	int bitmapStride=((textureWidth*2+sizeof(DWORD)-1)>>2)<<2; // stride is in DWORDs

	// Flip the bitmap and copy it into the image
	for(i=0;i<textureHeight;++i){
		for(j=0;j<textureWidth;++j){
			uint16 *s=(uint16*)(buffer+bitmapStride*i+j*2);
			tbyte *d=(tbyte*)(imageData+imageStride*(textureHeight-i-1)+j);
			*d=(tbyte)((*s)&0x1F)<<3;
		}
	}

	Font::ptr font(new Font(fontData->pointSize,0,shared_static_cast<Texture>(mTextureManager->createTexture(image)),charArray,&glyphs[0],glyphs.size()));

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
