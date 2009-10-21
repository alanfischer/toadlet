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

#include <toadlet/ribbit/Audio.h>
#include <toadlet/ribbit/AudioPlayer.h>

using namespace toadlet::egg;

namespace toadlet{
namespace ribbit{

String Audio::EMPTY_STRING;

Audio::Audio(){
	mAudioPlayer=NULL;
	mLooping=false;

	mAudioPeer=NULL;
}

Audio::Audio(AudioPlayer *player){
	mAudioPlayer=player;
	mLooping=false;

	mAudioPeer=NULL;
}

Audio::~Audio(){
	internal_destroyAudioPeer();
}

bool Audio::loadAudioBuffer(const AudioBuffer::ptr &audioBuffer){
	internal_destroyAudioPeer();

	mAudioBuffer=audioBuffer;
	if(mAudioPlayer!=NULL && mAudioPlayer->isStarted()){
		mAudioPeer=mAudioPlayer->createBufferedAudioPeer(this,audioBuffer);
	}

	return mAudioPeer!=NULL;
}

bool Audio::loadAudioStream(egg::io::InputStream::ptr in,const egg::String &extension){
	internal_destroyAudioPeer();

	mAudioBuffer=NULL;
	if(mAudioPlayer!=NULL && mAudioPlayer->isStarted()){
		mAudioPeer=mAudioPlayer->createStreamingAudioPeer(this,in,extension);
	}

	return mAudioPeer!=NULL;
}

AudioBuffer::ptr Audio::getAudioBuffer() const{
	return mAudioBuffer;
}

bool Audio::play(){
	if(mAudioPeer!=NULL){
		return mAudioPeer->play();
	}
	return false;
}

bool Audio::stop(){
	if(mAudioPeer!=NULL){
		return mAudioPeer->stop();
	}
	return false;
}

bool Audio::getPlaying() const{
	if(mAudioPeer!=NULL){
		return mAudioPeer->getPlaying();
	}
	return false;
}

void Audio::setGain(scalar gain){
	if(mAudioPeer!=NULL){
		mAudioPeer->setGain(gain);
	}
}

void Audio::fadeToGain(scalar gain,int time){
	if(mAudioPeer!=NULL){
		mAudioPeer->fadeToGain(gain,time);
	}
}

scalar Audio::getGain() const{
	if(mAudioPeer!=NULL){
		return mAudioPeer->getGain();
	}
	return 0;
}

void Audio::setLooping(bool looping){
	if(mAudioPeer!=NULL){
		mAudioPeer->setLooping(looping);
	}
	else{
		mLooping=looping;
	}
}

bool Audio::getLooping() const{
	if(mAudioPeer!=NULL){
		return mAudioPeer->getLooping();
	}
	return mLooping;
}

void Audio::setPitch(scalar pitch){
	if(mAudioPeer!=NULL){
		mAudioPeer->setPitch(pitch);
	}
}

scalar Audio::getPitch() const{
	if(mAudioPeer!=NULL){
		return mAudioPeer->getPitch();
	}
	return 0;
}

void Audio::setGlobal(bool global){
	if(mAudioPeer!=NULL){
		mAudioPeer->setGlobal(global);
	}
}

bool Audio::getGlobal() const{
	if(mAudioPeer!=NULL){
		return mAudioPeer->getGlobal();
	}
	return false;
}

void Audio::setGroup(const String &name){
	if(mAudioPeer!=NULL){
		mAudioPeer->setGroup(name);
	}
}

const String &Audio::getGroup() const{
	if(mAudioPeer){
		return mAudioPeer->getGroup();
	}
	return EMPTY_STRING;
}

void Audio::setPosition(const Vector3 &position){
	if(mAudioPeer!=NULL){
		mAudioPeer->setPosition(position);
	}
}

void Audio::setRolloffFactor(scalar f){
	if(mAudioPeer!=NULL){
		mAudioPeer->setRolloffFactor(f);
	}
}

scalar Audio::getRolloffFactor() const{
	if(mAudioPeer!=NULL){
		return mAudioPeer->getRolloffFactor();
	}
	return 0;
}

bool Audio::getFinished() const{
	if(mAudioPeer!=NULL){
		return mAudioPeer->getFinished();
	}
	return !mLooping;
}

void Audio::setVelocity(const Vector3 &velocity){
	if(mAudioPeer!=NULL){
		mAudioPeer->setVelocity(velocity);
	}
}

void Audio::internal_setAudioPlayer(AudioPlayer *player){
	internal_destroyAudioPeer();

	mAudioPlayer=player;
}

void Audio::internal_destroyAudioPeer(){
	if(mAudioPeer!=NULL){
		delete mAudioPeer;
		mAudioPeer=NULL;
	}
}

}
}

