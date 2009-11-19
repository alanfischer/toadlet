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

#ifndef TOADLET_RIBBIT_WIN32AUDIO_H
#define TOADLET_RIBBIT_WIN32AUDIO_H

#include "Win32Player.h"
#include <toadlet/ribbit/Audio.h>
#include <toadlet/ribbit/AudioBuffer.h>

namespace toadlet{
namespace ribbit{

class TOADLET_API Win32Audio:public Audio{
public:
	Win32Audio(Win32Player *player);
	virtual ~Win32Audio();

	virtual bool create(AudioBuffer::ptr audioBuffer);
	virtual bool create(egg::io::InputStream::ptr in,const egg::String &mimeType);
	virtual void destroy();

	virtual bool play();
	virtual bool stop();
	virtual bool getPlaying() const{return false;}
	virtual bool getFinished() const{return false;}

	virtual void setGain(scalar gain){}
	virtual void fadeToGain(scalar gain,int time){}
	virtual scalar getGain() const{return 0;}

	virtual void setLooping(bool looping){}
	virtual bool getLooping() const{return 0;}

	virtual void setPitch(scalar pitch){mPitch=pitch;}
	virtual scalar getPitch() const{return mPitch;}

	virtual void setGlobal(bool global){}
	virtual bool getGlobal() const{return false;}

	virtual void setRolloffFactor(scalar r){}
	virtual scalar getRolloffFactor() const{return 0;}

	virtual void setPosition(const Vector3 &position){}
	virtual const Vector3 &getPosition() const{return Math::ZERO_VECTOR3;}

	virtual void setVelocity(const Vector3 &velocity){}
	virtual const Vector3 &getVelocity() const{return Math::ZERO_VECTOR3;}

protected:
	Win32Player *mPlayer;
	AudioBuffer::ptr mAudioBuffer;
	scalar mPitch;
	uint64 mLastSoundTime;
};

}
}

#endif
