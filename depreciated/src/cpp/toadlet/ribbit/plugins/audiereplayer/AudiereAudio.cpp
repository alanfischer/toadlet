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

#include "AudierePlayer.h"
#include "AudiereAudioPeer.h"
#include "AudiereAudioBufferPeer.h"
#include <toadlet/ribbit/AudioBuffer.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace audiere;

namespace toadlet{
namespace ribbit{

AudiereAudioPeer::AudiereAudioPeer(AudierePlayer *player){
	mPlayer=player;
	mAudio=NULL;
	mLooping=false;
	mGain=1.0f;
}

void AudiereAudioPeer::setAudio(Audio *audio){
	mAudio=audio;
}

bool AudiereAudioPeer::loadAudioBuffer(AudioBuffer::ptr audioBuffer){
	mStream=mPlayer->makeStreamFromAudioBuffer(audioBuffer);
	return mStream!=NULL;
}

bool AudiereAudioPeer::loadAudioStream(InputStream::ptr in,const String &mimeType){
	mStream=mPlayer->makeStreamFromInputStream(in);
	return mStream!=NULL;
}

bool AudiereAudioPeer::play(){
	if(mStream){
		mStream->play();
		return true;
	}
	return false;
}

bool AudiereAudioPeer::stop(){
	if(mStream){
		mStream->stop();
		return true;
	}
	return false;
}

bool AudiereAudioPeer::getPlaying() const{
	if(mStream){
		return mStream->isPlaying();
	}
	else{
		return false;
	}
}

bool AudiereAudioPeer::getFinished() const{
	return false;
}

void AudiereAudioPeer::setGain(scalar gain){
	mGain=gain;
	if(mStream){
		mStream->setVolume(gain);
	}
}

// Current is the same as setGain
void AudiereAudioPeer::fadeToGain(scalar gain,int time){
	mGain=gain;
	if(mStream){
		mStream->setVolume(gain);
	}
}

scalar AudiereAudioPeer::getGain() const{
	if(mStream){
		return mStream->getVolume();
	}
	else{
		return mGain;
	}
}

scalar AudiereAudioPeer::getTargetGain() const{
	if(mStream){
		return mStream->getVolume();
	}
	else{
		return mGain;
	}
}

int AudiereAudioPeer::getFadeTime(){
	return 0;
}

void AudiereAudioPeer::setRolloffFactor(scalar f){
}

scalar AudiereAudioPeer::getRolloffFactor() const{
	return 0;
}

void AudiereAudioPeer::setLooping(bool looping){
	mLooping=looping;
	if(mStream){
		mStream->setRepeat(looping);
	}
}

bool AudiereAudioPeer::getLooping() const{
	if(mStream){
		return mStream->getRepeat();
	}
	else{
		return mLooping;
	}
}

void AudiereAudioPeer::setGlobal(bool global){
}

bool AudiereAudioPeer::getGlobal() const{
	return false;
}

void AudiereAudioPeer::setPitch(scalar pitch){
	if(mStream){
		mStream->setPitchShift(pitch);
	}
}

scalar AudiereAudioPeer::getPitch() const{
	if(mStream){
		return mStream->getPitchShift();
	}
	else{
		return 1.0f;
	}
}

void AudiereAudioPeer::setPosition(const Vector3 &position){
}

void AudiereAudioPeer::setVelocity(const Vector3 &velocity){
}

}
}
