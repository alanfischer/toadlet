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

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace audiere;

class AudiereFileStream:public audiere::File{
public:
	AudiereFileStream(InputStream::ptr in){
		mRefCount=0;

		mIn=in;
		mPosition=0;
		mSize=in->available();
	}

	ADR_METHOD(int) read(void *buffer,int size){
		int amt=mIn->read((char*)buffer,size);
		mPosition+=amt;
		return amt;
	}

	ADR_METHOD(bool) seek(int position,SeekMode mode){
		bool success=true;

		if(mode==BEGIN){
			success=mIn->reset();
			mPosition=0;
		}
		else if(mode==CURRENT && position<0){
			position+=mPosition;
			success=mIn->reset();
			mPosition=0;
		}
		else if(mode==END){
			if(mPosition>(mSize+position)){
				position=mSize+position;
				success=mIn->reset();
				mPosition=0;
			}
			else{
				position=(mSize+position)-mPosition;
			}
		}

		if(success==false){
			return false;
		}

		if(position>0){
			char *temp=new char[position];
			mPosition+=mIn->read(temp,position);
			delete[] temp;
		}
		else if(position<0){
			return false;
		}

		return true;
	}

	ADR_METHOD(int) tell(){
		return mPosition;
	}

	ADR_METHOD(void) ref(){
		mRefCount++;
	}

	ADR_METHOD(void) unref(){
		mRefCount--;
		if(mRefCount<=0){
			delete this;
		}
	}

protected:
	int mRefCount;

	InputStream::ptr mIn;
	int mPosition;
	int mSize;
};
typedef RefPtr<AudiereFileStream> AudiereFileStreamPtr;

namespace toadlet{
namespace ribbit{

TOADLET_C_API AudioPlayer* new_AudierePlayer(){
	return new AudierePlayer();
}

#if defined(TOADLET_BUILD_DYNAMIC)
	TOADLET_C_API AudioPlayer* new_AudioPlayer(){
		return new AudierePlayer();
	}
#endif

AudierePlayer::AudierePlayer(){
}

AudierePlayer::~AudierePlayer(){
}

bool AudierePlayer::startup(){
	TOADLET_LOG(TOADLET_RIBBIT_CATEGORY,Logger::Level_ALERT,
		"AudierePlayer: startup started");

	mDevice=OpenDevice();
	if(!mDevice){
		TOADLET_LOG(TOADLET_RIBBIT_CATEGORY,Logger::Level_ERROR,
			"Error opening audio device");
		return false;
	}

	mCapabilitySet.maxSources=16; // Just a guess
	mCapabilitySet.streaming=true;
	mCapabilitySet.positional=false;
	mCapabilitySet.mimeTypes.addElement("audio/x-wav");
	mCapabilitySet.mimeTypes.addElement("audio/mpeg");
	mCapabilitySet.mimeTypes.addElement("application/ogg");

	TOADLET_LOG(TOADLET_RIBBIT_CATEGORY,Logger::Level_ALERT,
		"AudierePlayer: startup completed");

	return true;
}

bool AudierePlayer::shutdown(){
	TOADLET_LOG(TOADLET_RIBBIT_CATEGORY,Logger::Level_ALERT,
		"AudierePlayer: shutdown started");
	TOADLET_LOG(TOADLET_RIBBIT_CATEGORY,Logger::Level_ALERT,
		"AudierePlayer: shutdown completed");

	return true;
}

AudioBufferPeer *AudierePlayer::createAudioBufferPeer(AudioBuffer *audioBuffer){
	AudiereAudioBufferPeer *peer=new AudiereAudioBufferPeer();
	SampleSourcePtr sampleSource=OpenSampleSource(new AudiereFileStream(audioBuffer->getInputStream()));
	peer->sampleBuffer=CreateSampleBuffer(sampleSource);
	if(!peer->sampleBuffer){
		delete peer;
		peer=NULL;
		Error::unknown(TOADLET_EGG_CATEGORY,
			"Not a seekable source");
	}
	return peer;
}

AudioPeer *AudierePlayer::createBufferedAudioPeer(Audio *audio,AudioBuffer::ptr buffer){
	AudiereAudioPeer *peer=new AudiereAudioPeer(this);
	peer->loadAudioBuffer(buffer);
	return peer;
}

AudioPeer *AudierePlayer::createStreamingAudioPeer(Audio *audio,InputStream::ptr in,const String &mimeType){
	AudiereAudioPeer *peer=new AudiereAudioPeer(this);
	peer->loadAudioStream(in,mimeType);
	return peer;
}

void AudierePlayer::suspend(){
}

void AudierePlayer::resume(){
}

void AudierePlayer::update(int dt){
	if(mDevice){
		mDevice->update();
	}
}

void AudierePlayer::setListenerTranslate(const Vector3 &translate){
}

void AudierePlayer::setListenerRotate(const Matrix3x3 &rotate){
}

void AudierePlayer::setListenerVelocity(const Vector3 &velocity){
}

void AudierePlayer::setListenerGain(scalar gain){
}

void AudierePlayer::setGroupGain(const String &group,scalar gain){
}

void AudierePlayer::removeGroup(const String &group){
}

scalar AudierePlayer::getGroupGain(const String &group){
	return 0;
}

void AudierePlayer::setDopplerFactor(scalar factor){
}

void AudierePlayer::setDopplerVelocity(scalar velocity){
}

void AudierePlayer::setDefaultRolloffFactor(scalar factor){
}

const CapabilitySet &AudierePlayer::getCapabilitySet(){
	return mCapabilitySet;
}

OutputStreamPtr AudierePlayer::makeStreamFromAudioBuffer(AudioBuffer *audioBuffer){
	AudiereAudioBufferPeer *peer=(AudiereAudioBufferPeer*)audioBuffer->internal_getAudioBufferPeer();
	return OpenSound(mDevice,peer->sampleBuffer->openStream(),false);
}

OutputStreamPtr AudierePlayer::makeStreamFromInputStream(InputStream::ptr in){
	return OpenSound(mDevice,new AudiereFileStream(in),true);
}

}
}

