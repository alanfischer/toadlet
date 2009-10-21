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
#include "toadlet/egg/MathConversion.h"
#include <toadlet/ribbit/AudioBuffer.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace ribbit{

ALAudioPeer::ALAudioPeer(ALPlayer *player){
	mALPlayer=player;
	mSourceHandle=0;
	mStreamingBuffers=NULL;
	mLooping=false;
	mGlobal=false;
	mTargetGain=Math::ONE;
	mGain=Math::ONE;
	mFadeTime=0;
}

ALAudioPeer::~ALAudioPeer(){
	if(mALPlayer!=NULL){
		mALPlayer->internal_audioPeerDestroy(this);
	}
}

bool ALAudioPeer::loadAudioBuffer(AudioBuffer::ptr audioBuffer){
	mStream=NULL;

	if(audioBuffer==NULL){
		return false;
	}

	ALAudioBufferPeer *alAudioBufferPeer=(ALAudioBufferPeer*)audioBuffer->internal_getAudioBufferPeer();

	if(alAudioBufferPeer!=NULL && alIsSource(mSourceHandle)){
		alSourceStop(mSourceHandle);
		TOADLET_CHECK_ALERROR("loadAudioBuffer::alSourceStop");

		alSourcei(mSourceHandle,AL_BUFFER,alAudioBufferPeer->bufferHandle);
		TOADLET_CHECK_ALERROR("loadAudioBuffer::alSourcei");

		return true;
 	}
	return false;
}

bool ALAudioPeer::loadAudioStream(AudioStream::ptr stream){
	mStream=stream;

	if(stream==NULL){
		return false;
	}

	if(mALPlayer!=NULL){
		mALPlayer->update(this,0);

		if(alIsSource(mSourceHandle)){
			alSourceStop(mSourceHandle);
			TOADLET_CHECK_ALERROR("loadAudioStream::alSourceStop");
		}

		mALPlayer->unlock();

		return true;
	}
	return false;
}

bool ALAudioPeer::play(){
	alSourcePlay(mSourceHandle);
	TOADLET_CHECK_ALERROR("playAudioBuffer:;alSourcePlay");

	return true;
}

bool ALAudioPeer::stop(){
	alSourceStop(mSourceHandle);
	TOADLET_CHECK_ALERROR("playAudioStream::alSourcePlay");

	return true;
}

bool ALAudioPeer::getPlaying() const{
	int state=0;
	if(alIsSource(mSourceHandle)){
		alGetSourcei(mSourceHandle,AL_SOURCE_STATE,&state);
	}
	return state==AL_PLAYING;
}

void ALAudioPeer::setGain(scalar gain){
	if(mALPlayer!=NULL){
		mALPlayer->lock();

		internal_setGain(gain);
		mTargetGain=gain;
		mFadeTime=0;

		mALPlayer->unlock();
	}
}

void ALAudioPeer::fadeToGain(scalar gain,int time){
	if(mALPlayer!=NULL){
		mALPlayer->lock();

		mTargetGain=gain;
		mFadeTime=time;

		mALPlayer->unlock();
	}
}

scalar ALAudioPeer::getGain() const{
	return mGain;
}

scalar ALAudioPeer::getTargetGain() const{
	return mTargetGain;
}

int ALAudioPeer::getFadeTime(){
	return mFadeTime;
}

void ALAudioPeer::setRolloffFactor(scalar factor){
	if(alIsSource(mSourceHandle)){
		alSourcef(mSourceHandle,AL_ROLLOFF_FACTOR,MathConversion::scalarToFloat(factor));
	}
}

scalar ALAudioPeer::getRolloffFactor() const{
	float r=0;
	if(alIsSource(mSourceHandle)){
		alGetSourcef(mSourceHandle,AL_ROLLOFF_FACTOR,&r);
	}
	return MathConversion::floatToScalar(r);
}

void ALAudioPeer::setLooping(bool looping){
	mLooping=looping;
	if(mStream==0 && alIsSource(mSourceHandle)){
		alSourcei(mSourceHandle,AL_LOOPING,looping);
	}
}

bool ALAudioPeer::getLooping() const{
	int looping=0;
	if(alIsSource(mSourceHandle)){
		alGetSourcei(mSourceHandle,AL_LOOPING,&looping);
	}

	if(looping>0){
		return true;
	}
	else{
		return mLooping;
	}
}

void ALAudioPeer::setGlobal(bool global){
	mGlobal=global;
	if(alIsSource(mSourceHandle)){
		if(mGlobal){
			ALfloat zero[]={0,0,0};
			alSourcefv(mSourceHandle,AL_POSITION,zero);
			alSourcei(mSourceHandle,AL_SOURCE_RELATIVE,true);
		}
		else{
			alSourcei(mSourceHandle,AL_SOURCE_RELATIVE,false);
		}
	}
}

