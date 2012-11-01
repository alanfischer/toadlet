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

#include <toadlet/egg/Log.h>
#include <toadlet/egg/Error.h>
#include "OggVorbisDecoder.h"
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

namespace toadlet{
namespace tadpole{

size_t read_func(void *ptr,size_t size,size_t nmemb,void *datasource);
int seek_func(void *datasource,ogg_int64_t offset,int whence);
int close_func(void *datasource);
long tell_func(void *datasource);

OggVorbisDecoder::OggVorbisDecoder():
	mVorbisInfo(NULL),
	mDataLength(0)
{
	mVorbisFile=new OggVorbis_File();
	mFormat=AudioFormat::ptr(new AudioFormat());
}

OggVorbisDecoder::~OggVorbisDecoder(){
	stopStream();
	delete mVorbisFile;
}

void OggVorbisDecoder::close(){
	stopStream();
}

bool OggVorbisDecoder::startStream(Stream::ptr stream){
	mStream=stream;

	if(mStream==NULL){
		Error::nullPointer(Categories::TOADLET_RIBBIT,
			"Stream is NULL");
		return false;
	}

	if(mVorbisInfo!=NULL){
		Error::nullPointer(Categories::TOADLET_RIBBIT,
			"mVorbisInfo is NULL");
		return false;
	}

	ov_callbacks callbacks;

	callbacks.read_func=read_func;
	callbacks.seek_func=seek_func;
	callbacks.close_func=close_func;
	callbacks.tell_func=tell_func;

	int result;
	result=ov_open_callbacks(mStream,mVorbisFile,NULL,0,callbacks);

	if(result!=0){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"OggVorbisDecoder: ov_open_callbacks failed");
		return false;
	}

	mVorbisInfo=ov_info(mVorbisFile,-1);
	if(mVorbisInfo==NULL){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"OggVorbisDecoder: ov_info failed");
		return false;
	}

	mFormat=AudioFormat::ptr(new AudioFormat(16,mVorbisInfo->channels,mVorbisInfo->rate));

	return true;
}

int OggVorbisDecoder::read(tbyte *buffer,int length){
	int sec;
	int tempLength;
	int count=length;

	if(mVorbisInfo==NULL){
		return -1;
	}

	while(count>0){
		//If we have no data in buffer, copy over
		if(mDataLength==0){
			mDataLength=ov_read(mVorbisFile,mDataBuffer,OGGPACKETSIZE,0,2,1,&sec);
		}

		tempLength=mDataLength;

		if(count<mDataLength){
			tempLength=count;
		}

		memcpy(buffer+length-count,mDataBuffer,tempLength);
		mDataLength=mDataLength-tempLength;
		if(mDataLength){
			memmove(mDataBuffer,mDataBuffer+tempLength,mDataLength);
		}
		count=count-tempLength;

		if(tempLength==mDataLength){
			return tempLength;
		}
	}

	return length;
}

bool OggVorbisDecoder::stopStream(){
	if(mVorbisInfo!=NULL){
		ov_clear(mVorbisFile);
		mVorbisInfo=NULL;
	}

	mDataLength=0;

	return true;
}

bool OggVorbisDecoder::reset(){
	stopStream();
	bool b=mStream->reset();
	startStream(mStream);

	return b;
}

int OggVorbisDecoder::length(){
	return ov_raw_total(mVorbisFile,-1);
}

int OggVorbisDecoder::position(){
	return ov_raw_tell(mVorbisFile);
}

bool OggVorbisDecoder::seek(int offs){
	return ov_raw_seek(mVorbisFile,offs)>=0;
}

size_t read_func(void *ptr,size_t size,size_t nmemb, void *datasource){
	return ((Stream*)datasource)->read((tbyte*)ptr,size*nmemb);
}

int seek_func(void *datasource, ogg_int64_t offset, int whence){
	int length=((Stream*)datasource)->length();
	int position=((Stream*)datasource)->position();
	if(length>=0 && position>=0){
		((Stream*)datasource)->reset();
		int result=0;
		switch(whence){
			case SEEK_SET:
				result=((Stream*)datasource)->seek(offset);
			break;
			case SEEK_CUR:
				result=((Stream*)datasource)->seek(position+offset);
			break;
			case SEEK_END:
				result=((Stream*)datasource)->seek(length+offset);
			break;
		}
		return result<0?-1:0;
	}
	else{
		return -1;
	}
}

int close_func(void *datasource){
	((Stream*)datasource)->close();
	return 0;
};

long tell_func(void *datasource){
	return ((Stream*)datasource)->position();
}

}
}

