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
#include <toadlet/egg/MathConversion.h>
#include <toadlet/egg/EndianConversion.h>
#include <toadlet/egg/Exception.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace ribbit{

const static int bufferSize=4096*4;
const static int numBuffers=8;

ALAudio::ALAudio(ALPlayer *audioPlayer):
	mAudioPlayer(NULL),
	mGlobal(false),
	mLooping(false),
	mHandle(0),
	mStreamingBuffers(NULL),
	mTotalBuffersPlayed(0),
	mTargetGain(0),
	mGain(0),
	mInternalFadeTime(0),
	mFadeTime(0),
	mPlayTime(0)
	//mAudioBuffer,
	//mAudioStream,
{
	mAudioPlayer=audioPlayer;

	mGain=Math::ONE;

	// This is used to reduce popping
	mInternalFadeTime=100;
}

ALAudio::~ALAudio(){
	destroy();
}

bool ALAudio::create(AudioBuffer::ptr audioBuffer){
	destroy();

	if(audioBuffer==NULL){
		Logger::warning(Categories::TOADLET_RIBBIT,"invalid audioBuffer");
		return false;
	}
	
	mHandle=mAudioPlayer->checkoutSourceHandle(this);

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

bool ALAudio::create(Stream::ptr stream,const String &mimeType){
	destroy();

	AudioStream::ptr decoder=NULL;
	TOADLET_TRY{
		decoder=mAudioPlayer->startAudioStream(stream,mimeType);
	}TOADLET_CATCH(const Exception &){
		decoder=NULL;
	}

	if(decoder==NULL){
		return false;
	}

	mHandle=mAudioPlayer->checkoutSourceHandle(this);

	mAudioStream=decoder;
	mAudioBuffer=NULL;

	mAudioPlayer->lock();
		update(0);
	mAudioPlayer->unlock();

	if(alIsSource(mHandle)){
		alSourceStop(mHandle);
		TOADLET_CHECK_ALERROR("loadAudioStream::alSourceStop");
	}

	return true;
}

void ALAudio::destroy(){
	if(mAudioPlayer!=NULL && mHandle!=0){
		if(alIsSource(mHandle)){
			alSourceStop(mHandle);
			alSourcei(mHandle,AL_BUFFER,0);
		}
		mAudioPlayer->checkinSourceHandle(this,mHandle);
		mHandle=0;
	}
}

bool ALAudio::play(){
	// Check for starting the internal fade in
	if(mFadeTime==0){
		fadeToGain(mTargetGain,mInternalFadeTime);
		setImmediateGain(0);
	}

	mPlayTime=0;

	alSourcePlay(mHandle);
	TOADLET_CHECK_ALERROR("playAudioBuffer::alSourcePlay");

	return true;
}

bool ALAudio::stop(){
	alSourceStop(mHandle);
	TOADLET_CHECK_ALERROR("playAudioStream::alSourcePlay");

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
	if(mAudioPlayer!=NULL){
		mAudioPlayer->lock();
			setImmediateGain(gain);
			mTargetGain=gain;
			mFadeTime=0;
		mAudioPlayer->unlock();
	}
}

void ALAudio::fadeToGain(scalar gain,int time){
	if(mAudioPlayer!=NULL){
		mAudioPlayer->lock();
			mTargetGain=gain;
			mFadeTime=time;
		mAudioPlayer->unlock();
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
			MathConversion::scalarToFloat(mAudioPlayer->cacheVector3,Math::ZERO_VECTOR3);
		}
		else{
			MathConversion::scalarToFloat(mAudioPlayer->cacheVector3,position);
		}
		alSourcefv(mHandle,AL_POSITION,(ALfloat*)mAudioPlayer->cacheVector3.getData());
	}
}

void ALAudio::setVelocity(const Vector3 &velocity){
	if(alIsSource(mHandle)){
		if(mGlobal){
			MathConversion::scalarToFloat(mAudioPlayer->cacheVector3,Math::ZERO_VECTOR3);
		}
		else{
			MathConversion::scalarToFloat(mAudioPlayer->cacheVector3,velocity);
		}
		alSourcefv(mHandle,AL_VELOCITY,(ALfloat*)mAudioPlayer->cacheVector3.getData());
	}
}

