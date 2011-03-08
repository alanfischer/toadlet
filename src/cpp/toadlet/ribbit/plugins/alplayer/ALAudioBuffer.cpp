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
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/ribbit/AudioFormatConversion.h>

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

bool ALAudioBuffer::create(AudioStream::ptr stream){
	tbyte *buffer=0;
	int length=0;
	AudioFormat::ptr format=stream->getAudioFormat();

	if(AudioFormatConversion::decode(stream,buffer,length)==false){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"unable to decode entire stream");
		return false;
	}

	int numFrames=length/format->frameSize();
	mLengthTime=numFrames*1000/format->samplesPerSecond;

	if(mAudioPlayer->getBufferFadeTime()>0){
		AudioFormatConversion::fade(buffer,length,format,mAudioPlayer->getBufferFadeTime());
	}

	ALenum alformat=ALPlayer::getALFormat(format->bitsPerSample,format->channels);

	alGenBuffers(1,&mHandle);
	if(mAudioPlayer->alBufferDataStatic!=NULL){
		mStaticData=buffer;
		mAudioPlayer->alBufferDataStatic(mHandle,alformat,mStaticData,length,format->samplesPerSecond);
	}
	else{
		alBufferData(mHandle,alformat,buffer,length,format->samplesPerSecond);
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
