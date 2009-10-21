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

#ifndef TOADLET_RIBBIT_WIN32AUDIOPEER_H
#define TOADLET_RIBBIT_WIN32AUDIOPEER_H

#include "Win32Player.h"
#include <toadlet/ribbit/Audio.h>
#include <toadlet/ribbit/AudioBuffer.h>

namespace toadlet{
namespace ribbit{

class TOADLET_API Win32AudioPeer:public AudioPeer{
public:
	Win32AudioPeer(Win32Player *player);
	virtual ~Win32AudioPeer();

	bool loadAudioBuffer(AudioBuffer::ptr audioBuffer);
	bool loadAudioStream(egg::io::InputStream::ptr in,const egg::String &mimeType);

	bool play();
	bool stop();
	bool getPlaying() const{return false;}

	void setGain(scalar gain){}
	void fadeToGain(scalar gain,int time){}
	scalar getGain() const{return 0;}

	void setLooping(bool looping){}
	bool getLooping() const{return 0;}

	void setPitch(scalar pitch){mPitch=pitch;}
	scalar getPitch() const{return mPitch;}

	void setGroup(const egg::String &group){}
	const egg::String &getGroup() const{static egg::String empty; return empty;}
	
	void setGlobal(bool global){}
	bool getGlobal() const{return false;}

	void setRolloffFactor(scalar r){}
	scalar getRolloffFactor() const{return 0;}

	void setPosition(const Vector3 &position){}
	Vector3 getPosition() const{return Math::ZERO_VECTOR3;}

	void setVelocity(const Vector3 &velocity){}

	bool getFinished() const{return false;}

	void internal_playerDestroy();

protected:
	Win32Player *mPlayer;
	AudioBuffer::ptr mAudioBuffer;
	scalar mPitch;
	uint64 mLastSoundTime;
};

}
}

#endif
