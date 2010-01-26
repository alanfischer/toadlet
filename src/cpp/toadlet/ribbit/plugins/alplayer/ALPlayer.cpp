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

#include "ALPlayer.h"
#include "ALAudio.h"
#include "ALAudioBuffer.h"
#include <toadlet/egg/System.h>
#include <toadlet/egg/Extents.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/EndianConversion.h>
#include <stdlib.h>
#include <string.h> // memcpy

#if defined(TOADLET_PLATFORM_OSX)
	#include "../decoders/coreaudiodecoder/CoreAudioDecoder.h"
	#include "platform/osx/CoreAudio.h"
#endif

#if defined(TOADLET_PLATFORM_WIN32)
	#if defined(TOADLET_LIBOPENAL_NAME)
		#pragma comment(lib,TOADLET_LIBOPENAL_NAME)
	#endif
#endif

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace ribbit{

#if defined(TOADLET_PLATFORM_OSX)
	using toadlet::egg::Collection;
#endif

extern AudioStream *new_CoreAudioDecoder();
extern AudioStream *new_OggVorbisDecoder();
extern AudioStream *new_WaveDecoder();

const int DECODE_BUFFER_SIZE=4096;

TOADLET_C_API AudioPlayer* new_ALPlayer(){
	return new ALPlayer();
}

#if defined(TOADLET_BUILD_DYNAMIC)
	TOADLET_C_API AudioPlayer* new_AudioPlayer(){
		return new ALPlayer();
	}
#endif

proc_alBufferDataStatic toadlet_alBufferDataStatic=NULL;

ALPlayer::ALPlayer():
	mDevice(NULL),
	mContext(NULL),
	//mAudios,
	//mSourcePool,
	//mAllSources,
	mDefaultRolloffFactor(0),
	mBufferFadeInTime(0),

	mStopThread(false)
	//mThread,
	//mMutex,

	//mCapabilitySet
{}

ALPlayer::~ALPlayer(){
	destroy();
}

bool ALPlayer::create(int *options){
	Logger::alert(Categories::TOADLET_RIBBIT,
		"creating ALPlayer");

	if(options!=NULL){
		int i=0;
		while(options[i]!=0){
			switch(options[i++]){
				case Option_FADE_IN_BUFFER_TIME:
					mBufferFadeInTime=options[i++];
					Logger::alert(Categories::TOADLET_PEEPER,
						String("Setting BufferFadeInTime to:")+mBufferFadeInTime);
					break;
			}
		}
	}

	#if defined(TOADLET_HAS_ALEM)
		if(alem_getInitialized()==false){
			int alemInitResult=alem_init(ALEM_ACCELERATED_IF_AVAILABLE);
			if(alemInitResult<=0){
				Error::unknown(Categories::TOADLET_PEEPER,
					"alem_init failed");
				return false;
			}
		}

		int alemAcceleratedResult=alem_getAccelerated();
		Logger::alert(Categories::TOADLET_RIBBIT,
			String("alem_getAccelerated result:")+alemAcceleratedResult);
	#endif

	// Initialize Open AL manually
	mDevice=alcOpenDevice(NULL);
	TOADLET_CHECK_ALERROR("alcOpenDevice");

	if(mDevice==NULL){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"invalid device");
		return false;
	}

	toadlet_alBufferDataStatic=(proc_alBufferDataStatic)alcGetProcAddress(mDevice,(ALCchar*)"alBufferDataStatic");
	if(toadlet_alBufferDataStatic!=NULL){
		Logger::alert(Categories::TOADLET_RIBBIT,
			"using alBufferDataStatic extension");
	}

	mContext=alcCreateContext(mDevice,NULL);
	TOADLET_CHECK_ALERROR("alcCreateContext");

	if(mContext==NULL){
		alcCloseDevice(mDevice);
		mDevice=NULL;
		Error::unknown(Categories::TOADLET_RIBBIT,
			"invalid context");
		return false;
	}

	alcMakeContextCurrent(mContext);
	TOADLET_CHECK_ALERROR("alcMakeContextCurrent");

	// TODO: Change this to a loop with alGenSources till it returns an error
	mCapabilitySet.maxSources=8;
	mCapabilitySet.streaming=true;
	mCapabilitySet.positional=true;
	mCapabilitySet.mimeTypes.add("audio/x-wav");
	mCapabilitySet.mimeTypes.add("application/ogg");

	alListenerf(AL_GAIN,1.0);
	TOADLET_CHECK_ALERROR("alListenerf");

	alDopplerFactor(1.0);
	TOADLET_CHECK_ALERROR("alDopplerFactor");
	alDopplerVelocity(340.0); // TODO: Move the doppler parameters to the interface
	TOADLET_CHECK_ALERROR("alDopplerVelocity");

	// We use a small epsilon here, since I believe an AL_POSITION of 0,0,0 on the listener results in no sound volume scaling for distances
	alListener3f(AL_POSITION,0.001,0.001,0.001);
	TOADLET_CHECK_ALERROR("alListener3f");

	mDefaultRolloffFactor=Math::ONE;

	mAllSources.resize(mCapabilitySet.maxSources);
	alGenSources(mAllSources.size(),&mAllSources[0]);
	TOADLET_CHECK_ALERROR("alGenSources");
	mSourcePool=mAllSources;

	int i;
	for(i=0;i<mAllSources.size();++i){
		alSourcei(mAllSources[i],AL_LOOPING,false);
		alSourcef(mAllSources[i],AL_PITCH,1.0);
		alSourcef(mAllSources[i],AL_GAIN,1.0);
		alSourcef(mAllSources[i],AL_ROLLOFF_FACTOR,1.0);
		ALfloat zero[]={0,0,0};
		alSourcefv(mAllSources[i],AL_POSITION,zero);
	}
	TOADLET_CHECK_ALERROR("Initialize sources");

	Logger::debug(Categories::TOADLET_RIBBIT,
		"starting thread");

	mStopThread=false;
	mThread=Thread::ptr(new Thread(this));
	mThread->start();

	Logger::alert(Categories::TOADLET_RIBBIT,
		"created ALPlayer");

	return true;
}

