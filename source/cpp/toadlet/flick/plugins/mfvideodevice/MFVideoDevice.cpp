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

#define INITGUID
#include "MFVideoDevice.h"
const GUID GUID_NULL = { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } }; 

namespace toadlet{
namespace flick{

TOADLET_C_API VideoDevice *new_MFVideoDevice(){
	return new MFVideoDevice();
}

#if defined(TOADLET_BUILD_DYNAMIC)
	TOADLET_C_API VideoDevice *new_VideoDevice(){
		return new MFVideoDevice();
	}
#endif

MFVideoDevice::MFVideoDevice():
	mListener(NULL),
	mReader(NULL),
	mType(NULL),
	//mFormat,
	mRunning(false),
	mRef(0)
{}

MFVideoDevice::~MFVideoDevice(){
	destroy();
}

bool MFVideoDevice::create(){
	if(mReader!=NULL){
		return false;
	}

	bool result=false;
	IMFActivate *device=findDevice();
	if(device!=NULL){
		result=create(device);
	}
	return result;
}

bool MFVideoDevice::create(IMFActivate *device){
    IMFMediaSource *source=NULL;
    IMFAttributes *attributes=NULL;

	CoInitialize(NULL);
	MFStartup(0);

	HRESULT hr=device->ActivateObject(__uuidof(IMFMediaSource),(void**)&source);
	if(SUCCEEDED(hr)){
		hr=MFCreateAttributes(&attributes,2);
	}
	if(SUCCEEDED(hr)){
		hr=attributes->SetUINT32(MF_READWRITE_DISABLE_CONVERTERS,TRUE);
	}
	if(SUCCEEDED(hr)){
		hr=attributes->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK,this);
	}
	if(SUCCEEDED(hr)){
		hr=MFCreateSourceReaderFromMediaSource(source,attributes,&mReader);
	}
	if(SUCCEEDED(hr)){
		int i;
		for(i=0;mFormat==NULL;i++){
			hr=mReader->GetNativeMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,i,&mType);
			if(FAILED(hr)) break;
			mFormat=getTextureFormat(mType);
			if(mFormat!=NULL) hr=S_OK;
		}
	}

	if(source!=NULL){
		if(FAILED(hr)){
			source->Shutdown();
		}
		source->Release();
	}
	if(attributes!=NULL){
		attributes->Release();
	}

	if(FAILED(hr)){
		destroy();
	}
	
    return SUCCEEDED(hr);
}

void MFVideoDevice::destroy(){
	if(mReader!=NULL){
		mReader->Release();
		mReader=NULL;
	}
	if(mType!=NULL){
		mType->Release();
		mType=NULL;
	}
}

bool MFVideoDevice::start(){
	HRESULT hr=E_FAIL;
	if(mRunning==false && mReader!=NULL){
		hr=mReader->ReadSample((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,0,NULL,NULL,NULL,NULL);
	}
	return SUCCEEDED(hr);
}

void MFVideoDevice::stop(){
	mRunning=false;
}

HRESULT MFVideoDevice::OnReadSample(HRESULT hrStatus,DWORD dwStreamIndex,DWORD dwStreamFlags,LONGLONG llTimestamp,IMFSample *pSample){
	HRESULT hr=S_OK;
	IMFMediaBuffer *buffer=NULL;

	if(FAILED(hrStatus)){
		hr=hrStatus;
	}

	if(SUCCEEDED(hr)){
		if(pSample){
			hr=pSample->GetBufferByIndex(0,&buffer);
			if(SUCCEEDED(hr)){
				tbyte *data=NULL;
	            hr=buffer->Lock(&data,NULL,NULL);
				if(SUCCEEDED(hr)){
					if(mFormat->getXPitch()<0){
						data=data+abs(mFormat->getXPitch())*(mFormat->getHeight()-1);
					}
				}

				if(mListener!=NULL){
					mListener->frameReceived(mFormat,data);
				}

				hr=buffer->Unlock();
			}
		}
	}

	if(SUCCEEDED(hr)){
		hr=mReader->ReadSample((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,0,NULL,NULL,NULL,NULL);
	}

	return hr;
}

ULONG MFVideoDevice::AddRef(){
	mRef++;
	return mRef;
}

ULONG MFVideoDevice::Release(){
	int count=mRef--;
	if(count<=0){
		delete this;
	}
	return count;
}

HRESULT MFVideoDevice::QueryInterface(REFIID riid, void** ppv){
	if(riid==__uuidof(IMFSourceReaderCallback)){
		AddRef();
		*ppv=(IUnknown*)this;
		return S_OK;
	}
	return E_FAIL;
}

IMFActivate *MFVideoDevice::findDevice(){
	IMFAttributes *attributes=NULL;
	IMFActivate **devices=NULL;
	UINT32 count=0;
	IMFActivate *device=NULL;

	HRESULT hr=S_OK;
	if(SUCCEEDED(hr)){
		hr=MFCreateAttributes(&attributes,1);
	}
	if(SUCCEEDED(hr)){
		hr=attributes->SetGUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID);
	}
	if(SUCCEEDED(hr)){
		hr=MFEnumDeviceSources(attributes,&devices,&count);
	}
	if(SUCCEEDED(hr)){
		if(count>0){
			device=devices[0];
			device->AddRef();
		}

		int i;
		for(i=0;i<count;++i){
			devices[i]->Release();
		}
		CoTaskMemFree(devices);
		devices=NULL;
	}

	return device;
}

TextureFormat::ptr MFVideoDevice::getTextureFormat(IMFMediaType *type){
	TextureFormat::ptr format;
	GUID subtype=GUID_NULL;
	int pixelFormat=0;
	UINT32 width=0;
	UINT32 height=0;
	LONG stride=0;

	// Get the subtype and the image size.
	HRESULT hr=type->GetGUID(MF_MT_SUBTYPE,&subtype);
	if(SUCCEEDED(hr)){
		if(subtype==MFVideoFormat_RGB32){
			pixelFormat=TextureFormat::Format_RGBA_8;
		}
		else if(subtype==MFVideoFormat_RGB24){
			pixelFormat=TextureFormat::Format_RGB_8;
		}
		else if(subtype==MFVideoFormat_YUY2){
			pixelFormat=TextureFormat::Format_YUY2;
		}
//		else if(subtype==MFVideoFormat_NV12){
//			pixelFormat=TextureFormat::Format_RGB_8;
//		}
		else{
			hr=E_FAIL;
		}
	}
	if(SUCCEEDED(hr)){
		hr=MFGetAttributeSize(type,MF_MT_FRAME_SIZE,&width,&height);
	}
	if(SUCCEEDED(hr)){
		hr=MFGetStrideForBitmapInfoHeader(subtype.Data1,width,&stride);
	}
	if(SUCCEEDED(hr)){
		format=TextureFormat::ptr(new TextureFormat(TextureFormat::Dimension_D2,pixelFormat,width,height,1,1));
		format->setPitches(stride,stride*height,stride*height);
	}

	return format;
}

}
}
