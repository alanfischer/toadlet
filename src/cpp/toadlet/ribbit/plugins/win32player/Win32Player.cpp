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
#include <toadlet/egg/System.h>
#include <toadlet/egg/Error.h>

#if !defined(TOADLET_PLATFORM_WINCE)
	#pragma comment(lib,"winmm.lib")
#endif

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace ribbit{

extern AudioStream *new_OggVorbisDecoder();
extern AudioStream *new_WaveDecoder();

const int DECODE_BUFFER_SIZE=4096;

TOADLET_C_API AudioPlayer* new_Win32Player(){
	return new Win32Player();
}

#if defined(TOADLET_BUILD_DYNAMIC)
	TOADLET_C_API AudioPlayer* new_AudioPlayer(){
		return new Win32Player();
	}
#endif

Win32Player::Win32Player():
	mWaveOut(NULL),
	mNextSoundTime(0)
{
	mCapabilitySet.maxSources=waveOutGetNumDevs();
	mCapabilitySet.mimeTypes.add("audio/x-wav");
}

Win32Player::~Win32Player(){
	destroy();
}

bool Win32Player::create(int *options){
	return true;
}

bool Win32Player::destroy(){
	if(mWaveOut!=NULL){
		waveOutClose(mWaveOut);
		mWaveOut=NULL;
	}

	int i;
	for(i=0;i<mAudios.size();++i){
		mAudios[i]->destroy();
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

bool Win32Player::canPlaySound(){
	return mNextSoundTime+400<System::mtime();
}

void Win32Player::playedSound(int time){
	mNextSoundTime=System::mtime()+time;
}

AudioStream::ptr Win32Player::startAudioStream(io::InputStream::ptr in,const String &mimeType){
	if(in==NULL){
		Error::nullPointer(Categories::TOADLET_RIBBIT,
			"Win32Player: Null InputStream");
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

	if(decoder->startStream(in)==false){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"Win32Player: Error starting decoder stream");
		return NULL;
	}

	return decoder;
}

void Win32Player::decodeStream(AudioStream *decoder,char *&finalBuffer,int &finalLength){
	Collection<char*> buffers;
	int amount=0,total=0;
	int i=0;

	while(true){
		char *buffer=new char[DECODE_BUFFER_SIZE];
		amount=decoder->read(buffer,DECODE_BUFFER_SIZE);
		if(amount==0){
			delete[] buffer;
			break;
		}
		else{
			total+=amount;
			buffers.add(buffer);
		}
	}

	finalBuffer=new char[total];
	finalLength=total;

	for(i=0;i<buffers.size();++i){
		int thing=DECODE_BUFFER_SIZE;
		if(total<thing){
			thing=total;
		}
		memcpy(finalBuffer+i*DECODE_BUFFER_SIZE,buffers[i],thing);
		total-=DECODE_BUFFER_SIZE;
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

}
}
