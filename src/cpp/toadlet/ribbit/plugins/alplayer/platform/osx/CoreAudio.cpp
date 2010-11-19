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

#include <toadlet/egg/Error.h>
#include <toadlet/egg/MathConversion.h>
#include "../../ALPlayer.h"
#include "CoreAudio.h"

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace ribbit{

const static int numBuffers=3;

CoreAudio::CoreAudio(ALPlayer *audioPlayer):
	mLooping(false),
	mAudioPlayer(NULL),
	//mAudioStream,
	mTargetGain(0),
	mGain(0),
	mFadeTime(0),

	mAudioQueue(0),
	//mBuffers[numBuffers],
	//mBuffersToDispose,
	mNumPacketsToRead(0),
	mCurrentPacket(0),
	mPacketDescs(NULL),
	mFileDataSize(0)
{
	mAudioPlayer=audioPlayer;

	setGain(Math::ONE);
}

CoreAudio::~CoreAudio(){
	destroy();
}

bool CoreAudio::create(AudioBuffer::ptr buffer){
	mAudioPlayer->registerCoreAudio(this);
	
	return false;
}
	
bool CoreAudio::create(Stream::ptr stream,const String &mimeType){
	AudioStream::ptr audioStream=mAudioPlayer->startAudioStream(stream,mimeType);
	if(audioStream==NULL){
		return false;
	}

	return create(audioStream);
}

bool CoreAudio::create(AudioStream::ptr stream){
	mStream=shared_static_cast<CoreAudioDecoder>(stream);

	setupQueue();

	setupBuffers();

	mAudioPlayer->registerCoreAudio(this);
	
	return true;
}

void CoreAudio::destroy(){
	if(mAudioPlayer!=NULL){
		mAudioPlayer->unregisterCoreAudio(this);
	}

	if(mAudioQueue!=NULL){
		AudioQueueDispose(mAudioQueue,true);
		mAudioQueue=NULL;
	}
}

bool CoreAudio::play(){
	OSStatus result=AudioQueuePrime(mAudioQueue,1,NULL);
	if(result!=0){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"Error in AudioQueuePrime");
		return false;
	}

	result=AudioQueueStart(mAudioQueue,NULL)!=0;
	if(result!=0){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"Error in AudioQueueStart");
		return false;
	}

	return true;
}

bool CoreAudio::stop(){
	OSStatus result=AudioQueueStop(mAudioQueue,true);
	if(result!=0){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"Error in AudioQueueStop");
		return false;
	}

	return true;
}

bool CoreAudio::getPlaying() const{
	UInt32 isRunning=0;
	OSStatus result=AudioQueueGetParameter(mAudioQueue,kAudioQueueProperty_IsRunning,(AudioQueueParameterValue*)&isRunning);
	if(result!=0){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"Error in AudioQueueGetParameter");
		return false;
	}

	return isRunning!=0;
}

bool CoreAudio::getFinished() const{
	return false; /// @todo FINISH ME
}

void CoreAudio::setGain(scalar gain){
	if(mAudioPlayer!=NULL){
		mAudioPlayer->lock();

		setImmediateGain(gain);
		mTargetGain=gain;
		mFadeTime=0;

		mAudioPlayer->unlock();
	}
}

void CoreAudio::fadeToGain(scalar gain,int time){
	if(mAudioPlayer!=NULL){
		mAudioPlayer->lock();

		mTargetGain=gain;
		mFadeTime=time;

		mAudioPlayer->unlock();
	}
}

scalar CoreAudio::getGain() const{
	return mGain;
}

void CoreAudio::setLooping(bool looping){
	mLooping=looping;
}

void CoreAudio::update(int dt){
	scalar fdt=Math::fromMilli(dt);
	if(mGain!=mTargetGain){
		scalar speed=Math::mul(mFadeTime,fdt);
		if(mGain<mTargetGain){
			mGain+=mFadeTime;
			if(mGain>mTargetGain){
				mGain=mTargetGain;
			}
		}
		else{
			mGain-=speed;
			if(mGain<mTargetGain){
				mGain=mTargetGain;
			}
		}

		setImmediateGain(mGain);
	}
}

void CoreAudio::setImmediateGain(scalar gain){
	mGain=gain;

	AudioQueueSetParameter(mAudioQueue,kAudioQueueParam_Volume,MathConversion::scalarToFloat(gain));
}

