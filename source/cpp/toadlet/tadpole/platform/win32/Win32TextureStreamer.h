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
#include <windows.h>
#include <Gdiplus.h>

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

	typedef Gdiplus::GpStatus(WINAPI *GdiplusStartup_)(OUT ULONG_PTR *token,const Gdiplus::GdiplusStartupInput *input,OUT Gdiplus::GdiplusStartupOutput *output);
	typedef VOID(WINAPI *GdiplusShutdown_)(ULONG_PTR token);
	typedef Gdiplus::GpStatus(WINGDIPAPI *GdipCreateBitmapFromStream_)(IStream* stream, Gdiplus::GpBitmap **bitmap);
	typedef Gdiplus::GpStatus(WINGDIPAPI *GdipDisposeImage_)(Gdiplus::GpImage *image);
	typedef Gdiplus::GpStatus(WINGDIPAPI *GdipImageGetFrameDimensionsCount_)(Gdiplus::GpImage* image, UINT* count);
	typedef Gdiplus::GpStatus(WINGDIPAPI *GdipImageGetFrameDimensionsList_)(Gdiplus::GpImage* image, GUID* dimensionIDs, UINT count);
	typedef Gdiplus::GpStatus(WINGDIPAPI *GdipImageGetFrameCount_)(Gdiplus::GpImage *image, GDIPCONST GUID* dimensionID, UINT* count);
	typedef Gdiplus::GpStatus(WINGDIPAPI *GdipImageSelectActiveFrame_)(Gdiplus::GpImage *image, GDIPCONST GUID* dimensionID, UINT frameIndex);
	typedef Gdiplus::GpStatus(WINGDIPAPI *GdipGetPropertyItemSize_)(Gdiplus::GpImage *image, PROPID propId, UINT* size);
	typedef Gdiplus::GpStatus(WINGDIPAPI *GdipGetPropertyItem_)(Gdiplus::GpImage *image, PROPID propId,UINT propSize, Gdiplus::PropertyItem* buffer);
	typedef Gdiplus::GpStatus(WINGDIPAPI *GdipGetImageWidth_)(Gdiplus::GpImage *image, UINT *width);
	typedef Gdiplus::GpStatus(WINGDIPAPI *GdipGetImageHeight_)(Gdiplus::GpImage *image, UINT *height);
	typedef Gdiplus::GpStatus(WINGDIPAPI *GdipGetImagePixelFormat_)(Gdiplus::GpImage *image, Gdiplus::PixelFormat *format);
	typedef Gdiplus::GpStatus(WINGDIPAPI *GdipBitmapLockBits_)(Gdiplus::GpBitmap* bitmap, GDIPCONST Gdiplus::GpRect* rect, UINT flags, Gdiplus::PixelFormat format, Gdiplus::BitmapData* lockedBitmapData);
	typedef Gdiplus::GpStatus(WINGDIPAPI *GdipBitmapUnlockBits_)(Gdiplus::GpBitmap* bitmap, Gdiplus::BitmapData* lockedBitmapData);

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

protected:
	int getFormat(INT *gdiformat);

	TextureManager *mTextureManager;
	HMODULE mLibrary;
	ULONG_PTR mToken;
};

}
}

#endif
