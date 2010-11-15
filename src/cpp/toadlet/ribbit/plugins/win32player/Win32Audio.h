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
#include "Win32AudioBuffer.h"
#include <toadlet/ribbit/Audio.h>

namespace toadlet{
namespace ribbit{

class TOADLET_API Win32Audio:public Audio{
public:
	Win32Audio(Win32Player *player);
	virtual ~Win32Audio();

	virtual bool create(AudioBuffer::ptr audioBuffer);
	virtual bool create(egg::io::Stream::ptr stream,const egg::String &mimeType);
	virtual bool create(AudioStream::ptr stream);
	virtual void destroy();

	virtual bool play(){mPosition=0;mPlaying=true;return true;}
	virtual bool stop(){mPlaying=false;return false;}
	virtual bool getPlaying() const{return mPlaying;}
	virtual bool getFinished() const{return !mPlaying;}

	virtual void setLooping(bool looping){mLooping=looping;}
	virtual bool getLooping() const{return mLooping;}

	virtual void setGain(scalar gain){mGain=gain;}
	virtual void fadeToGain(scalar gain,int time){mGain=gain;}
	virtual scalar getGain() const{return mGain;}

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
	int read(tbyte *data,int length);

	Win32Player *mPlayer;
	Win32AudioBuffer::ptr mAudioBuffer;
	bool mPlaying;
	bool mLooping;
	scalar mGain;
	scalar mPitch;

	int mPosition;

	friend class Win32Player;
};

}
}

#endif
