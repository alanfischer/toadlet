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

#ifndef TOADLET_RIBBIT_ALAUDIOPEER_H
#define TOADLET_RIBBIT_ALAUDIOPEER_H

#include <toadlet/ribbit/Types.h>
#include <toadlet/ribbit/Audio.h>
#include <toadlet/ribbit/AudioStream.h>

namespace toadlet{
namespace ribbit{

class ALPlayer;

class TOADLET_API ALAudioPeer:public AudioPeer{
public:
	ALAudioPeer(ALPlayer *player);
	virtual ~ALAudioPeer();

	void setAudio(Audio *audio);

	bool loadAudioBuffer(AudioBuffer::ptr buffer);
	bool loadAudioStream(AudioStream::ptr stream);

	bool play();
	bool stop();
	bool getPlaying() const;

	void setGain(scalar gain);
	void fadeToGain(scalar gain,int time);
	scalar getGain() const;
	scalar getTargetGain() const;
	int getFadeTime();

	void setRolloffFactor(scalar factor);
	scalar getRolloffFactor() const;

	void setLooping(bool looping);
	bool getLooping() const;

	void setGlobal(bool global);
	bool getGlobal() const;

	void setPitch(scalar pitch);
	scalar getPitch() const;

	void setGroup(const egg::String &group);
	const egg::String &getGroup() const;

	void setPosition(const Vector3 &position);
	Vector3 getPosition() const;

	void setVelocity(const Vector3 &velocity);

	bool getFinished() const;

	void setSourceHandle(int handle);
	int getSourceHandle();

	void setStream(AudioStream::ptr stream);
	AudioStream::ptr getStream();

	void setStreamingBuffers(unsigned int *buffers);
	unsigned int *getStreamingBuffers();

	void setTotalBuffersPlayed(int buffersPlayed);
	int getTotalBuffersPlayed();

	void internal_setGain(scalar gain);
	void internal_playerShutdown();

protected:
	bool mGlobal;
	bool mLooping;
	ALuint mSourceHandle;
	ALPlayer *mALPlayer;
	AudioStream::ptr mStream;
	unsigned int *mStreamingBuffers;
	int mTotalBuffersPlayed;
	scalar mTargetGain;
	scalar mGain;
	int mFadeTime;
	egg::String mGroup;
};

}
}

#endif
