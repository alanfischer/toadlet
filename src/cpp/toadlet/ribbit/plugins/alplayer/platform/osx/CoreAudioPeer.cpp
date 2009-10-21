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
#include "CoreAudioPeer.h"

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace ribbit{

CoreAudioPeer::CoreAudioPeer(ALPlayer *player){
	mALPlayer=player;
	mLooping=false;
	mTargetGain=Math::ONE;
	mGain=Math::ONE;
	mFadeTime=0;

	mAudioQueue=0;
	mNumPacketsToRead=0;
	mCurrentPacket=0;
	mPacketDescs=NULL;
	mFileDataSize=0;
}

CoreAudioPeer::~CoreAudioPeer(){
	if(mALPlayer!=NULL){
		mALPlayer->internal_audioPeerDestroy(this);
	}

	if(mAudioQueue!=NULL){
		AudioQueueDispose(mAudioQueue,true);
	}
}

bool CoreAudioPeer::loadAudioStream(AudioStream::ptr stream){
	if(mALPlayer!=NULL){
		mStream=shared_static_cast<CoreAudioDecoder>(stream);

		setupQueue();

		setupBuffers();

		return true;
	}
	return false;
}

bool CoreAudioPeer::play(){
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

bool CoreAudioPeer::stop(){
	OSStatus result=AudioQueueStop(mAudioQueue,true);
	if(result!=0){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"Error in AudioQueueStop");
		return false;
	}

	return true;
}

bool CoreAudioPeer::getPlaying() const{
	UInt32 isRunning=0;
	OSStatus result=AudioQueueGetParameter(mAudioQueue,kAudioQueueProperty_IsRunning,(AudioQueueParameterValue*)&isRunning);
	if(result!=0){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"Error in AudioQueueGetParameter");
		return false;
	}

	return isRunning!=0;
}

void CoreAudioPeer::setGain(scalar gain){
	if(mALPlayer!=NULL){
		mALPlayer->lock();

		internal_setGain(gain);
		mTargetGain=gain;
		mFadeTime=0;

		mALPlayer->unlock();
	}
}

void CoreAudioPeer::fadeToGain(scalar gain,int time){
	if(mALPlayer!=NULL){
		mALPlayer->lock();

		mTargetGain=gain;
		mFadeTime=time;

		mALPlayer->unlock();
	}
}

scalar CoreAudioPeer::getGain() const{
	return mGain;
}

scalar CoreAudioPeer::getTargetGain() const{
	return mTargetGain;
}

int CoreAudioPeer::getFadeTime(){
	return mFadeTime;
}

void CoreAudioPeer::setLooping(bool looping){
	mLooping=looping;
}

void CoreAudioPeer::setGroup(const String &group){
	mGroup=group;
	setGain(mTargetGain);
}

bool CoreAudioPeer::getFinished() const{
	return false; // TODO FINISH ME
}

void CoreAudioPeer::internal_setGain(scalar gain){
	mGain=gain;

	scalar groupGain=mALPlayer->internal_getGroupGain(mGroup);

	gain=Math::mul(gain,groupGain);

	AudioQueueSetParameter(mAudioQueue,kAudioQueueParam_Volume,MathConversion::scalarToFloat(gain));
}

void CoreAudioPeer::internal_playerShutdown(){
	mALPlayer=NULL;
}

void CoreAudioPeer::calculateBytesForTime(const AudioStreamBasicDescription streamDescription,UInt32 maxPacketSize,Float64 seconds,UInt32 *bufferSize,UInt32 *numPackets){
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

bool CoreAudioPeer::disposeBuffer(AudioQueueRef audioQueue,egg::Collection<AudioQueueBufferRef> bufferDisposeList,AudioQueueBufferRef bufferToDispose){
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

void CoreAudioPeer::queueCallback(void *userData,AudioQueueRef audioQueue,AudioQueueBufferRef audioQueueBuffer){
	OSStatus result=0;
	CoreAudioPeer *peer=(CoreAudioPeer*)userData;

	// dispose of the buffer if no longer in use
	if(disposeBuffer(audioQueue,peer->mBuffersToDispose,audioQueueBuffer)){
		return;
	}

	UInt32 numBytes=0;
	UInt32 numPackets=peer->mNumPacketsToRead;
	result=AudioFileReadPackets(peer->mStream->getAudioFileID(),false,&numBytes,peer->mPacketDescs,peer->mCurrentPacket,&numPackets,audioQueueBuffer->mAudioData);
	if(result!=0){
		Logger::log(Categories::TOADLET_RIBBIT,Logger::Level_ERROR,
			"Error reading file data");
	}
	audioQueueBuffer->mAudioDataByteSize=numBytes;

	// If we just played the final buffer
	if(numPackets==0){
		if(audioQueueBuffer==peer->mBuffers[numBuffers-1]){
			if(peer->mLooping){
				numPackets=peer->mNumPacketsToRead;
				peer->mCurrentPacket=0;
				result=AudioFileReadPackets(peer->mStream->getAudioFileID(),false,&numBytes,peer->mPacketDescs,peer->mCurrentPacket,&numPackets,audioQueueBuffer->mAudioData);
				audioQueueBuffer->mAudioDataByteSize=numBytes;

				if(result!=0){
					Logger::log(Categories::TOADLET_RIBBIT,Logger::Level_ERROR,
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

	result=AudioQueueEnqueueBuffer(audioQueue,audioQueueBuffer,peer->mPacketDescs?numPackets:0,peer->mPacketDescs);
	if(result!=0){
		Logger::log(Categories::TOADLET_RIBBIT,Logger::Level_ERROR,
			String("Error enqueuing new buffer:")+(int)result);
	}

	peer->mCurrentPacket+=numPackets;
}

bool CoreAudioPeer::setupQueue(){
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

	// TODO: Why is this commented out?
//	result=AudioQueueAddPropertyListener(mAudioQueue,kAudioQueueProperty_IsRunning,queueStopped,this);
//	if(result!=0){
//		Error::unknown(Logger::TOADLET_RIBBIT,
//			"Error in AudioQueueAddPropertyListener");
//		return false;
//	}

	return true;
}

bool CoreAudioPeer::setupBuffers(){
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

	// TODO: Why is this all commented out?
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
		// TODO: Why is this commented out?
//		if(mLoadAtOnce){
//			mFileDataInQueue=true;
//		}
	}

	return true;
}

}
}
