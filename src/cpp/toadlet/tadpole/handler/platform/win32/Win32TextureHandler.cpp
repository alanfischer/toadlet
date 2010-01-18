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

#if defined(TOADLET_PLATFORM_WINCE)
	#include <Imaging.h>
#else
	#include <OleCtl.h>
	#include <gdiplus.h>
	#pragma comment(lib,"gdiplus.lib")
	using namespace Gdiplus;
#endif

using namespace toadlet::egg;
using namespace toadlet::egg::image;
using namespace toadlet::egg::io;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{
namespace handler{

class StreamIStream:public IStream{
public:
	TOADLET_INTRUSIVE_POINTERS(StreamIStream);
	
	StreamIStream(Stream::ptr base):
		mCounter(new PointerCounter(0)),
		mBase(base)
	{}

	PointerCounter *getCounter(){return mCounter;}

	HRESULT STDMETHODCALLTYPE QueryInterface(const IID &riid,void **ppvObject){return E_NOINTERFACE;}
	ULONG STDMETHODCALLTYPE AddRef(){return mCounter->incSharedCount();}
	ULONG STDMETHODCALLTYPE Release(){return mCounter->decSharedCount();}

	HRESULT STDMETHODCALLTYPE Read(void *pv,ULONG cb,ULONG *pcbRead){
		int amount=mBase->read((byte*)pv,cb);
		if(pcbRead!=NULL){*pcbRead=amount;}
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE Write(const void *pv,ULONG cb,ULONG *pcbWritten){
		int amount=mBase->write((const byte*)pv,cb);
		if(pcbWritten!=NULL){*pcbWritten=amount;}
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove,DWORD dwOrigin,ULARGE_INTEGER *plibNewPosition){
		bool result=false;
		if(dwOrigin==STREAM_SEEK_SET){
			result=mBase->seek(dlibMove.QuadPart);
		}
		else if(dwOrigin==STREAM_SEEK_CUR){
			int position=mBase->position();
			result=mBase->seek(position+dlibMove.QuadPart);
		}
		else if(dwOrigin==STREAM_SEEK_END){
			int length=mBase->length();
			result=mBase->seek(length+dlibMove.QuadPart);
		}
		if(plibNewPosition!=NULL){
			(*plibNewPosition).QuadPart=mBase->position();
		}
		return result?S_OK:E_FAIL;
	}

	HRESULT STDMETHODCALLTYPE Stat(STATSTG *pstatstg,DWORD grfStatFlag){
		memset(pstatstg,0,sizeof(STATSTG));
		pstatstg->type=STGTY_STREAM;
		pstatstg->cbSize.QuadPart=mBase->length();
		pstatstg->clsid=CLSID_NULL;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize){return E_NOTIMPL;}
	HRESULT STDMETHODCALLTYPE CopyTo(IStream *pstm,ULARGE_INTEGER cb,ULARGE_INTEGER *pcbRead,ULARGE_INTEGER *pcbWritten){return E_NOTIMPL;}
	HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags){return E_NOTIMPL;}
	HRESULT STDMETHODCALLTYPE Revert(){return E_NOTIMPL;}
	HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER libOffset,ULARGE_INTEGER cb,DWORD dwLockType){return E_NOTIMPL;}
	HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER libOffset,ULARGE_INTEGER cb,DWORD dwLockType){return E_NOTIMPL;}
	HRESULT STDMETHODCALLTYPE Clone(IStream **ppstm){return E_NOTIMPL;}

protected:
	PointerCounter *mCounter;
	Stream::ptr mBase;
};

Win32TextureHandler::Win32TextureHandler(TextureManager *textureManager){
	mTextureManager=textureManager;
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&mToken,&gdiplusStartupInput,NULL);
}

Win32TextureHandler::~Win32TextureHandler(){
	GdiplusShutdown(mToken);
}

Resource::ptr Win32TextureHandler::load(Stream::ptr in,const ResourceHandlerData *handlerData){
	StreamIStream::ptr stream(new StreamIStream(in));
	HRESULT hr=0;

	#if defined(TOADLET_PLATFORM_WINCE)
		IImage *image=NULL;
		hr=CreateImageFromStream(stream,&image);
		if(FAILED(hr)){
			return NULL;
		}

		CreateBitmapFromImage(
	#else
		Bitmap *bitmap=Bitmap::FromStream(stream);
		if(bitmap==NULL){
			return NULL;
		}

		int status=bitmap->GetLastStatus();
		if(status!=Ok){
			delete bitmap;
			Logger::alert(String("Status:")+status);
			return NULL;
		}

		Rect rect(0,0,bitmap->GetWidth(),bitmap->GetHeight());
		PixelFormat gdiformat=bitmap->GetPixelFormat();
		int format=0;
		switch(format){
			case(PixelFormat16bppARGB1555):
				format=Texture::Format_BGRA_5_5_5_1;
			break;
			case(PixelFormat16bppRGB565):
				format=Texture::Format_BGR_5_6_5;
			break;
			case(PixelFormat24bppRGB):
				format=Texture::Format_BGR_8;
			break;
			case(PixelFormat32bppARGB):
				format=Texture::Format_BGRA_8;
			break;
			default:
				if((format&PixelFormatAlpha)>0){
					gdiformat=PixelFormat32bppARGB;
					format=Texture::Format_BGRA_8;
				}
				else{
					gdiformat=PixelFormat24bppRGB;
					format=Texture::Format_BGR_8;
				}
			break;
		}

		image::Image::ptr image(new image::Image(image::Image::Dimension_D2,format,bitmap->GetWidth(),bitmap->GetHeight()));

		BitmapData data;
		bitmap->LockBits(&rect,ImageLockModeRead,gdiformat,&data);

		int i;
		for(i=0;i<image->getHeight();++i){
			memcpy(image->getData()+image->getWidth()*image->getPixelSize()*i,((uint8*)data.Scan0)+data.Stride*(bitmap->GetHeight()-i-1),image->getWidth()*image->getPixelSize());
		}

		bitmap->UnlockBits(&data);

		return mTextureManager->createTexture(image);
	#endif

	return NULL;
}

}
}
}