void CoreAudio::calculateBytesForTime(const AudioStreamBasicDescription streamDescription,UInt32 maxPacketSize,Float64 seconds,UInt32 *bufferSize,UInt32 *numPackets){
	static const UInt32 maxBufferSize = 0x10000; // limit size to 64K
	static const UInt32 minBufferSize = 0x4000; // limit size to 16K

	if(streamDescription.mFramesPerPacket){
		Float64 numPacketsForTime=streamDescription.mSampleRate/streamDescription.mFramesPerPacket*seconds;
		*bufferSize=numPacketsForTime*maxPacketSize;
	}
	else{
		// if frames per packet is zero, then the codec has no predictable packet == time
		// so we can't tailor this (we don't know how many Packets represent a time period
		// we'll just return a default buffer size
		*bufferSize=maxBufferSize>maxPacketSize ? maxBufferSize : maxPacketSize;
	}
	
	// we're going to limit our size to our default
	if(*bufferSize > maxBufferSize && *bufferSize > maxPacketSize){
		*bufferSize = maxBufferSize;
	}
	else{
		// also make sure we're not too small - we don't want to go the disk for too small chunks
		if(*bufferSize < minBufferSize)
			*bufferSize = minBufferSize;
	}
	*numPackets=*bufferSize / maxPacketSize;
}

bool CoreAudio::disposeBuffer(AudioQueueRef audioQueue,egg::Collection<AudioQueueBufferRef> bufferDisposeList,AudioQueueBufferRef bufferToDispose){
	int i;
	for(i=0;i<bufferDisposeList.size();++i){
		if(bufferToDispose==bufferDisposeList[i]){
			OSStatus result=AudioQueueFreeBuffer(audioQueue,bufferToDispose);
			if(result==0){
				bufferDisposeList.erase(bufferDisposeList.begin()+i);
				return true;
			}
		}
	}

	return false;
}

void CoreAudio::queueCallback(void *userData,AudioQueueRef audioQueue,AudioQueueBufferRef audioQueueBuffer){
	OSStatus result=0;
	CoreAudio *audio=(CoreAudio*)userData;

	// dispose of the buffer if no longer in use
	if(disposeBuffer(audioQueue,audio->mBuffersToDispose,audioQueueBuffer)){
		return;
	}

	UInt32 numBytes=0;
	UInt32 numPackets=audio->mNumPacketsToRead;
	result=AudioFileReadPackets(audio->mStream->getAudioFileID(),false,&numBytes,audio->mPacketDescs,audio->mCurrentPacket,&numPackets,audioQueueBuffer->mAudioData);
	if(result!=0){
		Logger::error(Categories::TOADLET_RIBBIT,
			"Error reading file data");
	}
	audioQueueBuffer->mAudioDataByteSize=numBytes;

	// If we just played the final buffer
	if(numPackets==0){
		if(audioQueueBuffer==audio->mBuffers[numBuffers-1]){
			if(audio->mLooping){
				numPackets=audio->mNumPacketsToRead;
				audio->mCurrentPacket=0;
				result=AudioFileReadPackets(audio->mStream->getAudioFileID(),false,&numBytes,audio->mPacketDescs,audio->mCurrentPacket,&numPackets,audioQueueBuffer->mAudioData);
				audioQueueBuffer->mAudioDataByteSize=numBytes;

				if(result!=0){
					Logger::error(Categories::TOADLET_RIBBIT,
						"Error reading file data");
				}
			}
			else{
				return;
			}
		}
		else{
			return;
		}
	}

	result=AudioQueueEnqueueBuffer(audioQueue,audioQueueBuffer,audio->mPacketDescs?numPackets:0,audio->mPacketDescs);
	if(result!=0){
		Logger::error(Categories::TOADLET_RIBBIT,
			String("Error enqueuing new buffer:")+(int)result);
	}

	audio->mCurrentPacket+=numPackets;
}

