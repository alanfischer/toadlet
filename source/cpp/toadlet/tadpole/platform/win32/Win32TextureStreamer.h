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

#ifndef TOADLET_TADPOLE_WIN32TEXTURESTREAMER_H
#define TOADLET_TADPOLE_WIN32TEXTURESTREAMER_H

#include <toadlet/peeper/Texture.h>
#include <toadlet/tadpole/TextureManager.h>
#if defined(TOADLET_PLATFORM_WINCE)
	#include <Imaging.h>
	#include <initguid.h>
	#include <imgguids.h>
#else
	#include <windows.h>
    #include <Gdiplus.h>

	using namespace Gdiplus;
	using namespace Gdiplus::DllExports;
#endif

namespace toadlet{
namespace tadpole{

/// @brief  A lighter weight texture handling class for the Win32 platform
class TOADLET_API Win32TextureStreamer:public Object,public ResourceStreamer{
public:
	TOADLET_OBJECT(Win32TextureStreamer);

	Win32TextureStreamer(TextureManager *textureManager);
	virtual ~Win32TextureStreamer();
	bool valid();

	Resource::ptr load(Stream::ptr in,ResourceData *data,ProgressListener *listener);

	#if !defined(TOADLET_PLATFORM_WINCE)
		typedef Status(WINAPI *GdiplusStartup_)(OUT ULONG_PTR *token,const GdiplusStartupInput *input,OUT GdiplusStartupOutput *output);
		typedef VOID(WINAPI *GdiplusShutdown_)(ULONG_PTR token);
		typedef GpStatus(WINGDIPAPI *GdipCreateBitmapFromStream_)(IStream* stream, GpBitmap **bitmap);
		typedef GpStatus(WINGDIPAPI *GdipDisposeImage_)(GpImage *image);
		typedef GpStatus(WINGDIPAPI *GdipImageGetFrameDimensionsCount_)(GpImage* image, UINT* count);
		typedef GpStatus(WINGDIPAPI *GdipImageGetFrameDimensionsList_)(GpImage* image, GUID* dimensionIDs, UINT count);
		typedef GpStatus(WINGDIPAPI *GdipImageGetFrameCount_)(GpImage *image, GDIPCONST GUID* dimensionID, UINT* count);
		typedef GpStatus(WINGDIPAPI *GdipImageSelectActiveFrame_)(GpImage *image, GDIPCONST GUID* dimensionID, UINT frameIndex);
		typedef GpStatus(WINGDIPAPI *GdipGetPropertyItemSize_)(GpImage *image, PROPID propId, UINT* size);
		typedef GpStatus(WINGDIPAPI *GdipGetPropertyItem_)(GpImage *image, PROPID propId,UINT propSize, PropertyItem* buffer);
		typedef GpStatus(WINGDIPAPI *GdipGetImageWidth_)(GpImage *image, UINT *width);
		typedef GpStatus(WINGDIPAPI *GdipGetImageHeight_)(GpImage *image, UINT *height);
		typedef GpStatus(WINGDIPAPI *GdipGetImagePixelFormat_)(GpImage *image, PixelFormat *format);
		typedef GpStatus(WINGDIPAPI *GdipBitmapLockBits_)(GpBitmap* bitmap, GDIPCONST GpRect* rect, UINT flags, PixelFormat format, BitmapData* lockedBitmapData);
		typedef GpStatus(WINGDIPAPI *GdipBitmapUnlockBits_)(GpBitmap* bitmap, BitmapData* lockedBitmapData);

		GdiplusStartup_ GdiplusStartup;
		GdiplusShutdown_ GdiplusShutdown;
		GdipCreateBitmapFromStream_ GdipCreateBitmapFromStream;
		GdipDisposeImage_ GdipDisposeImage;
		GdipImageGetFrameDimensionsCount_ GdipImageGetFrameDimensionsCount;
		GdipImageGetFrameDimensionsList_ GdipImageGetFrameDimensionsList;
		GdipImageGetFrameCount_ GdipImageGetFrameCount;
		GdipImageSelectActiveFrame_ GdipImageSelectActiveFrame;
		GdipGetPropertyItemSize_ GdipGetPropertyItemSize;
		GdipGetPropertyItem_ GdipGetPropertyItem;
		GdipGetImageWidth_ GdipGetImageWidth;
		GdipGetImageHeight_ GdipGetImageHeight;
		GdipGetImagePixelFormat_ GdipGetImagePixelFormat;
		GdipBitmapLockBits_ GdipBitmapLockBits;
		GdipBitmapUnlockBits_ GdipBitmapUnlockBits;
	#endif

protected:
	int getFormat(INT *gdiformat);

	TextureManager *mTextureManager;
	#if !defined(TOADLET_PLATFORM_WINCE)
		HMODULE mLibrary;
		ULONG_PTR mToken;
	#else
		IUnknown *mImagingFactory;
	#endif
};

}
}

#endif
