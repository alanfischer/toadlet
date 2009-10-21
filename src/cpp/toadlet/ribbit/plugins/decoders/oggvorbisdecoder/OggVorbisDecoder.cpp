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

#include "OggVorbisDecoder.h"
#include <toadlet/egg/Error.h>
#include <string.h> // memcpy

#if defined(TOADLET_PLATFORM_WIN32)
	#if defined(TOADLET_LIBOGG_NAME)
		#pragma comment(lib,TOADLET_LIBOGG_NAME)
	#endif
	#if defined(TOADLET_LIBVORBISFILE_NAME)
		#pragma comment(lib,TOADLET_LIBVORBISFILE_NAME)
	#endif
	#if defined(TOADLET_LIBVORBIS_NAME)
		#pragma comment(lib,TOADLET_LIBVORBIS_NAME)
	#endif
#endif

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace ribbit{

AudioStream *new_OggVorbisDecoder(){
	return new OggVorbisDecoder();
}

OggVorbisDecoder::OggVorbisDecoder(){
	mVorbisInfo=NULL;
	mDataLength=0;
}

OggVorbisDecoder::~OggVorbisDecoder(){
	stopStream();
}

size_t OggVorbisDecoder::read_func(void *ptr,size_t size,size_t nmemb, void *datasource){
	return ((egg::io::InputStream*)datasource)->read((char*)ptr,size*nmemb);
}

int OggVorbisDecoder::seek_func(void *datasource, ogg_int64_t offset, int whence){
	return -1;
}

int OggVorbisDecoder::close_func(void *datasource){
	return 0;
};

long OggVorbisDecoder::tell_func(void *datasource){
	return 0;
}

bool OggVorbisDecoder::startStream(egg::io::InputStream::ptr istream){
	mIn=istream;

	if(mIn==NULL){
		Error::nullPointer(Categories::TOADLET_RIBBIT,
			"InputStream is NULL");
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
	result=ov_open_callbacks(mIn,&mVorbisFile,NULL,0,callbacks);

	if(result!=0){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"OggVorbisDecoder: ov_open_callbacks failed");
		return false;
	}

	mVorbisInfo=ov_info(&mVorbisFile,-1);
	if(mVorbisInfo==NULL){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"OggVorbisDecoder: ov_info failed");
		return false;
	}

	return true;
}

int OggVorbisDecoder::read(char *buffer,int length){
	int sec;
	int tempLength;
	int count=length;

	if(mVorbisInfo==NULL){
		return 0;
	}

	while(count>0){
		//If we have no data in buffer, copy over
		if(mDataLength==0){
			mDataLength=ov_read(&mVorbisFile,mDataBuffer,OGGPACKETSIZE,0,2,1,&sec);
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
		ov_clear(&mVorbisFile);
		mVorbisInfo=NULL;
	}

	mDataLength=0;

	return true;
}

bool OggVorbisDecoder::reset(){
	stopStream();
	bool b=mIn->reset();
	startStream(mIn);

	return b;
}

bool OggVorbisDecoder::seek(int offs){
	return false;
}

int OggVorbisDecoder::available(){
	return 0;
}

void OggVorbisDecoder::close(){
	stopStream();
}

int OggVorbisDecoder::getChannels(){
	if(mVorbisInfo!=NULL){
		return mVorbisInfo->channels;
	}
	
	return 0;
}

int OggVorbisDecoder::getSamplesPerSecond(){
	if(mVorbisInfo!=NULL){
		return mVorbisInfo->rate;
	}

	return 0;
}

int OggVorbisDecoder::getBitsPerSample(){
	return 16;
}

InputStream::ptr OggVorbisDecoder::getParentStream(){
	return mIn;
}

}
}