bool ALPlayer::destroy(){
	Logger::alert(Categories::TOADLET_RIBBIT,
		"shutingdown ALPlayer");

	mStopThread=true;
	if(mThread!=NULL){
		mThread->join();
		mThread=NULL;
	}

	int i;
	for(i=0;i<mAudios.size();++i){
		mAudios[i]->mAudioPlayer=NULL;
	}
	#if defined(TOADLET_PLATFORM_OSX)
		for(i=0;i<mCoreAudios.size();++i){
			mCoreAudios[i]->mAudioPlayer=NULL;
		}
	#endif

	if(mAllSources.size()>0){
		alDeleteSources(mAllSources.size(),&mAllSources[0]);
		mAllSources.clear();
	}

	if(mContext!=NULL){
		alcDestroyContext(mContext);
		mContext=NULL;
	}

	if(mDevice!=NULL){
		alcCloseDevice(mDevice);
		mDevice=NULL;
	}

	Logger::alert(Categories::TOADLET_RIBBIT,
		"shutdown ALPlayer");

	return true;
}

AudioBuffer *ALPlayer::createAudioBuffer(){
	return new ALAudioBuffer(this);
}

Audio *ALPlayer::createBufferedAudio(){
	return new ALAudio(this);
}

Audio *ALPlayer::createStreamingAudio(){
	#if defined(TOADLET_PLATFORM_OSX)
		return new CoreAudio(this);
	#else
		return new ALAudio(this);
	#endif
}

void ALPlayer::suspend(){
	alcSuspendContext(mContext);
}

void ALPlayer::resume(){
	alcProcessContext(mContext);
}

void ALPlayer::update(int dt){
	// This has been moved to a thread
}

void ALPlayer::run(){
	uint64 startTime=System::mtime();
	uint64 dt=33; // Update audio at about 60 times a second
	int i=0;
	while(mStopThread==false){
		uint64 curTime=System::mtime();
		uint64 diff=curTime-startTime;
		if(diff>=dt){
			lock();
				for(i=0;i<mAudios.size();++i){
					mAudios[i]->update(dt);
				}
				#if defined(TOADLET_PLATFORM_OSX)
					for(i=0;i<mCoreAudios.size();++i){
						mCoreAudios[i]->update(dt);
					}
				#endif
			unlock();
			// I realize that this won't correctly keep count of the 'extra' chunks of time
			// in an update, but doing that is unnecessary for audio at least
			startTime=curTime;
		}
		System::msleep(10);
	}
}

