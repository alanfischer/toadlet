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

#include "Win32Player.h"
#include "Win32Audio.h"
#include "Win32AudioBuffer.h"
#include <toadlet/egg/EndianConversion.h>
#include <toadlet/egg/Extents.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/ribbit/AudioFormatConversion.h>

#include "../decoders/wavedecoder/WaveDecoder.h"
#if defined(TOADLET_HAS_OGGVORBIS)
	#include "../decoders/oggvorbisdecoder/OggVorbisDecoder.h"
#endif

#if !defined(TOADLET_PLATFORM_WINCE)
	#pragma comment(lib,"winmm.lib")
#endif

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace ribbit{

TOADLET_C_API AudioPlayer* new_Win32Player(){
	return new Win32Player();
}

#if defined(TOADLET_BUILD_DYNAMIC)
	TOADLET_C_API AudioPlayer* new_AudioPlayer(){
		return new Win32Player();
	}
#endif

Win32Player::Win32Player():
	mChannels(0),
	mBitsPerSample(0),
	mSamplesPerSecond(0),
	mDevice(NULL),
	mBuffers(NULL),
	mBufferData(NULL),
	mBufferFadeTime(0),
	mNumBuffers(0),
	mBufferSize(0)
{
}

Win32Player::~Win32Player(){
	destroy();
}

bool Win32Player::create(int *options){
	Logger::alert(Categories::TOADLET_RIBBIT,
		"creating Win32Player");

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

	mChannels=2;
	mBitsPerSample=16;
	mSamplesPerSecond=44100;
	mBufferSize=AudioFormatConversion::findConvertedLength(8192,mChannels,mBitsPerSample,mSamplesPerSecond,2,16,44100);
	mNumBuffers=4;
	mBufferFadeTime=100;

	mCapabilitySet.maxSources=16;
	mCapabilitySet.mimeTypes.add(WaveDecoder::mimeType());
	#if defined(TOADLET_HAS_OGGVORBIS)
		mCapabilitySet.mimeTypes.add(OggVorbisDecoder::mimeType());
	#endif

	WAVEFORMATEX format={0};
	format.cbSize=sizeof(WAVEFORMATEX);
	format.wFormatTag=WAVE_FORMAT_PCM;
	format.nChannels=mChannels;
	format.nSamplesPerSec=mSamplesPerSecond;
	format.wBitsPerSample=mBitsPerSample;
	format.nBlockAlign=format.nChannels*format.wBitsPerSample/8;
	format.nAvgBytesPerSec=format.nSamplesPerSec*format.nBlockAlign;

	MMRESULT result=waveOutOpen(&mDevice,WAVE_MAPPER,&format,0,0,0);
	if(result!=MMSYSERR_NOERROR){
		return false;
	}

	mBuffers=new WAVEHDR[mNumBuffers];
	memset(mBuffers,0,sizeof(WAVEHDR)*mNumBuffers);

	mBufferData=new tbyte[mNumBuffers*mBufferSize];
	if(mBitsPerSample==8){
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

bool Win32Player::destroy(){
	Logger::alert(Categories::TOADLET_RIBBIT,
		"destroying Win32Player");

	if(mDevice!=NULL){
	    waveOutReset(mDevice);

		int i;
	    for(i=0;i<mNumBuffers;++i){
			WAVEHDR *header=&mBuffers[i];
			if((header->dwFlags&WHDR_PREPARED)>0 || (header->dwFlags&WHDR_DONE)>0){
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

	return true;
}

AudioBuffer *Win32Player::createAudioBuffer(){
	return new Win32AudioBuffer(this);
}

Audio *Win32Player::createBufferedAudio(){
	return new Win32Audio(this);
}

Audio *Win32Player::createStreamingAudio(){
	return new Win32Audio(this);
}

void Win32Player::update(int dt){
	int i;
	for(i=0;i<mNumBuffers;++i){
		WAVEHDR *header=&mBuffers[i];
		if((header->dwFlags&WHDR_DONE)>0){
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

AudioStream::ptr Win32Player::startAudioStream(Stream::ptr stream,const String &mimeType){
	if(stream==NULL){
		Error::nullPointer(Categories::TOADLET_RIBBIT,
			"null Stream");
		return NULL;
	}

	AudioStream::ptr decoder;
	if(mimeType==WaveDecoder::mimeType()){
		decoder=AudioStream::ptr(new WaveDecoder());
	}
	#if defined(TOADLET_HAS_OGGVORBIS)
		if(mimeType==OggVorbisDecoder::mimeType()){
			decoder=AudioStream::ptr(new OggVorbisDecoder());
		}
	#endif

	if(decoder==NULL){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"MIME type not supported: " + mimeType);
		return NULL;
	}

	if(decoder->startStream(stream)==false){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"error starting decoder stream");
		return NULL;
	}

	return decoder;
}

void Win32Player::internal_audioCreate(Win32Audio *audio){
	mAudios.add(audio);
}

void Win32Player::internal_audioDestroy(Win32Audio *audio){
	mAudios.remove(audio);
}

// Mix all the currently playing audios
int Win32Player::read(tbyte *data,int length){
	int bps=mBitsPerSample;

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
