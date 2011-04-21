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

#include "MMPlayer.h"
#include "MMAudio.h"
#include "MMAudioBuffer.h"
#include <toadlet/egg/EndianConversion.h>
#include <toadlet/egg/Extents.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/ribbit/AudioFormatConversion.h>

#if !defined(TOADLET_PLATFORM_WINCE)
	#pragma comment(lib,"winmm.lib")
#endif

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace ribbit{

TOADLET_C_API AudioPlayer* new_MMPlayer(){
	return new MMPlayer();
}

#if defined(TOADLET_BUILD_DYNAMIC)
	TOADLET_C_API AudioPlayer* new_AudioPlayer(){
		return new MMPlayer();
	}
#endif

MMPlayer::MMPlayer():
	mDevice(NULL),
	mBuffers(NULL),
	mBufferData(NULL),
	mBufferFadeTime(0),
	mNumBuffers(0),
	mBufferSize(0)
{
	mFormat=AudioFormat::ptr(new AudioFormat());
}

MMPlayer::~MMPlayer(){
	destroy();
}

bool MMPlayer::create(int *options){
	Logger::alert(Categories::TOADLET_RIBBIT,
		"creating MMPlayer");

	if(options!=NULL){
		int i=0;
		while(options[i]!=0){
			switch(options[i++]){
				case Option_BUFFER_FADE_TIME:
					mBufferFadeTime=options[i++];
					Logger::alert(Categories::TOADLET_PEEPER,
						String("Setting BufferFadeTime to:")+mBufferFadeTime);
					break;
			}
		}
	}

	mFormat->channels=2;
	mFormat->bitsPerSample=16;
	mFormat->samplesPerSecond=44100/2;
	AudioFormat standardFormat(2,16,44100);
	mBufferSize=AudioFormatConversion::findConvertedLength(8192,mFormat,&standardFormat);
	mNumBuffers=4;
	mBufferFadeTime=100;

	mCapabilitySet.maxSources=16;

	WAVEFORMATEX format={0};
	format.cbSize=sizeof(WAVEFORMATEX);
	format.wFormatTag=WAVE_FORMAT_PCM;
	format.nChannels=mFormat->channels;
	format.nSamplesPerSec=mFormat->samplesPerSecond;
	format.wBitsPerSample=mFormat->bitsPerSample;
	format.nBlockAlign=format.nChannels*format.wBitsPerSample/8;
	format.nAvgBytesPerSec=format.nSamplesPerSec*format.nBlockAlign;

	MMRESULT result=waveOutOpen(&mDevice,WAVE_MAPPER,&format,0,0,0);
	if(result!=MMSYSERR_NOERROR){
		return false;
	}

	mBuffers=new WAVEHDR[mNumBuffers];
	memset(mBuffers,0,sizeof(WAVEHDR)*mNumBuffers);

	mBufferData=new tbyte[mNumBuffers*mBufferSize];
	if(mFormat->bitsPerSample==8){
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
			Logger::warning(Categories::TOADLET_RIBBIT,"waveOutPrepareHeader error");
		}

		result=waveOutWrite(mDevice,header,sizeof(WAVEHDR));
		if(result!=MMSYSERR_NOERROR){
			Logger::warning(Categories::TOADLET_RIBBIT,"waveOutWrite error");
		}
	}

	return result==MMSYSERR_NOERROR;
}

void MMPlayer::destroy(){
	Logger::alert(Categories::TOADLET_RIBBIT,
		"destroying MMPlayer");

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

AudioBuffer *MMPlayer::createAudioBuffer(){
	return new MMAudioBuffer(this);
}

Audio *MMPlayer::createBufferedAudio(){
	return new MMAudio(this);
}

Audio *MMPlayer::createStreamingAudio(){
	return new MMAudio(this);
}

void MMPlayer::update(int dt){
	int i;
	for(i=0;i<mNumBuffers;++i){
		WAVEHDR *header=&mBuffers[i];
		if((header->dwFlags&WHDR_DONE)!=0){
			MMRESULT result=waveOutUnprepareHeader(mDevice,header,sizeof(WAVEHDR));
			if(result!=MMSYSERR_NOERROR){
				Logger::warning(Categories::TOADLET_RIBBIT,"waveOutUnprepareHeader error");
			}

			read((tbyte*)header->lpData,mBufferSize);
			header->dwFlags=0;

			result=waveOutPrepareHeader(mDevice,header,sizeof(WAVEHDR));
			if(result!=MMSYSERR_NOERROR){
				Logger::warning(Categories::TOADLET_RIBBIT,"waveOutPrepareHeader error");
			}

			result=waveOutWrite(mDevice,header,sizeof(WAVEHDR));
			if(result!=MMSYSERR_NOERROR){
				Logger::warning(Categories::TOADLET_RIBBIT,"waveOutWrite error");
			}
		}
	}
}

void MMPlayer::internal_audioCreate(MMAudio *audio){
	mAudios.add(audio);
}

void MMPlayer::internal_audioDestroy(MMAudio *audio){
	mAudios.remove(audio);
}

// Mix all the currently playing audios
int MMPlayer::read(tbyte *data,int length){
	int bps=mFormat->bitsPerSample;

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
