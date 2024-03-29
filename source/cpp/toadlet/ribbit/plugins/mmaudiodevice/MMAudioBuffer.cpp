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
#include <toadlet/ribbit/AudioFormatConversion.h>

namespace toadlet{
namespace ribbit{

MMAudioBuffer::MMAudioBuffer(MMAudioDevice *device):
	mDevice(NULL),
	mData(NULL),
	mLength(0)
{
	mDevice=device;
}

bool MMAudioBuffer::create(AudioStream *stream){
	AudioFormat::ptr format=stream->getAudioFormat();
	tbyte *buffer=NULL;
	int length=0;

	if(AudioFormatConversion::decode(stream,buffer,length)==false){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"unable to decode entire stream");
		return false;
	}

	if(mDevice->getBufferFadeTime()>0){
		AudioFormatConversion::fade(buffer,length,format,mDevice->getBufferFadeTime());
	}

	AudioFormat::ptr newFormat=mDevice->getAudioFormat();
	if(format!=newFormat){
		Log::debug(Categories::TOADLET_RIBBIT,String("converting audio from ")+
			format->getChannels()+","+format->getBitsPerSample()+","+format->getSamplesPerSecond()+" to "+
			newFormat->getChannels()+","+newFormat->getBitsPerSample()+","+newFormat->getSamplesPerSecond());

		int newLength=AudioFormatConversion::findConvertedLength(length,format,newFormat,true);
		tbyte *newBuffer=new tbyte[newLength];
		if(newBuffer==NULL){
			delete[] buffer;
			Error::unknown(Categories::TOADLET_RIBBIT,
				"unable to allocate buffer");
			return false;
		}

		AudioFormatConversion::convert(buffer,length,format,newBuffer,newLength,newFormat);

		format=newFormat;

		delete[] buffer;

		mLength=newLength;
		mData=newBuffer;
	}
	else{
		mLength=length;
		mData=buffer;
	}

	int numFrames=mLength*8/format->getBitsPerSample();
	mLengthTime=numFrames*1000/format->getSamplesPerSecond();

	return true;
}

void MMAudioBuffer::destroy(){
	if(mData!=NULL){
		delete[] mData;
		mData=NULL;
	}

	BaseResource::destroy();
}

}
}
