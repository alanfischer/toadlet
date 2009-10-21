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
#include "ALAudioPeer.h"
#include "ALAudioBufferPeer.h"
#include <toadlet/egg/EndianConversion.h>
#include <toadlet/egg/System.h>
#include <toadlet/egg/Extents.h>
#include <toadlet/egg/Error.h>
#include <stdlib.h>
#if defined(TOADLET_PLATFORM_POSIX)
	#include <string.h>
#endif

#if defined(TOADLET_PLATFORM_OSX)
	#include "../decoders/coreaudiodecoder/CoreAudioDecoder.h"
	#include "platform/osx/CoreAudioPeer.h"
#endif

#if defined(TOADLET_PLATFORM_WIN32)
	#if defined(TOADLET_LIBOPENAL_NAME)
		#pragma comment(lib,TOADLET_LIBOPENAL_NAME)
	#endif
#endif

#ifdef TOADLET_LITTLE_ENDIAN
	#define TOADLET_NATIVE_FORMAT
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

ALPlayer::ALPlayer(){
	mBufferFadeInTime=0;
	mStopThread=false;
	mThread=NULL;
	mStarted=false;
}

ALPlayer::~ALPlayer(){
	TOADLET_ASSERT(mStarted==false);
}

bool ALPlayer::startup(int *options){
	Logger::log(Categories::TOADLET_RIBBIT,Logger::Level_ALERT,
		"ALPlayer: Startup started");

	if(options!=NULL){
		int i=0;
		while(options[i]!=0){
			switch(options[i++]){
				case Option_FADE_IN_BUFFER_TIME:
					mBufferFadeInTime=options[i++];
					Logger::log(Categories::TOADLET_PEEPER,Logger::Level_ALERT,
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
		Logger::log(Categories::TOADLET_RIBBIT,Logger::Level_ALERT,
			String("alem_getAccelerated result:")+alemAcceleratedResult);
	#endif

	// Initialize Open AL manually
	mDevice=alcOpenDevice(NULL);
	TOADLET_CHECK_ALERROR("alcOpenDevice");

	if(mDevice==NULL){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"ALPlayer::startup: Invalid device");
		return false;
	}

	toadlet_alBufferDataStatic=(proc_alBufferDataStatic)alcGetProcAddress(mDevice,(ALCchar*)"alBufferDataStatic");
	if(toadlet_alBufferDataStatic!=NULL){
		Logger::log(Categories::TOADLET_RIBBIT,Logger::Level_ALERT,
			"using alBufferDataStatic extension");
	}

	mContext=alcCreateContext(mDevice,NULL);
	TOADLET_CHECK_ALERROR("alcCreateContext");

	if(mContext==NULL){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"ALPlayer::startup: Invalid context");

		alcCloseDevice(mDevice);
		mDevice=NULL;
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

	Logger::log(Categories::TOADLET_RIBBIT,Logger::Level_DEBUG,
		"ALPlayer: starting thread");

	mStopThread=false;
	mThread=new Thread(this);
	mThread->start();

	Logger::log(Categories::TOADLET_RIBBIT,Logger::Level_ALERT,
		"ALPlayer: Startup completed");

	mStarted=true;

	return true;
}

bool ALPlayer::shutdown(){
	Logger::log(Categories::TOADLET_RIBBIT,Logger::Level_ALERT,
		"ALPlayer: Shutdown started");
	Logger::log(Categories::TOADLET_RIBBIT,Logger::Level_DEBUG,
		"ALPlayer: Stopping thread");

	mStopThread=true;
	if(mThread!=NULL){
		while(mThread->isAlive()){
			System::msleep(10);
		}
		delete mThread;
		mThread=NULL;
	}

	Logger::log(Categories::TOADLET_RIBBIT,Logger::Level_ALERT,
		"ALPlayer: Thread stopped");

	alDeleteSources(mAllSources.size(),&mAllSources[0]);

	if(mContext!=NULL){
		alcDestroyContext(mContext);
		mContext=NULL;
	}

	if(mDevice!=NULL){
		alcCloseDevice(mDevice);
		mDevice=NULL;
	}

	int i;
	for(i=0;i<mAudioPeers.size();++i){
		mAudioPeers[i]->internal_playerShutdown();
	}
	#if defined(TOADLET_PLATFORM_OSX)
		for(i=0;i<mCoreAudioPeers.size();++i){
			mCoreAudioPeers[i]->internal_playerShutdown();
		}
	#endif

	Logger::log(Categories::TOADLET_RIBBIT,Logger::Level_ALERT,
		"ALPlayer: Shutdown completed");

	mStarted=false;

	return true;
}

AudioBufferPeer *ALPlayer::createAudioBufferPeer(AudioBuffer *audioBuffer){
	AudioStream::ptr decoder=startAudioStream(audioBuffer->getInputStream(),audioBuffer->getMimeType());

	if(decoder==NULL){
		return NULL;
	}

	#if defined(TOADLET_PLATFORM_OSX)
		if(((CoreAudioDecoder*)decoder.get())->isVariableBitRate()){
			Error::unknown(Categories::TOADLET_RIBBIT,
				"Variable bit rate streams not supported as Audio Buffers");
			return NULL;
		}
	#endif

	char *buffer=0;
	int length=0;
	int channels=decoder->getChannels();
	int sps=decoder->getSamplesPerSecond();
	int bps=decoder->getBitsPerSample();

	decodeStream(decoder,buffer,length);

	audioBuffer->setStream(NULL,NULL);

	ALenum format=getALFormat(bps,channels);

	// Lets us programatically reduce popping on some platforms
	if(mBufferFadeInTime>0){
		int stf=sps*mBufferFadeInTime/1000;
		int ns=length/channels/(bps/8);
		if(stf>ns){stf=ns;}
		int sampsize=channels*(bps/8);

		int i,j;
		for(i=0;i<stf;++i){
			for(j=0;j<sampsize;++j){
				buffer[i*sampsize+j]=(char)(((int)buffer[i*sampsize+j])*i/stf);
			}
		}
	}
	
	ALAudioBufferPeer *peer=new ALAudioBufferPeer(format,sps,buffer,length);
	if(peer->ownsBuffer()==false){
		delete[] buffer;
	}

	return peer;
}

AudioPeer *ALPlayer::createBufferedAudioPeer(Audio *audio,AudioBuffer::ptr buffer){
	lock();

	ALAudioPeer *peer=new ALAudioPeer(this);

	if(mSourcePool.size()>0){
		peer->setSourceHandle(mSourcePool[0]);
		mSourcePool.remove(0);
		mAudioPeers.add(peer);
	}
	else{
		peer->setSourceHandle(0);
	}

	unlock();

	// Reset the gain so it takes into account the group gain
	peer->setGain(peer->getGain());

	peer->loadAudioBuffer(buffer);

	return peer;
}

AudioPeer *ALPlayer::createStreamingAudioPeer(Audio *audio,InputStream::ptr in,const String &mimeType){
	AudioStream::ptr decoder=startAudioStream(in,mimeType);

	if(decoder==NULL){
		return NULL;
	}

	#if defined(TOADLET_PLATFORM_OSX)
		if(((CoreAudioDecoder*)decoder.get())->isVariableBitRate()){
			CoreAudioPeer *peer=new CoreAudioPeer(this);

			lock();
			mCoreAudioPeers.add(peer);
			unlock();

			peer->loadAudioStream(decoder);

			return peer;
		}
	#endif

	lock();

	ALAudioPeer *peer=new ALAudioPeer(this);

	if(mSourcePool.size()>0){
		peer->setSourceHandle(mSourcePool[0]);
		mSourcePool.remove(0);
		mAudioPeers.add(peer);
	}
	else{
		peer->setSourceHandle(0);
	}

	unlock();

	peer->loadAudioStream(decoder);

	return peer;
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
			for(i=0;i<mAudioPeers.size();++i){
				update(mAudioPeers[i],dt);
			}
			#if defined(TOADLET_PLATFORM_OSX)
				for(i=0;i<mCoreAudioPeers.size();++i){
					update(mCoreAudioPeers[i],dt);
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

void ALPlayer::update(ALAudioPeer *audioPeer,int dt){
	scalar fdt=Math::fromMilli(dt);

	scalar gain=audioPeer->getGain();
	scalar target=audioPeer->getTargetGain();
	scalar speed=audioPeer->getFadeTime();
	if(gain!=target){
		speed=Math::mul(speed,fdt);
		if(gain<target){
			gain+=speed;
			if(gain>target){
				gain=target;
			}
		}
		else{
			gain-=speed;
			if(gain<target){
				gain=target;
			}
		}

		audioPeer->internal_setGain(gain);
	}

	if(audioPeer->getStream()){
		unsigned char buffer[bufferSize];
		ALenum format=getALFormat(audioPeer->getStream()->getBitsPerSample(),audioPeer->getStream()->getChannels());
		int total=0;
		if(audioPeer->getStreamingBuffers()==NULL){
			audioPeer->setStreamingBuffers(new unsigned int[numBuffers]);
			alGenBuffers(numBuffers,audioPeer->getStreamingBuffers());
			TOADLET_CHECK_ALERROR("update::alGenBuffers");

			int i;
			for(i=0;i<numBuffers;++i){
				int amount=readAudioData(audioPeer,buffer,bufferSize);
				total=total+amount;
				alBufferData(audioPeer->getStreamingBuffers()[i],format,buffer,amount,audioPeer->getStream()->getSamplesPerSecond());
				TOADLET_CHECK_ALERROR("update::alBufferData");
			}
			if(total==0){
				Error::unknown(Categories::TOADLET_RIBBIT,
					"ALPlayer: Bad audio stream");
				return;
			}

			audioPeer->setTotalBuffersPlayed(numBuffers);

			alSourceQueueBuffers(audioPeer->getSourceHandle(),numBuffers,audioPeer->getStreamingBuffers());
			TOADLET_CHECK_ALERROR("update::alSourceQueueBuffers");

			alSourcePlay(audioPeer->getSourceHandle());
			TOADLET_CHECK_ALERROR("update::alSourcePlay");
		}
		else{
			int processed=0;

			alGetSourcei(audioPeer->getSourceHandle(),AL_BUFFERS_PROCESSED,&processed);
			TOADLET_CHECK_ALERROR("update::alGetSourcei");

			if(audioPeer->getTotalBuffersPlayed()>processed && processed>0){
				unsigned char buffer[bufferSize];

				int i;
				for(i=(audioPeer->getTotalBuffersPlayed()-processed);i<audioPeer->getTotalBuffersPlayed();++i){
					unsigned int bufferID=audioPeer->getStreamingBuffers()[i % numBuffers];
					
					alSourceUnqueueBuffers(audioPeer->getSourceHandle(),1,&bufferID);
					TOADLET_CHECK_ALERROR("update::alSourceUnqueueBuffers");

					int amount=readAudioData(audioPeer,buffer,bufferSize);
					total=total+amount;
					if(amount>0){
						alBufferData(bufferID,format,buffer,amount,audioPeer->getStream()->getSamplesPerSecond());
						TOADLET_CHECK_ALERROR("update::alBufferData");
						alSourceQueueBuffers(audioPeer->getSourceHandle(),1,&bufferID);
						TOADLET_CHECK_ALERROR("update::alSourceQueueBuffers");
					}
				}
			}

			audioPeer->setTotalBuffersPlayed(audioPeer->getTotalBuffersPlayed() + processed);

			if(processed>=numBuffers){
				alSourcePlay(audioPeer->getSourceHandle());
				TOADLET_CHECK_ALERROR("update::alSourcePlay");
			}
			else if(processed>0 && total==0){
				alSourceStop(audioPeer->getSourceHandle());
				TOADLET_CHECK_ALERROR("update::alSourceStop");
				alDeleteBuffers(numBuffers,audioPeer->getStreamingBuffers());
				TOADLET_CHECK_ALERROR("update::alDeleteBuffers");

				delete[] audioPeer->getStreamingBuffers();
				audioPeer->setStreamingBuffers(NULL);

				audioPeer->setStream(NULL);
			}
		}
	}
}

#if defined(TOADLET_PLATFORM_OSX)
void ALPlayer::update(CoreAudioPeer *audioPeer,int dt){
	scalar fdt=Math::fromMilli(dt);

	scalar gain=audioPeer->getGain();
	scalar target=audioPeer->getTargetGain();
	scalar speed=audioPeer->getFadeTime();
	if(gain!=target){
		speed=Math::mul(speed,fdt);
		if(gain<target){
			gain+=speed;
			if(gain>target){
				gain=target;
			}
		}
		else{
			gain-=speed;
			if(gain<target){
				gain=target;
			}
		}

		audioPeer->internal_setGain(gain);
	}
}
#endif

int ALPlayer::readAudioData(ALAudioPeer *peer,unsigned char *buffer,int bsize){
	int amount=peer->getStream()->read((char*)buffer,bsize);
	if(amount==0 && peer->getLooping()){
		peer->getStream()->reset();
		amount=peer->getStream()->read((char*)buffer,bsize);
	}

	#if !defined(TOADLET_NATIVE_FORMAT)
		if(peer->getStream()->getBitsPerSample()==16){
			int j=0;
			while(j<amount){
				littleInt16InPlace((int16&)buffer[j]);
				j+=2;
			}
		}
	#endif

	return amount;
}

void ALPlayer::decodeStream(AudioStream *decoder,char *&finalBuffer,int &finalLength){
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

AudioStream::ptr ALPlayer::startAudioStream(io::InputStream::ptr in,const String &mimeType){
	if(in==NULL){
		Error::nullPointer(Categories::TOADLET_RIBBIT,
			"ALPlayer: Null InputStream");
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

	if(decoder->startStream(in)==false){
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

void ALPlayer::setGroupGain(const String &group,scalar gain){
	lock();
	mGainGroup[group]=gain;
	unlock();
	int i;
	for(i=0;i<mAudioPeers.size();i++){
		if(mAudioPeers[i]->getGroup()==group){
			// Update the Audio's gain with respect to the group
			mAudioPeers[i]->setGroup(group); 
		}
	}

	TOADLET_CHECK_ALERROR("setGroupGain");
}

void ALPlayer::removeGroup(const String &group){
	lock();
	Map<String,scalar>::iterator it=mGainGroup.find(group);
	if(it!=mGainGroup.end()){
		mGainGroup.erase(it);
	}
	unlock();
	int i;
	for(i=0;i<mAudioPeers.size();i++){
		if(mAudioPeers[i]->getGroup()==group){
			// Reset the Audio's gain with respect to this group
			mAudioPeers[i]->setGroup(group);
		}
	}
	
	TOADLET_CHECK_ALERROR("removeGroup");
}

scalar ALPlayer::getGroupGain(const String &group){
	scalar gain=Math::ONE;
	lock();
	gain=internal_getGroupGain(group);
	unlock();
	return gain;
}

scalar ALPlayer::internal_getGroupGain(const String &group){
	Map<String,scalar>::iterator it=mGainGroup.find(group);
	if(it!=mGainGroup.end()){
		return it->second;
	}
	return Math::ONE;
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

void ALPlayer::internal_audioPeerDestroy(ALAudioPeer *audioPeer){
	lock();

	if(audioPeer!=NULL && audioPeer->getSourceHandle()){
		int source=audioPeer->getSourceHandle();
		if(alIsSource(source)){
			alSourceStop(source);
			alSourcei(source,AL_BUFFER,0);
		}
		mSourcePool.add(source);
	}

	mAudioPeers.remove(audioPeer);

	unlock();

	TOADLET_CHECK_ALERROR("internal_audioPeerDestroy");
}

#if defined(TOADLET_PLATFORM_OSX)
void ALPlayer::internal_audioPeerDestroy(CoreAudioPeer *audioPeer){
	lock();

	mCoreAudioPeers.remove(audioPeer);

	unlock();
}
#endif

}
}

