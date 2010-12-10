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

bool MMAudioBuffer::create(AudioStream::ptr stream){
	AudioFormat::ptr format=stream->getAudioFormat();
	tbyte *buffer=NULL;
	int length=0;

	if(AudioFormatConversion::decode(stream,buffer,length)==false){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"unable to decode entire stream");
		return false;
	}

	if(mPlayer->getBufferFadeTime()>0){
		AudioFormatConversion::fade(buffer,length,format,mPlayer->getBufferFadeTime());
	}

	AudioFormat::ptr newFormat=mPlayer->getAudioFormat();
	if(format!=newFormat){
		Logger::debug(Categories::TOADLET_RIBBIT,String("converting audio from ")+
			format->channels+","+format->bitsPerSample+","+format->samplesPerSecond+" to "+
			newFormat->channels+","+newFormat->bitsPerSample+","+newFormat->samplesPerSecond);

		if(format->samplesPerSecond!=newFormat->samplesPerSecond){
			Logger::warning(Categories::TOADLET_RIBBIT,
				"audio rate conversion not implemented, not converting rate");
		}

		int numFrames=length/format->frameSize();
		int newLength=numFrames*newFormat->frameSize();
		tbyte *newBuffer=new tbyte[newLength];

		AudioFormatConversion::convert(buffer,format,newBuffer,newFormat,length);

		delete[] buffer;

		mLength=newLength;
		mData=newBuffer;
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