bool ALAudioPeer::getGlobal() const{
	return mGlobal;
}

void ALAudioPeer::setPitch(scalar pitch){
	if(alIsSource(mSourceHandle)){
		alSourcef(mSourceHandle,AL_PITCH,MathConversion::scalarToFloat(pitch));
	}
}

scalar ALAudioPeer::getPitch() const{
	float pitch=0;
	if(alIsSource(mSourceHandle)){
		alGetSourcef(mSourceHandle,AL_PITCH,&pitch);
	}
	return MathConversion::floatToScalar(pitch);
}

void ALAudioPeer::setGroup(const String &group){
	mGroup=group;
	setGain(mTargetGain);
}

const String &ALAudioPeer::getGroup() const{
	return mGroup;
}

void ALAudioPeer::setPosition(const Vector3 &position){
	if(alIsSource(mSourceHandle)){
		if(mGlobal){
			MathConversion::scalarToFloat(mALPlayer->cacheVector3,Math::ZERO_VECTOR3);
		}
		else{
			MathConversion::scalarToFloat(mALPlayer->cacheVector3,position);
		}
		alSourcefv(mSourceHandle,AL_POSITION,(ALfloat*)mALPlayer->cacheVector3.getData());
	}
}

void ALAudioPeer::setVelocity(const Vector3 &velocity){
	// Since doppler doesn't work on most *nix OpenALs
	if(alIsSource(mSourceHandle)){
		if(mGlobal){
			MathConversion::scalarToFloat(mALPlayer->cacheVector3,Math::ZERO_VECTOR3);
		}
		else{
			MathConversion::scalarToFloat(mALPlayer->cacheVector3,velocity);
		}
		alSourcefv(mSourceHandle,AL_VELOCITY,(ALfloat*)mALPlayer->cacheVector3.getData());
	}
}

bool ALAudioPeer::getFinished() const{
	// First check the saved looping, in case the source is invalid
	// Also never stop until our stream is depleated
	if(mLooping==true || mStream!=NULL){
		return false;
	}

	int state=0;
	if(alIsSource(mSourceHandle)){
		alGetSourcei(mSourceHandle,AL_SOURCE_STATE,&state);
	}
	if(state!=AL_PLAYING && state!=AL_PAUSED){
		return true;
	}
	return false;
}

void ALAudioPeer::setSourceHandle(int handle){
	mSourceHandle=handle;
	if(alIsSource(mSourceHandle)){
//		alSourcei(mSourceHandle,AL_LOOPING,0);
//		alSourcef(mSourceHandle,AL_PITCH,1);
//		alSourcef(mSourceHandle,AL_GAIN,1);
//		alSourcefv(mSourceHandle,AL_POSITION,(ALfloat*)scalarToReal(Math::ZERO_VECTOR3).getData());
//		alSourcef(mSourceHandle,AL_ROLLOFF_FACTOR,scalarToReal(mALPlayer->getDefaultRolloffFactor()));
//		alSourcei(mSourceHandle,AL_SOURCE_RELATIVE,false);
		setVelocity(Math::ZERO_VECTOR3);
	}
	
	TOADLET_CHECK_ALERROR("setSourceHandle");
}

int ALAudioPeer::getSourceHandle(){
	return mSourceHandle;
}

void ALAudioPeer::setStream(AudioStream::ptr stream){
	mStream=stream;
}

AudioStream::ptr ALAudioPeer::getStream(){
	return mStream;
}

void ALAudioPeer::setStreamingBuffers(unsigned int *buffers){
	mStreamingBuffers=buffers;
}

unsigned int *ALAudioPeer::getStreamingBuffers(){
	return mStreamingBuffers;
}

void ALAudioPeer::setTotalBuffersPlayed(int buffersPlayed){
	mTotalBuffersPlayed=buffersPlayed;
}

int ALAudioPeer::getTotalBuffersPlayed(){
	return mTotalBuffersPlayed;
}

void ALAudioPeer::internal_setGain(scalar gain){
	if(mALPlayer!=NULL){
		scalar groupGain=mALPlayer->internal_getGroupGain(mGroup);

		mGain=gain;
		if(alIsSource(mSourceHandle)){
			gain=Math::mul(gain,groupGain);
//			alSourcef(mSourceHandle,AL_GAIN,scalarToReal(gain));
		}
	}
}

void ALAudioPeer::internal_playerShutdown(){
	mALPlayer=NULL;
}

}
}
