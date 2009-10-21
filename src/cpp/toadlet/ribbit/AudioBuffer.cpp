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

#include <toadlet/ribbit/AudioBuffer.h>
#include <toadlet/ribbit/AudioPlayer.h>

#include <stdio.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace ribbit{

AudioBuffer::AudioBuffer():
	//mIn,
	//mMimeType,

	mAudioBufferPeer(NULL),
	mOwnsAudioBufferPeer(false)
{
}

AudioBuffer::AudioBuffer(AudioPlayer *player,InputStream::ptr in,const String &mimeType):
	//mIn,
	//mMimeType,

	mAudioBufferPeer(NULL),
	mOwnsAudioBufferPeer(false)
{
	mIn=in;
	mMimeType=mimeType;

	if(player->isStarted()){
		mAudioBufferPeer=player->createAudioBufferPeer(this);
	}
	mOwnsAudioBufferPeer=true;
}

AudioBuffer::~AudioBuffer(){
	internal_setAudioBufferPeer(NULL,false);
}

void AudioBuffer::setStream(InputStream::ptr in,const String &mimeType){
	mIn=in;
	mMimeType=mimeType;
}

InputStream::ptr AudioBuffer::getInputStream() const{
	return mIn;
}

const egg::String &AudioBuffer::getMimeType() const{
	return mMimeType;
}

void AudioBuffer::internal_setAudioBufferPeer(AudioBufferPeer *audioBufferPeer,bool owns){
	if(mOwnsAudioBufferPeer && mAudioBufferPeer!=NULL){
		delete mAudioBufferPeer;
	}
	mAudioBufferPeer=audioBufferPeer;
	mOwnsAudioBufferPeer=owns;
}

}
}

