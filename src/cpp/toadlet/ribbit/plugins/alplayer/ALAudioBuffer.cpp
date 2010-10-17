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
	mStaticData(NULL)
{
	mAudioPlayer=player;
}

ALAudioBuffer::~ALAudioBuffer(){
	destroy();
}

bool ALAudioBuffer::create(Stream::ptr stream,const String &mimeType){
	destroy();

	AudioStream::ptr decoder=mAudioPlayer->startAudioStream(stream,mimeType);
	if(decoder==NULL){
		return false;
	}

	#if defined(TOADLET_PLATFORM_OSX)
		if(((CoreAudioDecoder*)decoder.get())->isVariableBitRate()){
			Error::unknown(Categories::TOADLET_RIBBIT,
				"Variable bit rate streams not supported as Audio Buffers");
			return false;
		}
	#endif

	tbyte *buffer=0;
	int length=0;
	int channels=decoder->getChannels();
	int sps=decoder->getSamplesPerSecond();
	int bps=decoder->getBitsPerSample();

	mAudioPlayer->decodeStream(decoder,buffer,length);
	int numsamps=length/channels/(bps/8);

	// Lets us programatically reduce popping on some platforms
	if(mAudioPlayer->getBufferFadeTime()>0){
		int stf=sps*mAudioPlayer->getBufferFadeTime()/1000;
		if(stf>numsamps){stf=numsamps;}
		int sampsize=channels*(bps/8);
		int i,j;
		for(i=0;i<stf;++i){
			// Fade front
			for(j=0;j<sampsize;++j){
				buffer[i*sampsize+j]=(tbyte)(((int)buffer[i*sampsize+j])*i/stf);
			}
			// Fade back
			for(j=0;j<sampsize;++j){
				buffer[(numsamps-i-1)*sampsize+j]=(tbyte)(((int)buffer[(numsamps-i-1)*sampsize+j])*i/stf);
			}
		}
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
