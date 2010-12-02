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

#include "MMAudioBuffer.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/ribbit/AudioFormatConversion.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace ribbit{

MMAudioBuffer::MMAudioBuffer(MMPlayer *player):BaseResource(),
	mPlayer(NULL),
	mData(NULL),
	mLength(0)
{
	mPlayer=player;
}

MMAudioBuffer::~MMAudioBuffer(){
	destroy();
}


bool MMAudioBuffer::create(Stream::ptr stream,const String &mimeType){
	AudioStream::ptr audioStream=mPlayer->startAudioStream(stream,mimeType);
	if(audioStream==NULL){
		return false;
	}

	return create(audioStream);
}

bool MMAudioBuffer::create(AudioStream::ptr stream){
	int channels=stream->getAudioFormat().channels;
	int bps=stream->getAudioFormat().bitsPerSample;
	int sps=stream->getAudioFormat().samplesPerSecond;
	tbyte *buffer=NULL;
	int length=0;

	if(AudioFormatConversion::decode(stream,buffer,length)==false){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"unable to decode entire stream");
		return false;
	}

	int numsamps=length/channels/(bps/8);

	if(mPlayer->getBufferFadeTime()>0){
		AudioFormatConversion::fade(buffer,length,channels,bps,sps,mPlayer->getBufferFadeTime());
	}

	int nchannels=mPlayer->getAudioFormat().channels;
	int nbps=mPlayer->getAudioFormat().bitsPerSample;
	int nsps=mPlayer->getAudioFormat().samplesPerSecond;

	if(nchannels!=channels || nbps!=bps || nsps!=sps){
		Logger::debug(Categories::TOADLET_RIBBIT,String("converting audio from ")+channels+","+bps+","+sps+" to "+nchannels+","+nbps+","+nsps);

		int nlength=numsamps*nchannels*(nbps/8);
		tbyte *nbuffer=new tbyte[nlength];

		AudioFormatConversion::convert(buffer,channels,bps,sps,nbuffer,nchannels,nbps,nsps,length);

		delete[] buffer;

		mLength=nlength;
		mData=nbuffer;
	}
	else{
		mLength=length;
		mData=buffer;
	}

	return true;
}

void MMAudioBuffer::destroy(){
	if(mData!=NULL){
		delete[] mData;
		mData=NULL;
	}
}

}
}
