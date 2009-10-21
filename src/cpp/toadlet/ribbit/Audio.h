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

#ifndef TOADLET_RIBBIT_AUDIO_H
#define TOADLET_RIBBIT_AUDIO_H

#include <toadlet/ribbit/AudioBuffer.h>

namespace toadlet{
namespace ribbit{

class AudioPlayer;

class AudioPeer{
public:
	virtual ~AudioPeer(){}

	virtual void setGain(scalar gain)=0;
	virtual void fadeToGain(scalar gain,int time)=0; // milliseconds
	virtual scalar getGain() const=0;

	virtual bool play()=0;
	virtual bool stop()=0;
	virtual bool getPlaying() const=0;

	virtual void setLooping(bool looping)=0;
	virtual bool getLooping() const=0;

	virtual void setPitch(scalar pitch)=0;
	virtual scalar getPitch() const=0;

	virtual void setGroup(const egg::String &group)=0;
	virtual const egg::String &getGroup() const=0;
	
	virtual void setGlobal(bool global)=0;
	virtual bool getGlobal() const=0;

	virtual void setRolloffFactor(scalar r)=0;
	virtual scalar getRolloffFactor() const=0;

	virtual void setPosition(const Vector3 &position)=0;

	virtual void setVelocity(const Vector3 &velocity)=0;

	virtual bool getFinished() const=0;
};

class TOADLET_API Audio{
public:
	TOADLET_SHARED_POINTERS(Audio,Audio);

	Audio();
	Audio(AudioPlayer *player);

	virtual ~Audio();

	bool loadAudioBuffer(const AudioBuffer::ptr &audioBuffer);
	bool loadAudioStream(egg::io::InputStream::ptr in,const egg::String &extension);
	AudioBuffer::ptr getAudioBuffer() const;

	bool play();
	bool stop();
	bool getPlaying() const;

	void setGain(scalar gain);
	void fadeToGain(scalar gain,int time); // milliseconds
	scalar getGain() const;

	void setLooping(bool looping);
	bool getLooping() const;

	void setPitch(scalar pitch);
	scalar getPitch() const;

	void setGlobal(bool global);
	bool getGlobal() const;

	void setGroup(const egg::String &name);
	const egg::String &getGroup() const;

	void setPosition(const Vector3 &position);

	void setRolloffFactor(scalar f);
	scalar getRolloffFactor() const;

	bool getFinished() const;

	void setVelocity(const Vector3 &velocity);

	virtual void internal_setAudioPlayer(AudioPlayer *player);
	inline AudioPeer *internal_getAudioPeer() const{return mAudioPeer;}
	virtual void internal_destroyAudioPeer();

protected:
	AudioPlayer *mAudioPlayer;
	AudioBuffer::ptr mAudioBuffer;
	bool mLooping;

	static egg::String EMPTY_STRING;

	AudioPeer *mAudioPeer;
};

}
}

#endif
