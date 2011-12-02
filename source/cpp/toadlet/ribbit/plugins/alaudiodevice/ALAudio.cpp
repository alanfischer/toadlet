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

#include "ALAudioDevice.h"
#include "ALAudio.h"
#include "ALAudioBuffer.h"
#include <toadlet/egg/MathConversion.h>
#include <toadlet/egg/EndianConversion.h>
#include <toadlet/egg/System.h>

namespace toadlet{
namespace ribbit{

/// @todo: Move these into member variables of ALAudio
const static int bufferSize=1024*4;
const static int numBuffers=4;

/// @todo: When playing back the Stove sample video, when we stop, rewind, and play again the audio timing seems to start later sometimes.  We probably need to flush more buffers.
ALAudio::ALAudio(ALAudioDevice *audioDevice):
	mDevice(NULL),
	mGlobal(false),
	mLooping(false),
	mHandle(0),
	mStreamingBuffers(NULL),
	mTotalBuffersPlayed(0),mTotalBuffersQueued(0),
	mGain(0)
	//mAudioBuffer,
	//mAudioStream,
{
	mDevice=audioDevice;

	mGain=Math::ONE;
}

ALAudio::~ALAudio(){
	destroy();
}

bool ALAudio::create(AudioBuffer::ptr audioBuffer){
	if(audioBuffer==NULL){
		Logger::warning(Categories::TOADLET_RIBBIT,"invalid audioBuffer");
		return false;
	}
	
	mDevice->activate();

	mHandle=mDevice->checkoutSourceHandle(this);

	mAudioStream=NULL;
	mAudioBuffer=audioBuffer;

	ALAudioBuffer *alaudioBuffer=((ALAudioBuffer*)audioBuffer->getRootAudioBuffer());

	if(alIsSource(mHandle)){
		alSourceStop(mHandle);
		TOADLET_CHECK_ALERROR("loadAudioBuffer::alSourceStop");

		alSourcei(mHandle,AL_BUFFER,alaudioBuffer->getHandle());
		TOADLET_CHECK_ALERROR("loadAudioBuffer::alSourcei");
 	}

	return true;
}

bool ALAudio::create(AudioStream::ptr stream){
	mDevice->activate();

	mHandle=mDevice->checkoutSourceHandle(this);

	mAudioStream=stream;
	mAudioBuffer=NULL;

	update(0);

	if(alIsSource(mHandle)){
		alSourceStop(mHandle);
		TOADLET_CHECK_ALERROR("alSourceStop");
	}

	return true;
}

void ALAudio::destroy(){
	if(mDevice!=NULL && mHandle!=0){
		mDevice->activate();

		stop();

		if(alIsSource(mHandle)){
			alSourceStop(mHandle);
			alSourcei(mHandle,AL_BUFFER,0);
		}
		mDevice->checkinSourceHandle(this,mHandle);
		mHandle=0;
	}
}

bool ALAudio::play(){
	if(mAudioStream!=NULL && mStreamingBuffers==NULL){
		unsigned char buffer[bufferSize];
		int total=0;

		mStreamingBuffers=new ALuint[numBuffers];
		alGenBuffers(numBuffers,mStreamingBuffers);
		TOADLET_CHECK_ALERROR("alGenBuffers");

		int i;
		for(i=0;i<numBuffers;++i){
			int amount=readAudioData(buffer,bufferSize);
			total=total+amount;
			alBufferData(mStreamingBuffers[i],
				ALAudioDevice::getALFormat(mAudioStream->getAudioFormat()),
				buffer,amount,mAudioStream->getAudioFormat()->getSamplesPerSecond());
			TOADLET_CHECK_ALERROR("alBufferData");
		}

		mTotalBuffersPlayed=numBuffers;
		mTotalBuffersQueued=numBuffers;

		alSourceQueueBuffers(mHandle,numBuffers,mStreamingBuffers);
		TOADLET_CHECK_ALERROR("alSourceQueueBuffers");
	}

	if(alIsSource(mHandle)){
		alSourcePlay(mHandle);
		TOADLET_CHECK_ALERROR("alSourcePlay");
	}

	return true;
}

bool ALAudio::stop(){
	if(alIsSource(mHandle)){
		alSourceStop(mHandle);
		TOADLET_CHECK_ALERROR("alSourceStop");
	}

	if(mAudioStream!=NULL && mStreamingBuffers!=NULL){
		alSourceUnqueueBuffers(mHandle,numBuffers,mStreamingBuffers);
		alDeleteBuffers(numBuffers,mStreamingBuffers);
		TOADLET_CHECK_ALERROR("alDeleteBuffers");

		delete[] mStreamingBuffers;
		mStreamingBuffers=NULL;
	}

	return true;
}

bool ALAudio::getPlaying() const{
	int state=0;
	if(alIsSource(mHandle)){
		alGetSourcei(mHandle,AL_SOURCE_STATE,&state);
	}
	return state==AL_PLAYING;
}

bool ALAudio::getFinished() const{
	// First check the saved looping, in case the source is invalid
	// Also never stop until our stream is depleated
	if(mLooping==true || mAudioStream!=NULL){
		return false;
	}

	int state=0;
	if(alIsSource(mHandle)){
		alGetSourcei(mHandle,AL_SOURCE_STATE,&state);
	}
	if(state!=AL_PLAYING && state!=AL_PAUSED){
		return true;
	}
	return false;
}

void ALAudio::setLooping(bool looping){
	mLooping=looping;
	if(mAudioStream==NULL && alIsSource(mHandle)){
		alSourcei(mHandle,AL_LOOPING,looping);
	}
}

bool ALAudio::getLooping() const{
	int looping=0;
	if(alIsSource(mHandle)){
		alGetSourcei(mHandle,AL_LOOPING,&looping);
	}

	if(looping>0){
		return true;
	}
	else{
		return mLooping;
	}
}

void ALAudio::setGain(scalar gain){
	mGain=gain;
	if(alIsSource(mHandle)){
		alSourcef(mHandle,AL_GAIN,MathConversion::scalarToFloat(gain));
	}
}

void ALAudio::setRolloffFactor(scalar factor){
	if(alIsSource(mHandle)){
		alSourcef(mHandle,AL_ROLLOFF_FACTOR,MathConversion::scalarToFloat(factor));
	}
}

scalar ALAudio::getRolloffFactor() const{
	float r=0;
	if(alIsSource(mHandle)){
		alGetSourcef(mHandle,AL_ROLLOFF_FACTOR,&r);
	}
	return MathConversion::floatToScalar(r);
}

void ALAudio::setGlobal(bool global){
	mGlobal=global;
	if(alIsSource(mHandle)){
		if(mGlobal){
			ALfloat zero[]={0,0,0};
			alSourcefv(mHandle,AL_POSITION,zero);
			alSourcei(mHandle,AL_SOURCE_RELATIVE,true);
		}
		else{
			alSourcei(mHandle,AL_SOURCE_RELATIVE,false);
		}
	}
}

void ALAudio::setPitch(scalar pitch){
	if(alIsSource(mHandle)){
		alSourcef(mHandle,AL_PITCH,MathConversion::scalarToFloat(pitch));
	}
}

scalar ALAudio::getPitch() const{
	float pitch=0;
	if(alIsSource(mHandle)){
		alGetSourcef(mHandle,AL_PITCH,&pitch);
	}
	return MathConversion::floatToScalar(pitch);
}

void ALAudio::setPosition(const Vector3 &position){
	if(alIsSource(mHandle)){
		if(mGlobal){
			MathConversion::scalarToFloat(mDevice->cacheVector3,Math::ZERO_VECTOR3);
		}
		else{
			MathConversion::scalarToFloat(mDevice->cacheVector3,position);
		}
		alSourcefv(mHandle,AL_POSITION,(ALfloat*)mDevice->cacheVector3.getData());
	}
}

void ALAudio::setVelocity(const Vector3 &velocity){
	if(alIsSource(mHandle)){
		if(mGlobal){
			MathConversion::scalarToFloat(mDevice->cacheVector3,Math::ZERO_VECTOR3);
		}
		else{
			MathConversion::scalarToFloat(mDevice->cacheVector3,velocity);
		}
		alSourcefv(mHandle,AL_VELOCITY,(ALfloat*)mDevice->cacheVector3.getData());
	}
}

void ALAudio::update(int dt){
	if(mAudioStream!=NULL){
		updateStreaming(dt);
	}
}

void ALAudio::updateStreaming(int dt){
	unsigned char buffer[bufferSize];
	int total=0;
	if(mStreamingBuffers!=NULL){
		int processed=0;

		alGetSourcei(mHandle,AL_BUFFERS_PROCESSED,&processed);
		TOADLET_CHECK_ALERROR("update::alGetSourcei");

		if(mTotalBuffersPlayed>processed && processed>0){
			int i;
			for(i=(mTotalBuffersPlayed-processed);i<mTotalBuffersPlayed;++i){
				unsigned int bufferID=mStreamingBuffers[i % numBuffers];
				
				alSourceUnqueueBuffers(mHandle,1,&bufferID);
				TOADLET_CHECK_ALERROR("update::alSourceUnqueueBuffers");

				int amount=readAudioData(buffer,bufferSize);
				total=total+amount;
				if(amount>0){
					alBufferData(bufferID,
						ALAudioDevice::getALFormat(mAudioStream->getAudioFormat()),
						buffer,amount,mAudioStream->getAudioFormat()->getSamplesPerSecond());
					TOADLET_CHECK_ALERROR("update::alBufferData");
					alSourceQueueBuffers(mHandle,1,&bufferID);
					TOADLET_CHECK_ALERROR("update::alSourceQueueBuffers");

					mTotalBuffersQueued+=1;
				}
			}
		}

		mTotalBuffersPlayed+=processed;
		mTotalBuffersQueued-=processed;

		if(processed>=numBuffers){
			alSourcePlay(mHandle);
			TOADLET_CHECK_ALERROR("update::alSourcePlay");
		}
		else if(processed>0 && total<0){
			stop();
		}
	}
}

int ALAudio::readAudioData(tbyte *buffer,int bsize){
	int amount=mAudioStream->read(buffer,bsize);
	if(amount<0 && mLooping){
		mAudioStream->reset();
		amount=mAudioStream->read(buffer,bsize);
	}
	else if(amount==0){
		amount=4;
		memset(buffer,0,4);
	}

	#if !defined(TOADLET_NATIVE_FORMAT)
		if(mAudioStream->getAudioFormat()->bitsPerSample==16){
			int j=0;
			while(j<amount){
				littleInt16InPlace((int16&)buffer[j]);
				j+=2;
			}
		}
	#endif

	return amount;
}

}
}