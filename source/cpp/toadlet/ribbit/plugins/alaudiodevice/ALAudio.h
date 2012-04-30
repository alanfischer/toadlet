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

#ifndef TOADLET_RIBBIT_ALAUDIO_H
#define TOADLET_RIBBIT_ALAUDIO_H

#include <toadlet/egg/BaseResource.h>
#include <toadlet/ribbit/Types.h>
#include <toadlet/ribbit/Audio.h>
#include <toadlet/ribbit/AudioStream.h>

namespace toadlet{
namespace ribbit{

class ALAudioDevice;

class TOADLET_API ALAudio:public BaseResource,public Audio{
public:
	TOADLET_RESOURCE(ALAudio,Audio);

	ALAudio(ALAudioDevice *audioDevice);
	virtual ~ALAudio();

	bool create(AudioBuffer *buffer);
	bool create(AudioStream *stream);
	void destroy();

	AudioBuffer *getAudioBuffer(){return mAudioBuffer;}
	AudioStream *getAudioStream(){return mAudioStream;}

	bool play();
	bool stop();
	bool getPlaying() const;
	bool getFinished() const;

	void setLooping(bool looping);
	bool getLooping() const;

	void setGain(scalar gain);
	void fadeToGain(scalar gain,int time);
	scalar getGain() const{return mGain;}

	void setRolloffFactor(scalar factor);
	scalar getRolloffFactor() const;

	void setGlobal(bool global);
	bool getGlobal() const{return mGlobal;}

	void setPitch(scalar pitch);
	scalar getPitch() const;

	void setPosition(const Vector3 &position);
	void setVelocity(const Vector3 &velocity);

	inline ALuint getHandle() const{return mHandle;}

protected:
	void update(int dt);
	void updateStreaming(int dt);
	int readAudioData(tbyte *buffer,int bsize);
	void setImmediateGain(scalar gain);

	ALAudioDevice *mDevice;
	bool mGlobal;
	bool mLooping;
	ALuint mHandle;
	ALuint *mStreamingBuffers;
	int mTotalBuffersPlayed,mTotalBuffersQueued;
	scalar mGain;
	AudioBuffer::ptr mAudioBuffer;
	AudioStream::ptr mAudioStream;

	friend class ALAudioDevice;
};

}
}

#endif
