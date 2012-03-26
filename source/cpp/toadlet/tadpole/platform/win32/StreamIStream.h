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

#ifndef TOADLET_TADPOLE_STREAMISTREAM_H
#define TOADLET_TADPOLE_STREAMISTREAM_H

#include <toadlet/egg/io/Stream.h>
#include <toadlet/egg/Object.h>
#include <windows.h>

namespace toadlet{
namespace tadpole{

class StreamIStream:public Object,public IStream{
public:
	TOADLET_IPTR(StreamIStream);
	
	StreamIStream(Stream::ptr base):Object(),
		mBase(base)
	{}

	HRESULT STDMETHODCALLTYPE QueryInterface(const IID &riid,void **ppvObject){return E_NOINTERFACE;}
	ULONG STDMETHODCALLTYPE AddRef(){return retain();}
	ULONG STDMETHODCALLTYPE Release(){return release();}

	HRESULT STDMETHODCALLTYPE Read(void *pv,ULONG cb,ULONG *pcbRead){
		int amount=mBase->read((byte*)pv,cb);
		if(pcbRead!=NULL){*pcbRead=amount;}
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE Write(const void *pv,ULONG cb,ULONG *pcbWritten){
		int amount=mBase->write((const tbyte*)pv,cb);
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
	Stream::ptr mBase;
};

}
}

#endif
