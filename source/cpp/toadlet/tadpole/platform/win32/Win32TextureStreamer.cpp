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
#include <toadlet/egg/Log.h>
#include <toadlet/egg/Error.h>
#include <toadlet/tadpole/platform/win32/Win32TextureStreamer.h>
#include <toadlet/tadpole/platform/win32/StreamIStream.h>

#if defined(TOADLET_PLATFORM_WINCE)
	#include <Imaging.h>
	#include <initguid.h>
	#include <imgguids.h>
#else
	#include <OleCtl.h>
	#include <gdiplus.h>
	#pragma comment(lib,"gdiplus.lib")
	using namespace Gdiplus;
#endif

namespace toadlet{
namespace tadpole{

Win32TextureStreamer::Win32TextureStreamer(TextureManager *textureManager):
	mTextureManager(NULL),
	#if !defined(TOADLET_PLATFORM_WINCE)
		mToken(0)
	#else
		mImagingFactory(NULL)
	#endif
{
	mTextureManager=textureManager;
	#if !defined(TOADLET_PLATFORM_WINCE)
		GdiplusStartupInput gdiplusStartupInput;
		GdiplusStartup(&mToken,&gdiplusStartupInput,NULL);
	#else
		HRESULT hr=CoInitializeEx(NULL,COINIT_MULTITHREADED);
		if(hr==S_OK){
			IImagingFactory *imagingFactory=NULL;
			hr=CoCreateInstance(CLSID_ImagingFactory,NULL,CLSCTX_INPROC_SERVER,IID_IImagingFactory,(void**)&imagingFactory);
			if(SUCCEEDED(hr) && imagingFactory!=NULL){
				mImagingFactory=imagingFactory;
			}
		}
	#endif
}

Win32TextureStreamer::~Win32TextureStreamer(){
	#if !defined(TOADLET_PLATFORM_WINCE)
		GdiplusShutdown(mToken);
	#else
		if(mImagingFactory!=NULL){
			mImagingFactory->Release();
		}
		CoUninitialize();
	#endif
}

bool Win32TextureStreamer::valid(){
	#if !defined(TOADLET_PLATFORM_WINCE)
		return true;
	#else
		return mImagingFactory!=NULL;
	#endif
}

Resource::ptr Win32TextureStreamer::load(Stream::ptr in,ResourceData *data,ProgressListener *listener){
	Log::debug(Categories::TOADLET_TADPOLE,"Win32TextureStreamer::load");

	Texture::ptr texture=NULL;
	int usage=(data!=NULL)?((TextureData*)data)->usage:0;

	StreamIStream::ptr stream(new StreamIStream(in));
	HRESULT hr=0;

	#if defined(TOADLET_PLATFORM_WINCE)
		IImage *image=NULL;
		hr=((IImagingFactory*)mImagingFactory)->CreateImageFromStream(stream,&image);
		if(FAILED(hr) || image==NULL){
			Error::unknown("CreateImageFromStream failed");
			return NULL;
		}

		IBitmapImage *bitmap=NULL;
		hr=((IImagingFactory*)mImagingFactory)->CreateBitmapFromImage(image,0,0,0,InterpolationHintDefault,&bitmap);
		if(FAILED(hr) || bitmap==NULL){
			Error::unknown("CreateBitmapFromImage failed");
			return NULL;
		}

		PixelFormat gdiformat=PixelFormatUndefined;
		hr=bitmap->GetPixelFormatID(&gdiformat);
		int pixelFormat=getFormat(&gdiformat);
		SIZE size={0};
		hr=bitmap->GetSize(&size);
		TextureFormat::ptr textureFormat(new TextureFormat(TextureFormat::Dimension_D2,pixelFormat,size.cx,size.cy,1,0);
		tbyte *textureData=new tbyte[textureFormat->getDataSize()];
		
		RECT rect={0};
		rect.right=size.cx;
		rect.bottom=size.cy;
		BitmapData bitmapData;
		bitmap->LockBits(&rect,ImageLockModeRead,gdiformat,&bitmapData);

		int i;
		for(i=0;i<textureFormat->getHeight();++i){
			memcpy(textureData+textureFormat->getXPitch()*i,((uint8*)bitmapData.Scan0)+bitmapData.Stride*(size.cy-i-1),textureFormat->getXPitch());
		}

		bitmap->UnlockBits(&bitmapData);

		texture=mTextureManager->createTexture(usage,textureFormat,textureData);

		delete[] textureData;
	#else
		Bitmap *bitmap=Bitmap::FromStream(stream);
		if(bitmap==NULL){
			Error::unknown("Bitmap::FromStream failed");
			return NULL;
		}

		int status=bitmap->GetLastStatus();
		if(status!=Ok){
			delete bitmap;
			Error::unknown("Bitmap::FromStream errored");
			return NULL;
		}

		int dimensionCount=bitmap->GetFrameDimensionsCount();
		GUID *dimensionIDs=new GUID[dimensionCount];
		bitmap->GetFrameDimensionsList(dimensionIDs,dimensionCount);
		int frameCount=bitmap->GetFrameCount(&dimensionIDs[0]);

		int propertySize=bitmap->GetPropertyItemSize(PropertyTagFrameDelay);
		PropertyItem *propertyItem=NULL;
		if(propertySize>0){
			propertyItem=(PropertyItem*)malloc(propertySize);
			bitmap->GetPropertyItem(PropertyTagFrameDelay,propertySize,propertyItem);
		}

		PixelFormat gdiformat=bitmap->GetPixelFormat();
		int pixelFormat=getFormat(&gdiformat);
		TextureFormat::ptr textureFormat(new TextureFormat(TextureFormat::Dimension_D2,pixelFormat,bitmap->GetWidth(),bitmap->GetHeight(),1,0));
		tbyte *textureData=new tbyte[textureFormat->getDataSize()];
		frameCount=1;

		int i;
		for(i=0;i<frameCount;++i){
			bitmap->SelectActiveFrame(&dimensionIDs[0],i);
			
			Rect rect(0,0,bitmap->GetWidth(),bitmap->GetHeight());
			BitmapData bitmapData;
			bitmap->LockBits(&rect,ImageLockModeRead,gdiformat,&bitmapData);

			int j;
			for(j=0;j<textureFormat->getHeight();++j){
				memcpy(textureData+textureFormat->getXPitch()*j,((uint8*)bitmapData.Scan0)+bitmapData.Stride*(textureFormat->getHeight()-j-1),textureFormat->getXPitch());
			}

			bitmap->UnlockBits(&bitmapData);

			int delay=0;
			if(propertyItem!=NULL){
				delay=((UINT*)propertyItem[0].value)[i]*10;
			}
			if(delay<100){
				delay=100;
			}
		}

		delete bitmap;
		delete dimensionIDs;
		free(propertyItem);

		texture=mTextureManager->createTexture(usage,textureFormat,textureData);

		delete[] textureData;
	#endif

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
