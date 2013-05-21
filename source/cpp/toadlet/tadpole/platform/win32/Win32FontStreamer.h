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

#ifndef TOADLET_TADPOLE_WIN32FONTSTREAMER_H
#define TOADLET_TADPOLE_WIN32FONTSTREAMER_H

#include <toadlet/tadpole/Font.h>
#include <toadlet/tadpole/FontData.h>
#include <toadlet/tadpole/TextureManager.h>
#if !defined(TOADLET_PLATFORM_WINCE)
	#include <windows.h>
    #include <Gdiplus.h>
#endif

namespace toadlet{
namespace tadpole{

/// @brief  A lighter weight font handling class for the Win32 platform
class TOADLET_API Win32FontStreamer:public Object,public ResourceStreamer{
public:
	TOADLET_OBJECT(Win32FontStreamer);

	Win32FontStreamer(TextureManager *textureManager);
	virtual ~Win32FontStreamer();
	bool valid();

	Resource::ptr load(Stream::ptr stream,ResourceData *data,ProgressListener *listener);

	#if !defined(TOADLET_PLATFORM_WINCE)
		typedef Gdiplus::GpStatus(WINAPI *GdiplusStartup_)(OUT ULONG_PTR *token,const Gdiplus::GdiplusStartupInput *input,OUT Gdiplus::GdiplusStartupOutput *output);
		typedef VOID(WINAPI *GdiplusShutdown_)(ULONG_PTR token);
		typedef Gdiplus::GpStatus(WINGDIPAPI *GdipNewPrivateFontCollection_)(Gdiplus::GpFontCollection** fontCollection);
		typedef Gdiplus::GpStatus(WINGDIPAPI *GdipDeletePrivateFontCollection_)(Gdiplus::GpFontCollection** fontCollection);
		typedef Gdiplus::GpStatus(WINGDIPAPI *GdipPrivateAddMemoryFont_)(Gdiplus::GpFontCollection* fontCollection, GDIPCONST void* memory, INT length);
		typedef Gdiplus::GpStatus(WINGDIPAPI *GdipGetFontCollectionFamilyCount_)(Gdiplus::GpFontCollection* fontCollection, INT* numFound);
		typedef Gdiplus::GpStatus(WINGDIPAPI *GdipGetFontCollectionFamilyList_)(Gdiplus::GpFontCollection* fontCollection, INT numSought, Gdiplus::GpFontFamily* gpfamilies[], INT* numFound);
		typedef Gdiplus::GpStatus(WINGDIPAPI *GdipGetFamilyName_)( GDIPCONST Gdiplus::GpFontFamily* family,__out_ecount(LF_FACESIZE) LPWSTR name, LANGID language);

		GdiplusStartup_ GdiplusStartup;
		GdiplusShutdown_ GdiplusShutdown;
		GdipNewPrivateFontCollection_ GdipNewPrivateFontCollection;
		GdipDeletePrivateFontCollection_ GdipDeletePrivateFontCollection;
		GdipPrivateAddMemoryFont_ GdipPrivateAddMemoryFont;
		GdipGetFontCollectionFamilyCount_ GdipGetFontCollectionFamilyCount;
		GdipGetFontCollectionFamilyList_ GdipGetFontCollectionFamilyList;
		GdipGetFamilyName_ GdipGetFamilyName;
	#endif

protected:
	TextureManager *mTextureManager;
	#if !defined(TOADLET_PLATFORM_WINCE)
		HMODULE mLibrary;
		ULONG_PTR mToken;
	#endif
};

}
}

#endif
