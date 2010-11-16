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

#include "ALAudioBuffer.h"
#include "ALPlayer.h"
#include <toadlet/ribbit/AudioFormatConversion.h>

#if defined(TOADLET_PLATFORM_OSX)
	#include <toadlet/egg/Error.h>
	#include "../decoders/coreaudiodecoder/CoreAudioDecoder.h"
	#include "platform/osx/CoreAudio.h"
#endif

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace ribbit{

ALAudioBuffer::ALAudioBuffer(ALPlayer *player):BaseResource(),
	mAudioPlayer(NULL),
	mHandle(0),
	mStaticData(NULL),
	mLengthTime(0)
{
	mAudioPlayer=player;
}

ALAudioBuffer::~ALAudioBuffer(){
	destroy();
}

bool ALAudioBuffer::create(Stream::ptr stream,const String &mimeType){
	destroy();

	AudioStream::ptr audioStream=mAudioPlayer->startAudioStream(stream,mimeType);
	if(audioStream==NULL){
		return false;
	}

	#if defined(TOADLET_PLATFORM_OSX)
		if(((CoreAudioDecoder*)audioStream.get())->isVariableBitRate()){
			Error::unknown(Categories::TOADLET_RIBBIT,
				"Variable bit rate streams not supported as Audio Buffers");
			return false;
		}
	#endif

	return create(audioStream);
}

bool ALAudioBuffer::create(AudioStream::ptr stream){
	tbyte *buffer=0;
	int length=0;
	int channels=stream->getChannels();
	int sps=stream->getSamplesPerSecond();
	int bps=stream->getBitsPerSample();

	AudioFormatConversion::decode(stream,buffer,length);
	int numsamps=length/channels/(bps/8);
	mLengthTime=numsamps*1000/sps;

	if(mAudioPlayer->getBufferFadeTime()>0){
		AudioFormatConversion::fade(buffer,length,channels,sps,bps,mAudioPlayer->getBufferFadeTime());
	}

	ALenum format=ALPlayer::getALFormat(bps,channels);

	alGenBuffers(1,&mHandle);
	if(mAudioPlayer->alBufferDataStatic!=NULL){
		mStaticData=buffer;
		mAudioPlayer->alBufferDataStatic(mHandle,format,mStaticData,length,sps);
	}
	else{
		alBufferData(mHandle,format,buffer,length,sps);
		delete[] buffer;
	}

	TOADLET_CHECK_ALERROR("ALAudioBuffer::create");

	return true;
}

void ALAudioBuffer::destroy(){
	if(mHandle!=0){
		alDeleteBuffers(1,&mHandle);
		mHandle=0;
	}

	if(mStaticData!=NULL){
		delete[] mStaticData;
		mStaticData=NULL;
	}

	TOADLET_CHECK_ALERROR("alDeleteBuffers::destroy");
}

}
}
