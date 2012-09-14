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

#ifndef TOADLET_FLICK_MFVIDEODEVICE_H
#define TOADLET_FLICK_MFVIDEODEVICE_H

#include <toadlet/flick/VideoDevice.h>
#include <toadlet/flick/VideoDeviceListener.h>
#include <toadlet/egg/WaitCondition.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

namespace toadlet{
namespace flick{

class TOADLET_API MFVideoDevice:public Object,public VideoDevice,public IMFSourceReaderCallback{
public:
	TOADLET_OBJECT(MFVideoDevice);

    STDMETHODIMP QueryInterface(REFIID iid, void** ppv);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();

    STDMETHODIMP OnReadSample(HRESULT hrStatus,DWORD dwStreamIndex,DWORD dwStreamFlags,LONGLONG llTimestamp,IMFSample *pSample);
    STDMETHODIMP OnEvent(DWORD, IMFMediaEvent *){return S_OK;}
    STDMETHODIMP OnFlush(DWORD){return S_OK;}

	MFVideoDevice();

	bool create();
	bool create(IMFActivate *device);
	void destroy();

	bool start();
	void update(int dt){}
	void stop();

	TextureFormat::ptr getTextureFormat(){return mFormat;}
	void setListener(VideoDeviceListener *listener){mListener=listener;}
	
protected:
	IMFActivate *findDevice();
	TextureFormat::ptr getTextureFormat(IMFMediaType *type);

	VideoDeviceListener *mListener;
	IMFSourceReader *mReader;
	IMFMediaType *mType;
	TextureFormat::ptr mFormat;
	int mRef;
	bool mRunning;
	Mutex mMutex;
	WaitCondition mCondition;
};

}
}

#endif