void ALAudio::update(int dt){
	if(mAudioBuffer!=NULL && getPlaying()){
		mPlayTime+=dt;
		// Check for starting the internal fade out
		if(mFadeTime==0){
			ALAudioBuffer *alaudioBuffer=((ALAudioBuffer*)mAudioBuffer->getRootAudioBuffer());
			if(mPlayTime>=alaudioBuffer->mLengthTime-mInternalFadeTime){
				fadeToGain(0,mInternalFadeTime);
			}
		}
	}

	scalar fdt=Math::fromMilli(dt);
	if(mGain!=mTargetGain){
		scalar speed=Math::div(fdt,Math::fromMilli(mFadeTime));
		if(mGain<mTargetGain){
			mGain+=speed;
			if(mGain>mTargetGain){
				mGain=mTargetGain;
				mFadeTime=0;
			}
		}
		else{
			mGain-=speed;
			if(mGain<mTargetGain){
				mGain=mTargetGain;
				mFadeTime=0;
			}
		}

		setImmediateGain(mGain);
	}

	if(mAudioStream!=NULL){
		unsigned char buffer[bufferSize];
		ALenum format=ALPlayer::getALFormat(mAudioStream->getBitsPerSample(),mAudioStream->getChannels());
		int total=0;
		if(mStreamingBuffers==NULL){
			mStreamingBuffers=new unsigned int[numBuffers];
			alGenBuffers(numBuffers,mStreamingBuffers);
			TOADLET_CHECK_ALERROR("update::alGenBuffers");

			int i;
			for(i=0;i<numBuffers;++i){
				int amount=readAudioData(buffer,bufferSize);
				total=total+amount;
				alBufferData(mStreamingBuffers[i],format,buffer,amount,mAudioStream->getSamplesPerSecond());
				TOADLET_CHECK_ALERROR("update::alBufferData");
			}
			if(total==0){
				Logger::error(Categories::TOADLET_RIBBIT,
					"bad audio stream");
				return;
			}

			mTotalBuffersPlayed=numBuffers;

			alSourceQueueBuffers(mHandle,numBuffers,mStreamingBuffers);
			TOADLET_CHECK_ALERROR("update::alSourceQueueBuffers");

			alSourcePlay(mHandle);
			TOADLET_CHECK_ALERROR("update::alSourcePlay");
		}
		else{
			int processed=0;

			alGetSourcei(mHandle,AL_BUFFERS_PROCESSED,&processed);
			TOADLET_CHECK_ALERROR("update::alGetSourcei");

			if(mTotalBuffersPlayed>processed && processed>0){
				unsigned char buffer[bufferSize];

				int i;
				for(i=(mTotalBuffersPlayed-processed);i<mTotalBuffersPlayed;++i){
					unsigned int bufferID=mStreamingBuffers[i % numBuffers];
					
					alSourceUnqueueBuffers(mHandle,1,&bufferID);
					TOADLET_CHECK_ALERROR("update::alSourceUnqueueBuffers");

					int amount=readAudioData(buffer,bufferSize);
					total=total+amount;
					if(amount>0){
						alBufferData(bufferID,format,buffer,amount,mAudioStream->getSamplesPerSecond());
						TOADLET_CHECK_ALERROR("update::alBufferData");
						alSourceQueueBuffers(mHandle,1,&bufferID);
						TOADLET_CHECK_ALERROR("update::alSourceQueueBuffers");
					}
				}
			}

			mTotalBuffersPlayed+=processed;

			if(processed>=numBuffers){
				alSourcePlay(mHandle);
				TOADLET_CHECK_ALERROR("update::alSourcePlay");
			}
			else if(processed>0 && total==0){
				alSourceStop(mHandle);
				TOADLET_CHECK_ALERROR("update::alSourceStop");
				alDeleteBuffers(numBuffers,mStreamingBuffers);
				TOADLET_CHECK_ALERROR("update::alDeleteBuffers");

				delete[] mStreamingBuffers;
				mStreamingBuffers=NULL;

				mAudioStream=NULL;
			}
		}
	}
}

int ALAudio::readAudioData(tbyte *buffer,int bsize){
	int amount=mAudioStream->read(buffer,bsize);
	if(amount==0 && mLooping){
		mAudioStream->reset();
		amount=mAudioStream->read(buffer,bsize);
	}

	#if !defined(TOADLET_NATIVE_FORMAT)
		if(mAudioStream->getBitsPerSample()==16){
			int j=0;
			while(j<amount){
				littleInt16InPlace((int16&)buffer[j]);
				j+=2;
			}
		}
	#endif

	return amount;
}

void ALAudio::setImmediateGain(scalar gain){
	if(mAudioPlayer!=NULL){
		mGain=gain;
		if(alIsSource(mHandle)){
			alSourcef(mHandle,AL_GAIN,MathConversion::scalarToFloat(gain));
		}
	}
}

}
}
