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
#include "ALAudioDevice.h"
#include <toadlet/ribbit/AudioFormatConversion.h>

namespace toadlet{
namespace ribbit{

ALAudioBuffer::ALAudioBuffer(ALAudioDevice *device):
	mDevice(NULL),
	mHandle(0),
	mStaticData(NULL),
	mLengthTime(0)
{
	mDevice=device;
}

bool ALAudioBuffer::create(AudioStream *stream){
	tbyte *buffer=0;
	int length=0;
	AudioFormat::ptr format=stream->getAudioFormat();

	if(AudioFormatConversion::decode(stream,buffer,length)==false){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"unable to decode entire stream");
		return false;
	}

	int numFrames=length/format->getFrameSize();
	mLengthTime=numFrames*1000/format->getSamplesPerSecond();

	if(mDevice->getBufferFadeTime()>0){
		AudioFormatConversion::fade(buffer,length,format,mDevice->getBufferFadeTime());
	}

	alGenBuffers(1,&mHandle);
	if(mDevice->alBufferDataStatic!=NULL){
		mStaticData=buffer;
		mDevice->alBufferDataStatic(mHandle,
			ALAudioDevice::getALFormat(format),
			mStaticData,length,format->getSamplesPerSecond());
	}
	else{
		alBufferData(mHandle,
			ALAudioDevice::getALFormat(format),
			buffer,length,format->getSamplesPerSecond());
		delete[] buffer;
	}

	TOADLET_CHECK_ALERROR("ALAudioBuffer::create");

	return true;
}

void ALAudioBuffer::destroy(){
	if(mHandle!=0){
		if(mDevice!=NULL){
			mDevice->preDestroyBuffer(this);
		}

		alDeleteBuffers(1,&mHandle);
		mHandle=0;

		TOADLET_CHECK_ALERROR("alDeleteBuffers::destroy");
	}

	if(mStaticData!=NULL){
		delete[] mStaticData;
		mStaticData=NULL;
	}

	BaseResource::destroy();
}

}
}
