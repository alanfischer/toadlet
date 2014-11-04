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

#include "MMAudioDevice.h"
#include "MMAudio.h"
#include "MMAudioBuffer.h"
#include <toadlet/egg/EndianConversion.h>
#include <toadlet/egg/Extents.h>
#include <toadlet/ribbit/AudioFormatConversion.h>

#if !defined(TOADLET_PLATFORM_WINCE)
	#pragma comment(lib,"winmm.lib")
#endif

namespace toadlet{
namespace ribbit{

TOADLET_C_API AudioDevice* new_MMAudioDevice(){
	return new MMAudioDevice();
}

#if defined(TOADLET_BUILD_DYNAMIC)
	TOADLET_C_API AudioDevice* new_AudioDevice(){
		return new MMAudioDevice();
	}
#endif

MMAudioDevice::MMAudioDevice():
	mDevice(NULL),
	mBuffers(NULL),
	mBufferData(NULL),
	mBufferFadeTime(0),
	mNumBuffers(0),
	mBufferSize(0)
{
	mBufferFadeTime=100;
}

bool MMAudioDevice::create(int options){
	Log::alert(Categories::TOADLET_RIBBIT,
		"creating MMAudioDevice");

	mFormat=new AudioFormat(16,2,11025);
	mBufferSize=1024;
	mNumBuffers=4;

	mCaps.maxSources=16;

	WAVEFORMATEX format={0};
	format.cbSize=sizeof(WAVEFORMATEX);
	format.wFormatTag=WAVE_FORMAT_PCM;
	format.nChannels=mFormat->getChannels();
	format.nSamplesPerSec=mFormat->getSamplesPerSecond();
	format.wBitsPerSample=mFormat->getBitsPerSample();
	format.nBlockAlign=mFormat->getFrameSize();
	format.nAvgBytesPerSec=format.nSamplesPerSec*format.nBlockAlign;

	MMRESULT result=waveOutOpen(&mDevice,WAVE_MAPPER,&format,0,0,0);
	if(result!=MMSYSERR_NOERROR){
		return false;
	}

	mBuffers=new WAVEHDR[mNumBuffers];
	memset(mBuffers,0,sizeof(WAVEHDR)*mNumBuffers);

	mBufferData=new tbyte[mNumBuffers*mBufferSize];
	if(mFormat->getBitsPerSample()==8){
		memset(mBufferData,128,mNumBuffers*mBufferSize);
	}
	else{
		memset(mBufferData,0,mNumBuffers*mBufferSize);
	}

	int i;
	for(i=0;i<mNumBuffers;++i){
		WAVEHDR *header=&mBuffers[i];
		header->lpData=(LPSTR)(mBufferData+mBufferSize*i);
		header->dwBufferLength=mBufferSize;

		result=waveOutPrepareHeader(mDevice,header,sizeof(WAVEHDR));
		if(result!=MMSYSERR_NOERROR){
			Error::unknown(Categories::TOADLET_RIBBIT,"waveOutPrepareHeader error");
		}

		result=waveOutWrite(mDevice,header,sizeof(WAVEHDR));
		if(result!=MMSYSERR_NOERROR){
			Error::unknown(Categories::TOADLET_RIBBIT,"waveOutWrite error");
		}
	}

	return result==MMSYSERR_NOERROR;
}

void MMAudioDevice::destroy(){
	Log::alert(Categories::TOADLET_RIBBIT,
		"destroying MMAudioDevice");

	if(mDevice!=NULL){
	    waveOutReset(mDevice);

		int i;
	    for(i=0;i<mNumBuffers;++i){
			WAVEHDR *header=&mBuffers[i];
			if((header->dwFlags&WHDR_PREPARED)!=0 || (header->dwFlags&WHDR_DONE)!=0){
				waveOutUnprepareHeader(mDevice,header,sizeof(WAVEHDR));
			}
		}

		waveOutClose(mDevice);
		mDevice=NULL;
	}

	int i;
	for(i=0;i<mAudios.size();++i){
		mAudios[i]->destroy();
	}

	if(mBuffers!=NULL){
		delete[] mBuffers;
		mBuffers=NULL;
	}

	if(mBufferData!=NULL){
		delete[] mBufferData;
		mBufferData=NULL;
	}
}

AudioBuffer *MMAudioDevice::createAudioBuffer(){
	return new MMAudioBuffer(this);
}

Audio *MMAudioDevice::createAudio(){
	return new MMAudio(this);
}

void MMAudioDevice::update(int dt){
	int i;
	for(i=0;i<mNumBuffers;++i){
		WAVEHDR *header=&mBuffers[i];
		if((header->dwFlags&WHDR_DONE)!=0){
			MMRESULT result=waveOutUnprepareHeader(mDevice,header,sizeof(WAVEHDR));
			if(result!=MMSYSERR_NOERROR){
				Error::unknown(Categories::TOADLET_RIBBIT,"waveOutUnprepareHeader error");
			}

			read((tbyte*)header->lpData,mBufferSize);
			header->dwFlags=0;

			result=waveOutPrepareHeader(mDevice,header,sizeof(WAVEHDR));
			if(result!=MMSYSERR_NOERROR){
				Error::unknown(Categories::TOADLET_RIBBIT,"waveOutPrepareHeader error");
			}

			result=waveOutWrite(mDevice,header,sizeof(WAVEHDR));
			if(result!=MMSYSERR_NOERROR){
				Error::unknown(Categories::TOADLET_RIBBIT,"waveOutWrite error");
			}
		}
	}
}

void MMAudioDevice::internal_audioCreate(MMAudio *audio){
	mAudios.push_back(audio);
}

void MMAudioDevice::internal_audioDestroy(MMAudio *audio){
	mAudios.erase(stlit::remove(mAudios.begin(),mAudios.end(),audio),mAudios.end());
}

// Mix all the currently playing audios
int MMAudioDevice::read(tbyte *data,int length){
	int bps=mFormat->getBitsPerSample();

	bool playing=false;
	int i,j;
	for(i=0;i<mAudios.size();++i){
		playing|=mAudios[i]->mPlaying;
	}

	if(!playing){
		if(bps==8){
			memset(data,128,length);
		}
		else{
			memset(data,0,length);
		}
		return length;
	}

	const int bufferSize=1024;
	int mixBuffer[bufferSize];
	tbyte singleBuffer[bufferSize];

	int amount=0,newamt=0;
	for(amount=0;amount<length;amount+=newamt){
		memset(mixBuffer,0,sizeof(mixBuffer));

		newamt=Math::intMinVal(length-amount,bufferSize);

		for(i=0;i<mAudios.size();++i){
			if(mAudios[i]->getPlaying()==false) continue;

			int readamt=mAudios[i]->read(singleBuffer,newamt);
			if(bps==8){
				for(j=0;j<readamt;++j){
					mixBuffer[j]+=(int)(((uint8*)singleBuffer)[j])-128;
				}
			}
			else if(bps==16){
				for(j=0;j<readamt/2;++j){
					mixBuffer[j]+=(int)((int16*)singleBuffer)[j];
				}
			}
		}

		if(bps==8){
			for(i=0;i<newamt;++i){
				int v=mixBuffer[i];
				((uint8*)(data+amount))[i]=Math::intClamp(0,Extents::MAX_UINT8,v+128);
			}
		}
		else if(bps==16){
			for(i=0;i<newamt/2;++i){
				int v=mixBuffer[i];
				((int16*)(data+amount))[i]=Math::intClamp(Extents::MIN_INT16,Extents::MAX_INT16,v);
			}
		}
	}

	return length;
}

}
}
