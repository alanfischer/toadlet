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

#ifndef TOADLET_RIBBIT_COREAUDIO_H
#define TOADLET_RIBBIT_COREAUDIO_H

#include <toadlet/ribbit/Types.h>
#include <toadlet/ribbit/Audio.h>
#include <toadlet/ribbit/plugins/decoders/coreaudiodecoder/CoreAudioDecoder.h>
#include <AudioToolbox/AudioToolbox.h>

namespace toadlet{
namespace ribbit{

class AudioStream;

class TOADLET_API CoreAudio:public Audio{
public:
	CoreAudio(ALPlayer *audioPlayer);
	virtual ~CoreAudio();

	bool create(AudioBuffer::ptr buffer);
	bool create(egg::io::Stream::ptr stream,const egg::String &mimeType);
	bool create(AudioStream::ptr stream);
	void destroy();

	bool play();
	bool stop();
	bool getPlaying() const;
	bool getFinished() const;

	void setGain(scalar gain);
	void fadeToGain(scalar gain,int time);
	scalar getGain() const;

	void setRolloffFactor(scalar factor){}
	scalar getRolloffFactor() const{return 0;}

	void setLooping(bool looping);
	bool getLooping() const{return mLooping;}

	void setGlobal(bool global){}
	bool getGlobal() const{return true;}

	void setPitch(scalar pitch){}
	scalar getPitch() const{return 0;}

	void setPosition(const Vector3 &position){}
	void setVelocity(const Vector3 &velocity){}

	void setSourceHandle(int handle);
	int getSourceHandle();

	void update(int dt);
	
	void setImmediateGain(scalar gain);

protected:
	bool setupQueue();
	bool setupBuffers();

	static void calculateBytesForTime(const AudioStreamBasicDescription streamDescription,UInt32 maxPacketSize,Float64 seconds,UInt32 *bufferSize,UInt32 *numPackets);
	static bool disposeBuffer(AudioQueueRef audioQueue,egg::Collection<AudioQueueBufferRef> bufferDisposeList,AudioQueueBufferRef bufferToDispose);
	static void queueCallback(void *userData,AudioQueueRef audioQueue,AudioQueueBufferRef audioQueueBuffer);

	bool mLooping;
	ALPlayer *mAudioPlayer;
	CoreAudioDecoder::ptr mStream;
	scalar mTargetGain;
	scalar mGain;
	int mFadeTime;

	const static int numBuffers=3;
	AudioQueueRef mAudioQueue;
	AudioQueueBufferRef mBuffers[numBuffers];
	egg::Collection<AudioQueueBufferRef> mBuffersToDispose;
	int mNumPacketsToRead;
	int mCurrentPacket;
	AudioStreamPacketDescription *mPacketDescs;
	UInt64 mFileDataSize;
	
	friend class ALPlayer;
};

}
}

#endif
