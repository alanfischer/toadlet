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

	HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize){return E_NOTIMPL;}
	HRESULT STDMETHODCALLTYPE CopyTo(IStream *pstm,ULARGE_INTEGER cb,ULARGE_INTEGER *pcbRead,ULARGE_INTEGER *pcbWritten){return E_NOTIMPL;}
	HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags){return E_NOTIMPL;}
	HRESULT STDMETHODCALLTYPE Revert(){return E_NOTIMPL;}
	HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER libOffset,ULARGE_INTEGER cb,DWORD dwLockType){return E_NOTIMPL;}
	HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER libOffset,ULARGE_INTEGER cb,DWORD dwLockType){return E_NOTIMPL;}
	HRESULT STDMETHODCALLTYPE Stat(STATSTG *pstatstg,DWORD grfStatFlag){return E_NOTIMPL;}
	HRESULT STDMETHODCALLTYPE Clone(IStream **ppstm){return E_NOTIMPL;}

protected:
	PointerCounter *mCounter;
	Stream::ptr mBase;
};

Win32TextureHandler::Win32TextureHandler(TextureManager *textureManager){
	mTextureManager=textureManager;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&mToken,&gdiplusStartupInput,NULL);
}

Win32TextureHandler::~Win32TextureHandler(){
	Gdiplus::GdiplusShutdown(mToken);
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
		Gdiplus::Bitmap *bitmap=Gdiplus::Bitmap::FromStream(stream);
		if(bitmap==NULL){
			return NULL;
		}
		if(bitmap->GetLastStatus()!=Gdiplus::Ok){
			delete bitmap;
			return NULL;
		}

		bitmap->LockBits(&rect,
		Logger::alert(String("W:")+bitmap->GetWidth()+" H:"+bitmap->GetHeight());
/*
 

    // create the destination Bitmap object

    Bitmap bitmap = new Bitmap(width, height, PixelFormat.Format32bppArgb);

 

    unsafe

    {

        // get a pointer to the raw bits

        RGBQUAD* pBits = (RGBQUAD*)(void*)dibsection.dsBm.bmBits;

 

        // copy each pixel manually

        for (int x = 0; x < dibsection.dsBmih.biWidth; x++)

            for (int y = 0; y < dibsection.dsBmih.biHeight; y++)

            {

                int offset = y * dibsection.dsBmih.biWidth + x;

                if (pBits[offset].rgbReserved != 0)

                {

                    bitmap.SetPixel(x, y, Color.FromArgb(pBits[offset].rgbReserved, pBits[offset].rgbRed, pBits[offset].rgbGreen, pBits[offset].rgbBlue));

                }

            }

    }

 

    return bitmap;
*/
	#endif

	return NULL;
}

}
}
}
