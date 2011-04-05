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

#ifndef TOADLET_RIBBIT_MMAUDIO_H
#define TOADLET_RIBBIT_MMAUDIO_H

#include "MMPlayer.h"
#include "MMAudioBuffer.h"
#include <toadlet/ribbit/Audio.h>

namespace toadlet{
namespace ribbit{

class TOADLET_API MMAudio:public Audio{
public:
	MMAudio(MMPlayer *player);
	virtual ~MMAudio();

	bool create(AudioBuffer::ptr audioBuffer);
	bool create(AudioStream::ptr stream);
	void destroy();

	AudioBuffer::ptr getAudioBuffer(){return mAudioBuffer;}
	AudioStream::ptr getAudioStream(){return mAudioStream;}

	bool play(){mPosition=0;mPlaying=true;return true;}
	bool stop(){mPlaying=false;return false;}
	bool getPlaying() const{return mPlaying;}
	bool getFinished() const{return !mPlaying;}

	void setLooping(bool looping){mLooping=looping;}
	bool getLooping() const{return mLooping;}

	void setGain(scalar gain){mGain=gain;}
	void fadeToGain(scalar gain,int time){mGain=gain;}
	scalar getGain() const{return mGain;}

	void setPitch(scalar pitch){mPitch=pitch;}
	scalar getPitch() const{return mPitch;}

	void setGlobal(bool global){}
	bool getGlobal() const{return false;}

	void setRolloffFactor(scalar r){}
	scalar getRolloffFactor() const{return 0;}

	void setPosition(const Vector3 &position){}
	const Vector3 &getPosition() const{return Math::ZERO_VECTOR3;}

	void setVelocity(const Vector3 &velocity){}
	const Vector3 &getVelocity() const{return Math::ZERO_VECTOR3;}

protected:
	int read(tbyte *data,int length);

	MMPlayer *mPlayer;
	AudioStream::ptr mAudioStream;
	AudioBuffer::ptr mAudioBuffer;
	bool mPlaying;
	bool mLooping;
	scalar mGain;
	scalar mPitch;

	int mPosition;

	friend class MMPlayer;
};

}
}

#endif
