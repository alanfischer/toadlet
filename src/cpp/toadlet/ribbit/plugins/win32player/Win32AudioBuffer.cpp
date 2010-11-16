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

#include "Win32AudioBuffer.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/ribbit/AudioFormatConversion.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace ribbit{

Win32AudioBuffer::Win32AudioBuffer(Win32Player *player):BaseResource(),
	mPlayer(NULL),
	mData(NULL),
	mLength(0)
{
	mPlayer=player;
}

Win32AudioBuffer::~Win32AudioBuffer(){
	destroy();
}


bool Win32AudioBuffer::create(Stream::ptr stream,const String &mimeType){
	AudioStream::ptr audioStream=mPlayer->startAudioStream(stream,mimeType);
	if(audioStream==NULL){
		return false;
	}

	return create(audioStream);
}

bool Win32AudioBuffer::create(AudioStream::ptr stream){
	int channels=stream->getChannels();
	int bps=stream->getBitsPerSample();
	int sps=stream->getSamplesPerSecond();
	tbyte *buffer=NULL;
	int length=0;

	if(AudioFormatConversion::decode(stream,buffer,length)==false){
		return false;
	}

	int numsamps=length/channels/(bps/8);

	if(mPlayer->getBufferFadeTime()>0){
		AudioFormatConversion::fade(buffer,length,channels,bps,sps,mPlayer->getBufferFadeTime());
	}

	int nchannels=mPlayer->getChannels();	
	int nbps=mPlayer->getBitsPerSample();
	int nsps=mPlayer->getSamplesPerSecond();

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

void Win32AudioBuffer::destroy(){
	if(mData!=NULL){
		delete[] mData;
		mData=NULL;
	}
}

}
}
