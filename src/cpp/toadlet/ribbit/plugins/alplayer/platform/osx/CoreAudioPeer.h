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

#ifndef TOADLET_RIBBIT_COREAUDIOPEER_H
#define TOADLET_RIBBIT_COREAUDIOPEER_H

#include <toadlet/ribbit/Types.h>
#include <toadlet/ribbit/Audio.h>
#include <toadlet/ribbit/plugins/decoders/coreaudiodecoder/CoreAudioDecoder.h>
#include <AudioToolbox/AudioToolbox.h>

namespace toadlet{
namespace ribbit{

class AudioStream;

class TOADLET_API CoreAudioPeer:public AudioPeer{
public:
	CoreAudioPeer(ALPlayer *player);
	virtual ~CoreAudioPeer();

	void setAudio(Audio *audio);

	bool loadAudioStream(AudioStream::ptr stream);

	bool play();
	bool stop();
	bool getPlaying() const;

	void setGain(scalar gain);
	void fadeToGain(scalar gain,int time);
	scalar getGain() const;
	scalar getTargetGain() const;
	int getFadeTime();

	void setRolloffFactor(scalar factor){}
	scalar getRolloffFactor() const{return 0;}

	void setLooping(bool looping);
	bool getLooping() const{return mLooping;}

	void setGlobal(bool global){}
	bool getGlobal() const{return true;}

	void setPitch(scalar pitch){}
	scalar getPitch() const{return 0;}

	void setGroup(const egg::String &group);
	const egg::String &getGroup() const{return mGroup;}

	void setPosition(const Vector3 &position){}

	void setVelocity(const Vector3 &velocity){}

	bool getFinished() const;

	void setSourceHandle(int handle);
	int getSourceHandle();

	void internal_setGain(scalar gain);
	void internal_playerShutdown();

protected:
	bool setupQueue();
	bool setupBuffers();

	static void calculateBytesForTime(const AudioStreamBasicDescription streamDescription,UInt32 maxPacketSize,Float64 seconds,UInt32 *bufferSize,UInt32 *numPackets);
	static bool disposeBuffer(AudioQueueRef audioQueue,egg::Collection<AudioQueueBufferRef> bufferDisposeList,AudioQueueBufferRef bufferToDispose);
	static void queueCallback(void *userData,AudioQueueRef audioQueue,AudioQueueBufferRef audioQueueBuffer);

	bool mLooping;
	ALPlayer *mALPlayer;
	CoreAudioDecoder::ptr mStream;
	scalar mTargetGain;
	scalar mGain;
	int mFadeTime;
	egg::String mGroup;

	const static int numBuffers=3;

	AudioQueueRef mAudioQueue;
	AudioQueueBufferRef mBuffers[numBuffers];
	egg::Collection<AudioQueueBufferRef> mBuffersToDispose;
	int mNumPacketsToRead;
	int mCurrentPacket;
	AudioStreamPacketDescription *mPacketDescs;
	UInt64 mFileDataSize;
};

}
}

#endif
