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

#include "CoreAudioDecoder.h"
#include <toadlet/egg/Error.h>
#include <memory.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace ribbit{

AudioStream *new_CoreAudioDecoder(){
	return new CoreAudioDecoder();
}

CoreAudioDecoder::CoreAudioDecoder(){
	mPosition=0;
	mSourceSize=0;
	mSourcePosition=0;
	mAudioFile=0;
}

CoreAudioDecoder::~CoreAudioDecoder(){
	close();
}

void CoreAudioDecoder::close(){
	if(mAudioFile!=0){
		AudioFileClose(mAudioFile);
		mAudioFile=0;
	}

	if(mIn!=NULL){
		mIn->close();
	}
}

bool CoreAudioDecoder::startStream(Stream::ptr in){
	UInt32 propertySize=0;
	OSStatus result=0;

	mIn=in;
	mSourceSize=in->length();
	mSourcePosition=0;

	result=AudioFileOpenWithCallbacks(this,audioFileRead,0,audioFileGetSize,0,kAudioFileWAVEType,&mAudioFile);
	if(result!=0){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"Error in AudioFileOpenWithCallbacks");
		return false;
	}

	propertySize=sizeof(mStreamDescription);
	result=AudioFileGetProperty(mAudioFile,kAudioFilePropertyDataFormat,&propertySize,&mStreamDescription);
	if(result!=0){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"Error in AudioFileGetProperty");
		return false;
	}

	return true;
}

int CoreAudioDecoder::read(tbyte *buffer,int length){
	AudioFileReadBytes(mAudioFile,false,mPosition,(UInt32*)&length,buffer);
	mPosition+=length;
	return length;
}

bool CoreAudioDecoder::reset(){
	mPosition=0;
	return true;
}

bool CoreAudioDecoder::isVariableBitRate() const{
	return mStreamDescription.mBytesPerPacket==0 || mStreamDescription.mFramesPerPacket==0;
}

OSStatus CoreAudioDecoder::audioFileRead(void *inRefCon,SInt64 inPosition,UInt32 requestCount,void *buffer,UInt32 *actualCount){
	CoreAudioDecoder *decoder=(CoreAudioDecoder*)inRefCon;

	if(inPosition!=decoder->mSourcePosition){
		decoder->mIn->reset();
		decoder->mIn->seek(inPosition);
		decoder->mSourcePosition=inPosition;
	}

	*actualCount=decoder->mIn->read((tbyte*)buffer,requestCount);

	decoder->mSourcePosition+=*actualCount;

	return noErr;
}

SInt64 CoreAudioDecoder::audioFileGetSize(void *inRefCon){
	CoreAudioDecoder *decoder=(CoreAudioDecoder*)inRefCon;
	return decoder->mSourceSize;
}

}
}

