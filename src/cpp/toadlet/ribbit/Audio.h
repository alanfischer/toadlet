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

class Audio{
public:
	TOADLET_SHARED_POINTERS(Audio);

	virtual ~Audio(){}

	virtual bool create(AudioBuffer::ptr buffer)=0;
	virtual bool create(egg::io::Stream::ptr stream,const egg::String &mimeType)=0;
	virtual void destroy()=0;

	virtual bool play()=0;
	virtual bool stop()=0;
	virtual bool getPlaying() const=0;
	virtual bool getFinished() const=0;

	virtual void setGain(scalar gain)=0;
	virtual void fadeToGain(scalar gain,int time)=0; // milliseconds
	virtual scalar getGain() const=0;

	virtual void setLooping(bool looping)=0;
	virtual bool getLooping() const=0;

	virtual void setPitch(scalar pitch)=0;
	virtual scalar getPitch() const=0;

	virtual void setGlobal(bool global)=0;
	virtual bool getGlobal() const=0;

	virtual void setPosition(const Vector3 &position)=0;
	virtual void setVelocity(const Vector3 &velocity)=0;

	virtual void setRolloffFactor(scalar f)=0;
	virtual scalar getRolloffFactor() const=0;
};

}
}

#endif