void ALPlayer::decodeStream(AudioStream *decoder,byte *&finalBuffer,int &finalLength){
	Collection<byte*> buffers;
	int amount=0,total=0;
	int i=0;

	while(true){
		byte *buffer=new byte[DECODE_BUFFER_SIZE];
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

	finalBuffer=new byte[total];
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

AudioStream::ptr ALPlayer::startAudioStream(io::Stream::ptr stream,const String &mimeType){
	if(stream==NULL){
		Error::nullPointer(Categories::TOADLET_RIBBIT,
			"ALPlayer: Null Stream");
		return NULL;
	}

	AudioStream::ptr decoder;
	#if defined(TOADLET_PLATFORM_OSX)
		decoder=AudioStream::ptr(new_CoreAudioDecoder());
	#else
		if(mimeType=="application/ogg"){
			decoder=AudioStream::ptr(new_OggVorbisDecoder());
		}
		else if(mimeType=="audio/x-wav"){
			decoder=AudioStream::ptr(new_WaveDecoder());
		}
	#endif

	if(decoder==NULL){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"ALPlayer: MIME type not supported: " + mimeType);
		return NULL;
	}

	if(decoder->startStream(stream)==false){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"ALPlayer: Error starting decoder stream");
		return NULL;
	}

	return decoder;
}

void ALPlayer::setListenerTranslate(const Vector3 &translate){
	MathConversion::scalarToFloat(cacheVector3,translate);

	// Not sure if this belongs here, but we'll just just in case
	// NAN values will lock the computer
	if(Extents::isNAN(cacheVector3.x)==false){
		return;
	}

	alListenerfv(AL_POSITION,cacheVector3.getData());
}

void ALPlayer::setListenerRotate(const Matrix3x3 &rotate){
	ALfloat final[6];

	final[0]=MathConversion::scalarToFloat(rotate.at(0,1));
	final[1]=MathConversion::scalarToFloat(rotate.at(1,1));
	final[2]=MathConversion::scalarToFloat(rotate.at(2,1));

	final[3]=MathConversion::scalarToFloat(rotate.at(0,2));
	final[4]=MathConversion::scalarToFloat(rotate.at(1,2));
	final[5]=MathConversion::scalarToFloat(rotate.at(2,2));

	// Not sure if this belongs here, but we'll just just in case
	// NAN values will lock the computer
	if(Extents::isNAN(final[0])==false){
		return;
	}

	alListenerfv(AL_ORIENTATION,final);

	TOADLET_CHECK_ALERROR("setListenerRotate");
}

void ALPlayer::setListenerVelocity(const Vector3 &velocity){

	// Doppler effect doesnt work on other platforms...  bad OpenAL?
	#if defined(TOADLET_PLATFORM_WIN32)
		MathConversion::scalarToFloat(cacheVector3,velocity);

		// Not sure if this belongs here, but we'll just just in case
		// NAN values will lock the computer
		if(Extents::isNAN(cacheVector3.x)==false){
			return;
		}

		alListenerfv(AL_VELOCITY,cacheVector3.getData());
	#endif

	TOADLET_CHECK_ALERROR("end setListenerVelocity");
}

ALenum ALPlayer::getALFormat(int bitsPerSample,int channels){
	ALenum format=0;

	if(bitsPerSample==8 && channels==1){
		format=AL_FORMAT_MONO8;
	}
	else if(bitsPerSample==16 && channels==1){
		format=AL_FORMAT_MONO16;
	}
	else if(bitsPerSample==8 && channels==2){
		format=AL_FORMAT_STEREO8;
	}
	else if(bitsPerSample==16 && channels==2){
		format=AL_FORMAT_STEREO16;
	}

	return format;
}

void ALPlayer::setListenerGain(scalar gain){
	alListenerf(AL_GAIN,MathConversion::scalarToFloat(gain));
	
	TOADLET_CHECK_ALERROR("setListenerGain");
}

void ALPlayer::setDopplerFactor(scalar factor){
	alDopplerFactor(factor);

	TOADLET_CHECK_ALERROR("setDopplerFactor");
}

void ALPlayer::setDopplerVelocity(scalar velocity){
	alDopplerVelocity(velocity);

	TOADLET_CHECK_ALERROR("setDopplerVelocity");
}

void ALPlayer::setDefaultRolloffFactor(scalar factor){
	mDefaultRolloffFactor=factor;
	int i;
	for(i=0;i<mAllSources.size();++i){
		alSourcef(mAllSources[i],AL_ROLLOFF_FACTOR,MathConversion::scalarToFloat(mDefaultRolloffFactor));
	}

	TOADLET_CHECK_ALERROR("setDefaultRolloffFactor");
}

const CapabilitySet &ALPlayer::getCapabilitySet(){
	return mCapabilitySet;
}

ALuint ALPlayer::checkoutSourceHandle(ALAudio *audio){
	mAudios.add(audio);

	ALuint handle=mSourcePool[0];
	mSourcePool.removeAt(0);
	return handle;
}

void ALPlayer::checkinSourceHandle(ALAudio *audio,ALuint source){
	mSourcePool.add(source);

	mAudios.remove(audio);
}

}
}

