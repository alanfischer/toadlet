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

#include <toadlet/egg/Collection.h>
#include <toadlet/egg/platform/win32/StreamIStream.h>
#include <toadlet/tadpole/platform/win32/Win32TextureStreamer.h>

using namespace Gdiplus;

namespace toadlet{
namespace tadpole{

Win32TextureStreamer::Win32TextureStreamer(TextureManager *textureManager):
	mTextureManager(NULL),
	mToken(0)
{
	mLibrary=LoadLibrary("gdiplus.dll");
	if(mLibrary==0){
		Error::libraryNotFound(Categories::TOADLET_TADPOLE,
			"Win32TextureStreamer: Error loading gdiplus.dll");
		return;
	}

	GdiplusStartup=(GdiplusStartup_)GetProcAddress(mLibrary,"GdiplusStartup");
	GdiplusShutdown=(GdiplusShutdown_)GetProcAddress(mLibrary,"GdiplusShutdown");
	GdipCreateBitmapFromStream=(GdipCreateBitmapFromStream_)GetProcAddress(mLibrary,"GdipCreateBitmapFromStream");
	GdipDisposeImage=(GdipDisposeImage_)GetProcAddress(mLibrary,"GdipDisposeImage");
	GdipImageGetFrameDimensionsCount=(GdipImageGetFrameDimensionsCount_)GetProcAddress(mLibrary,"GdipImageGetFrameDimensionsCount");
	GdipImageGetFrameDimensionsList=(GdipImageGetFrameDimensionsList_)GetProcAddress(mLibrary,"GdipImageGetFrameDimensionsList");
	GdipImageGetFrameCount=(GdipImageGetFrameCount_)GetProcAddress(mLibrary,"GdipImageGetFrameCount");
	GdipImageSelectActiveFrame=(GdipImageSelectActiveFrame_)GetProcAddress(mLibrary,"GdipImageSelectActiveFrame");
	GdipGetPropertyItemSize=(GdipGetPropertyItemSize_)GetProcAddress(mLibrary,"GdipGetPropertyItemSize");
	GdipGetPropertyItem=(GdipGetPropertyItem_)GetProcAddress(mLibrary,"GdipGetPropertyItem");
	GdipGetImageWidth=(GdipGetImageWidth_)GetProcAddress(mLibrary,"GdipGetImageWidth");
	GdipGetImageHeight=(GdipGetImageHeight_)GetProcAddress(mLibrary,"GdipGetImageHeight");
	GdipGetImagePixelFormat=(GdipGetImagePixelFormat_)GetProcAddress(mLibrary,"GdipGetImagePixelFormat");
	GdipBitmapLockBits=(GdipBitmapLockBits_)GetProcAddress(mLibrary,"GdipBitmapLockBits");
	GdipBitmapUnlockBits=(GdipBitmapUnlockBits_)GetProcAddress(mLibrary,"GdipBitmapUnlockBits");

	mTextureManager=textureManager;
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&mToken,&gdiplusStartupInput,NULL);
}

Win32TextureStreamer::~Win32TextureStreamer(){
	GdiplusShutdown(mToken);

	if(mLibrary!=0){
		FreeLibrary(mLibrary);
		mLibrary=0;
	}
}

bool Win32TextureStreamer::valid(){
	return mLibrary!=0;
}

Resource::ptr Win32TextureStreamer::load(Stream::ptr in,ResourceData *data,ProgressListener *listener){
	Log::debug(Categories::TOADLET_TADPOLE,"Win32TextureStreamer::load");

	Texture::ptr texture=NULL;
	int usage=(data!=NULL)?((TextureData*)data)->usage:0;

	StreamIStream::ptr stream=new StreamIStream(in);
	HRESULT hr=0;

	GpBitmap *bitmap=NULL;
	GpStatus status=GdipCreateBitmapFromStream(stream,&bitmap);
	if(bitmap==NULL){
		Error::unknown("GdipCreateBitmapFromStream failed");
		return NULL;
	}

	if(status!=Ok){
		GdipDisposeImage(bitmap);
		Error::unknown("GdipCreateBitmapFromStream errored");
		return NULL;
	}

	UINT dimensionCount=0;
	GdipImageGetFrameDimensionsCount(bitmap,&dimensionCount);
	GUID *dimensionIDs=new GUID[dimensionCount];
	GdipImageGetFrameDimensionsList(bitmap,dimensionIDs,dimensionCount);
	UINT frameCount=0;
	GdipImageGetFrameCount(bitmap,&dimensionIDs[0],&frameCount);

	UINT propertySize=0;
	GdipGetPropertyItemSize(bitmap,PropertyTagFrameDelay,&propertySize);
	PropertyItem *propertyItem=NULL;
	if(propertySize>0){
		propertyItem=(PropertyItem*)malloc(propertySize);
		GdipGetPropertyItem(bitmap,PropertyTagFrameDelay,propertySize,propertyItem);
	}

	PixelFormat gdiformat;
	GdipGetImagePixelFormat(bitmap,&gdiformat);
	int pixelFormat=getFormat(&gdiformat);
	UINT width=0,height=0;
	GdipGetImageWidth(bitmap,&width);
	GdipGetImageHeight(bitmap,&height);
	TextureFormat::ptr textureFormat=new TextureFormat(TextureFormat::Dimension_D2,pixelFormat,width,height,1,0);
	tbyte *textureData=new tbyte[textureFormat->getDataSize()];
	frameCount=1;

	int i;
	for(i=0;i<frameCount;++i){
		GdipImageSelectActiveFrame(bitmap,&dimensionIDs[0],i);
			
		GdipGetImageWidth(bitmap,&width);
		GdipGetImageHeight(bitmap,&height);
		Rect rect(0,0,width,height);
		BitmapData bitmapData;
		GdipBitmapLockBits(bitmap,&rect,ImageLockModeRead,gdiformat,&bitmapData);

		int j;
		for(j=0;j<textureFormat->getHeight();++j){
			memcpy(textureData+textureFormat->getXPitch()*j,((uint8*)bitmapData.Scan0)+bitmapData.Stride*(textureFormat->getHeight()-j-1),textureFormat->getXPitch());
		}

		GdipBitmapUnlockBits(bitmap,&bitmapData);

		int delay=0;
		if(propertyItem!=NULL){
			delay=((UINT*)propertyItem[0].value)[i]*10;
		}
		if(delay<100){
			delay=100;
		}
	}

	GdipDisposeImage(bitmap);
	delete[] dimensionIDs;
	free(propertyItem);

	texture=mTextureManager->createTexture(usage,textureFormat,textureData);

	delete[] textureData;

	return texture;
}

int Win32TextureStreamer::getFormat(PixelFormat *gdiformat){
	switch(*gdiformat){
		case PixelFormat16bppARGB1555:
			return TextureFormat::Format_BGRA_5_5_5_1;
		break;
		case PixelFormat16bppRGB565:
			return TextureFormat::Format_BGR_5_6_5;
		break;
		case PixelFormat24bppRGB:
			return TextureFormat::Format_BGR_8;
		break;
		case PixelFormat32bppARGB:
			return TextureFormat::Format_BGRA_8;
		break;
		default:
			if(((*gdiformat)&PixelFormatAlpha)>0){
				*gdiformat=PixelFormat32bppARGB;
				return TextureFormat::Format_BGRA_8;
			}
			else{
				*gdiformat=PixelFormat24bppRGB;
				return TextureFormat::Format_BGR_8;
			}
		break;
	}
}

}
}
