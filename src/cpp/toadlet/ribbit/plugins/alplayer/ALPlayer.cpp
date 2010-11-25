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

#include "../decoders/wavedecoder/WaveDecoder.h"
#if defined(TOADLET_HAS_OGGVORBIS)
	#include "../decoders/oggvorbisdecoder/OggVorbisDecoder.h"
#endif
#if defined(TOADLET_HAS_SIDPLAY)
	#include "../decoders/siddecoder/SIDDecoder.h"
#endif
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

TOADLET_C_API AudioPlayer* new_ALPlayer(){
	return new ALPlayer();
}

#if defined(TOADLET_BUILD_DYNAMIC)
	TOADLET_C_API AudioPlayer* new_AudioPlayer(){
		return new ALPlayer();
	}
#endif

ALPlayer::ALPlayer():
	mDevice(NULL),
	mContext(NULL),
	//mAudios,
	//mSourcePool,
	//mAllSources,
	mBufferFadeTime(0),

	//mMutex,

	//mCapabilitySet,

	alBufferDataStatic(NULL)
{
	mBufferFadeTime=100;
}

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
				case Option_BUFFER_FADE_TIME:
					mBufferFadeTime=options[i++];
					Logger::alert(Categories::TOADLET_PEEPER,
						String("Setting BufferFadeTime to:")+mBufferFadeTime);
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

	alBufferDataStatic=(proc_alBufferDataStatic)alcGetProcAddress(mDevice,(ALCchar*)"alBufferDataStatic");
	if(alBufferDataStatic!=NULL){
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

	/// @todo: Change this to a loop with alGenSources till it returns an error
	mCapabilitySet.maxSources=8;
	mCapabilitySet.streaming=true;
	mCapabilitySet.positional=true;
	mCapabilitySet.mimeTypes.add(WaveDecoder::mimeType());
	#if defined(TOADLET_HAS_OGGVORBIS)
		mCapabilitySet.mimeTypes.add(OggVorbisDecoder::mimeType());
	#endif
	#if defined(TOADLET_HAS_SIDPLAY)
		mCapabilitySet.mimeTypes.add(SIDDecoder::mimeType());
	#endif

	alListenerf(AL_GAIN,1.0);
	TOADLET_CHECK_ALERROR("alListenerf");

	alDopplerFactor(1.0);
	TOADLET_CHECK_ALERROR("alDopplerFactor");
	alDopplerVelocity(340.0); /// @todo: Move the doppler parameters to the interface
	TOADLET_CHECK_ALERROR("alDopplerVelocity");

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

	return true;
}

bool ALPlayer::destroy(){
	Logger::alert(Categories::TOADLET_RIBBIT,
		"destroying ALPlayer");

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
	lock();
		int i;
		for(i=0;i<mAudios.size();++i){
			mAudios[i]->update(dt);
		}
		#if defined(TOADLET_PLATFORM_OSX)
			for(i=0;i<mCoreAudios.size();++i){
				mCoreAudios[i]->update(dt);
			}
		#endif
	unlock();
}

AudioStream::ptr ALPlayer::startAudioStream(io::Stream::ptr stream,const String &mimeType){
	if(stream==NULL){
		Error::nullPointer(Categories::TOADLET_RIBBIT,
			"null Stream");
		return NULL;
	}

	/// @todo: Change this so the decoder creation is as it is in toadlets resource system, or at least into ribbit core, so we dont duplicate it per player
	AudioStream::ptr decoder;
	#if defined(TOADLET_PLATFORM_OSX)
		decoder=AudioStream::ptr(new CoreAudioDecoder());
	#else
		if(mimeType==WaveDecoder::mimeType()){
			decoder=AudioStream::ptr(new WaveDecoder());
		}
		#if defined(TOADLET_HAS_OGGVORBIS)
			if(mimeType==OggVorbisDecoder::mimeType()){
				decoder=AudioStream::ptr(new OggVorbisDecoder());
			}
		#endif
		#if defined(TOADLET_HAS_SIDPLAY)
			if(mimeType==SIDDecoder::mimeType()){
				decoder=AudioStream::ptr(new SIDDecoder());
			}
		#endif
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

void ALPlayer::setListenerGain(scalar gain){
	alListenerf(AL_GAIN,MathConversion::scalarToFloat(gain));
	
	TOADLET_CHECK_ALERROR("setListenerGain");
}

const CapabilitySet &ALPlayer::getCapabilitySet(){
	return mCapabilitySet;
}

ALuint ALPlayer::checkoutSourceHandle(ALAudio *audio){
	mAudios.add(audio);

	ALuint handle=-1;
	if(mSourcePool.size()>0){
		handle=mSourcePool[0];
		mSourcePool.removeAt(0);
	}
	return handle;
}

void ALPlayer::checkinSourceHandle(ALAudio *audio,ALuint source){
	mSourcePool.add(source);

	mAudios.remove(audio);
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

}
}