bool CoreAudio::setupQueue(){
	UInt32 propertySize=0;
	OSStatus result=0;

	propertySize=sizeof(UInt64);
	result=AudioFileGetProperty(mStream->getAudioFileID(),kAudioFilePropertyAudioDataByteCount,&propertySize,&mFileDataSize);
	if(result!=0){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"Error getting file data size");
		return false;
	}

	result=AudioQueueNewOutput(&mStream->getStreamDescription(),queueCallback,this,CFRunLoopGetCurrent(),kCFRunLoopCommonModes,0,&mAudioQueue);
	if(result!=0){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"Error in AudioQueueNewOutput");
		return false;
	}

	propertySize=sizeof(UInt32);
	result=AudioFileGetPropertyInfo(mStream->getAudioFileID(),kAudioFilePropertyMagicCookieData,&propertySize,NULL);

	if(!result && propertySize){
		char *cookie=new char[propertySize];
		result=AudioFileGetProperty(mStream->getAudioFileID(),kAudioFilePropertyMagicCookieData,&propertySize,cookie);
		if(result==0){
			result=AudioQueueSetProperty(mAudioQueue,kAudioQueueProperty_MagicCookie,cookie,propertySize);
		}
		delete[] cookie;
		if(result!=0){
			Error::unknown(Categories::TOADLET_RIBBIT,
				"Error setting magic cookie");
			return false;
		}
	}

	result=AudioFileGetPropertyInfo(mStream->getAudioFileID(),kAudioFilePropertyChannelLayout,&propertySize,NULL);
	if(result==0 && propertySize>0){
		AudioChannelLayout *acl=(AudioChannelLayout*)malloc(propertySize);
		result=AudioFileGetProperty(mStream->getAudioFileID(),kAudioFilePropertyChannelLayout,&propertySize,acl);
		if(result==0){
			result=AudioQueueSetProperty(mAudioQueue,kAudioQueueProperty_ChannelLayout,acl,propertySize);
		}
		free(acl);
		if(result!=0){
			Error::unknown(Categories::TOADLET_RIBBIT,
				"Error setting AudioChannelLayout");
			return false;
		}
	}

	/// @todo: Why is this commented out?
//	result=AudioQueueAddPropertyListener(mAudioQueue,kAudioQueueProperty_IsRunning,queueStopped,this);
//	if(result!=0){
//		Error::unknown(Logger::TOADLET_RIBBIT,
//			"Error in AudioQueueAddPropertyListener");
//		return false;
//	}

	return true;
}

bool CoreAudio::setupBuffers(){
	int numBuffersToQueue=numBuffers;
	OSStatus result=0;
	UInt32 maxPacketSize;
	UInt32 propertySize=sizeof(maxPacketSize);
	// we need to calculate how many packets we read at a time, and how big a buffer we need
	// we base this on the size of the packets in the file and an approximate duration for each buffer
		
	// first check to see what the max size of a packet is - if it is bigger
	// than our allocation default size, that needs to become larger
	result=AudioFileGetProperty(mStream->getAudioFileID(),kAudioFilePropertyPacketSizeUpperBound,&propertySize,&maxPacketSize);
	if(result!=0){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"Error getting upper bound of packet size");
		return false;
	}

	UInt32 bufferByteSize=0;
	calculateBytesForTime(mStream->getStreamDescription(),maxPacketSize,0.5/*seconds*/,&bufferByteSize,(UInt32*)&mNumPacketsToRead);

	/// @todo: Why is this all commented out?
	// if the file is smaller than the capacity of all the buffer queues, always load it at once
//	if((bufferByteSize*numBuffersToQueue)>mFileDataSize){
//		mLoadAtOnce=true;
//	}
/*
	if(inFileInfo->mLoadAtOnce){
		UInt64 theFileNumPackets;
		propertySize=sizeof(UInt64);
		result=AudioFileGetProperty(mStream->getAudioFileID(),kAudioFilePropertyAudioDataPacketCount,&propertySize,&theFileNumPackets);
		if(result!=0){
			Error::unknown(Logger::TOADLET_RIBBIT,
				"Error getting packet count");
			return false;
		}
		
		mNumPacketsToRead=(UInt32)theFileNumPackets;
		bufferByteSize=mFileDataSize;
		numBuffersToQueue=1;
	}
	else{
*/		mNumPacketsToRead=bufferByteSize/maxPacketSize;
//	}

	if(mStream->isVariableBitRate()){
		mPacketDescs=new AudioStreamPacketDescription[mNumPacketsToRead];
	}

	// allocate the queue's buffers
	int i;
	for(i=0;i<numBuffersToQueue;++i){
		result=AudioQueueAllocateBuffer(mAudioQueue,bufferByteSize,&mBuffers[i]);
		if(result!=0){
			Error::unknown(Categories::TOADLET_RIBBIT,
				"Error allocating buffer for queue");
			return false;
		}

		queueCallback(this,mAudioQueue,mBuffers[i]);
		/// @todo: Why is this commented out?
//		if(mLoadAtOnce){
//			mFileDataInQueue=true;
//		}
	}

	return true;
}

}
}
