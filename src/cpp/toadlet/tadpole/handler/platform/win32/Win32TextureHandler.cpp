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
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/tadpole/handler/platform/win32/Win32TextureHandler.h>
#include <toadlet/tadpole/handler/platform/win32/StreamIStream.h>

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
namespace handler{

Win32TextureHandler::Win32TextureHandler(TextureManager *textureManager):
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

Win32TextureHandler::~Win32TextureHandler(){
	#if !defined(TOADLET_PLATFORM_WINCE)
		GdiplusShutdown(mToken);
	#else
		if(mImagingFactory!=NULL){
			mImagingFactory->Release();
		}
		CoUninitialize();
	#endif
}

bool Win32TextureHandler::valid(){
	#if !defined(TOADLET_PLATFORM_WINCE)
		return true;
	#else
		return mImagingFactory!=NULL;
	#endif
}

Resource::ptr Win32TextureHandler::load(Stream::ptr in,ResourceData *data,ProgressListener *listener){
	Logger::debug(Categories::TOADLET_TADPOLE,"Win32TextureHandler::load");

	Texture::ptr texture=NULL;

	StreamIStream::ptr stream(new StreamIStream(in));
	HRESULT hr=0;

	#if defined(TOADLET_PLATFORM_WINCE)
		IImage *iimage=NULL;
		hr=((IImagingFactory*)mImagingFactory)->CreateImageFromStream(stream,&iimage);
		if(FAILED(hr) || iimage==NULL){
			Error::unknown("CreateImageFromStream failed");
			return NULL;
		}

		IBitmapImage *bitmap=NULL;
		hr=((IImagingFactory*)mImagingFactory)->CreateBitmapFromImage(iimage,0,0,0,InterpolationHintDefault,&bitmap);
		if(FAILED(hr) || bitmap==NULL){
			Error::unknown("CreateBitmapFromImage failed");
			return NULL;
		}

		PixelFormat gdiformat=PixelFormatUndefined;
		hr=bitmap->GetPixelFormatID(&gdiformat);
		int format=getFormat(&gdiformat);
		SIZE size={0};
		hr=bitmap->GetSize(&size);
		image::Image::ptr image(image::Image::createAndReallocate(image::Image::Dimension_D2,format,size.cx,size.cy));
		if(image==NULL){
			return NULL;
		}
		
		RECT rect={0};
		rect.right=size.cx;
		rect.bottom=size.cy;
		BitmapData data;
		bitmap->LockBits(&rect,ImageLockModeRead,gdiformat,&data);

		int i;
		for(i=0;i<image->getHeight();++i){
			memcpy(image->getData()+image->getRowPitch()*i,((uint8*)data.Scan0)+data.Stride*(size.cy-i-1),image->getWidth()*image->getPixelSize());
		}

		bitmap->UnlockBits(&data);

		texture=mTextureManager->createTexture(image);
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

		Collection<image::Image::ptr> images;
		Collection<int> delayMilliseconds;
		int i;
		for(i=0;i<frameCount;++i){
			bitmap->SelectActiveFrame(&dimensionIDs[0],i);
			PixelFormat gdiformat=bitmap->GetPixelFormat();
			int format=getFormat(&gdiformat);

			image::Image::ptr image(image::Image::createAndReallocate(image::Image::Dimension_D2,format,bitmap->GetWidth(),bitmap->GetHeight()));
			if(image==NULL){
				return NULL;
			}
			
			Rect rect(0,0,bitmap->GetWidth(),bitmap->GetHeight());
			BitmapData data;
			bitmap->LockBits(&rect,ImageLockModeRead,gdiformat,&data);

			int j;
			for(j=0;j<image->getHeight();++j){
				memcpy(image->getData()+image->getRowPitch()*j,((uint8*)data.Scan0)+data.Stride*(bitmap->GetHeight()-j-1),image->getWidth()*image->getPixelSize());
			}

			bitmap->UnlockBits(&data);

			images.add(image);

			int delay=0;
			if(propertyItem!=NULL){
				delay=((UINT*)propertyItem[0].value)[i]*10;
			}
			if(delay<100){
				delay=100;
			}
			delayMilliseconds.add(delay);
		}

		delete dimensionIDs;
		free(propertyItem);

		if(images.size()==0){
			return NULL;
		}
		else if(images.size()==1){
			texture=mTextureManager->createTexture(image::Image::ptr(images[0]));
		}
 		else{
/// @todo: revive this somehow, probably as a 3d texture
//			SequenceTexture::ptr sequence(new SequenceTexture(Texture::Dimension_D2,images.size()));
//			int i;
//			for(i=0;i<images.size();++i){
//				sequence->setTexture(i,mTextureManager->createTexture(image::Image::ptr(images[i])),Math::fromMilli(delayMilliseconds[i]));
//			}
//			texture=shared_static_cast<Texture>(sequence);
		}
	#endif

	return texture;
}

int Win32TextureHandler::getFormat(PixelFormat *gdiformat){
	switch(*gdiformat){
		case(PixelFormat16bppARGB1555):
			return TextureFormat::Format_BGRA_5_5_5_1;
		break;
		case(PixelFormat16bppRGB565):
			return TextureFormat::Format_BGR_5_6_5;
		break;
		case(PixelFormat24bppRGB):
			return TextureFormat::Format_BGR_8;
		break;
		case(PixelFormat32bppARGB):
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
}
