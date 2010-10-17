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

#if !defined(TOADLET_PLATFORM_WINCE)
	#pragma comment(lib,"winmm.lib")
#endif

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace ribbit{

extern AudioStream *new_OggVorbisDecoder();
extern AudioStream *new_WaveDecoder();

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
	mChannels=1;
	mBitsPerSample=16;
	mSamplesPerSecond=11025;//44100;
	mBufferFadeTime=100;
	mNumBuffers=4;
	mBufferSize=2048;

	mCapabilitySet.maxSources=16;
	mCapabilitySet.mimeTypes.add("audio/x-wav");
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
	memset(mBufferData,0,mNumBuffers*mBufferSize);

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
	return NULL;
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
			"Win32Player: Null Stream");
		return NULL;
	}

	AudioStream::ptr decoder;
	#if defined(TOADLET_HAS_OGG_VORBIS)
		if(mimeType=="application/ogg"){
			decoder=AudioStream::ptr(new_OggVorbisDecoder());
		}
	#endif
	if(mimeType=="audio/x-wav"){
		decoder=AudioStream::ptr(new_WaveDecoder());
	}

	if(decoder==NULL){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"Win32Player: MIME type not supported: " + mimeType);
		return NULL;
	}

	if(decoder->startStream(stream)==false){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"Win32Player: Error starting decoder stream");
		return NULL;
	}

	return decoder;
}

void Win32Player::decodeStream(AudioStream *decoder,tbyte *&finalBuffer,int &finalLength){
	Collection<tbyte*> buffers;
	int amount=0,total=0;
	int i=0;

	while(true){
		tbyte *buffer=new tbyte[mBufferSize];
		amount=decoder->read(buffer,mBufferSize);
		if(amount==0){
			delete[] buffer;
			break;
		}
		else{
			total+=amount;
			buffers.add(buffer);
		}
	}

	finalBuffer=new tbyte[total];
	finalLength=total;

	for(i=0;i<buffers.size();++i){
		int thing=mBufferSize;
		if(total<thing){
			thing=total;
		}
		memcpy(finalBuffer+i*mBufferSize,buffers[i],thing);
		total-=mBufferSize;
		delete[] buffers[i];
	}

	#if !defined(TOADLET_NATIVE_FORMAT)
		int bps=decoder->getBitsPerSample();
		if(bps==16){
			int i=0;
			while(i<finalLength){
				littleInt16InPlace((int16&)finalBuffer[i]);
				i+=2;
			}
		}
	#endif
}

void Win32Player::internal_audioCreate(Win32Audio *audio){
	mAudios.add(audio);
}

void Win32Player::internal_audioDestroy(Win32Audio *audio){
	mAudios.remove(audio);
}

// Mix all the currently playing audios
int Win32Player::read(tbyte *data,int length){
	bool playing=false;
	int i,j;
	for(i=0;i<mAudios.size();++i){
		playing|=mAudios[i]->mPlaying;
	}

	if(!playing){
		memset(data,0,length);
		return length;
	}

	const int bufferSize=1024;
	int mixBuffer[bufferSize];
	byte singleBuffer[bufferSize];

	int bps=mBitsPerSample;

	int amount=0;
	for(amount=0;amount<length;){
		memset(mixBuffer,0,sizeof(mixBuffer));

		int newamt=Math::minVal(length-amount,bufferSize);

		for(i=0;i<mAudios.size();++i){
			if(mAudios[i]->getPlaying()==false) continue;

			mAudios[i]->read(singleBuffer,newamt);
			if(bps==8){
				for(j=0;j<newamt;++j){
					mixBuffer[j]+=((uint8*)singleBuffer)[j];
				}
			}
			else if(bps==16){
				for(j=0;j<newamt/2;++j){
					mixBuffer[j]+=((uint16*)singleBuffer)[j];
				}
			}
		}

		if(bps==8){
			for(i=0;i<newamt;++i){
				int v=mixBuffer[i];
				if(v<Extents::MIN_INT8){
					v=Extents::MIN_INT8;
				}
				else if(v>Extents::MAX_INT8){
					v=Extents::MAX_INT8;
				}
				((uint8*)(data+amount))[i]=v;
			}
		}
		else if(bps==16){
			for(i=0;i<newamt/2;++i){
				int v=mixBuffer[i];
				if(v<Extents::MIN_INT16){
					v=Extents::MIN_INT16;
				}
				else if(v>Extents::MAX_INT16){
					v=Extents::MAX_INT16;
				}
				((uint16*)(data+amount))[i]=v;
			}
		}

		amount+=newamt;
	}

	return length;
}

}
}
