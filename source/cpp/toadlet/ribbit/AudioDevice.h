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

#ifndef TOADLET_RIBBIT_AUDIODEVICE_H
#define TOADLET_RIBBIT_AUDIODEVICE_H

#include <toadlet/ribbit/Audio.h>
#include <toadlet/ribbit/AudioBuffer.h>

namespace toadlet{
namespace ribbit{

class AudioCaps;

class AudioDevice:public Interface{
public:
	TOADLET_IPTR(AudioDevice);

	virtual ~AudioDevice(){}

	virtual bool create(int *options=NULL)=0;
	virtual void destroy()=0;

	virtual void activate()=0;
	virtual void deactivate()=0;

	virtual AudioBuffer *createAudioBuffer()=0;
	virtual Audio *createAudio()=0;

	virtual void setListenerTranslate(const Vector3 &translate)=0;
	virtual void setListenerRotate(const Matrix3x3 &rotate)=0;
	virtual void setListenerVelocity(const Vector3 &velocity)=0;
	virtual void setListenerGain(scalar gain)=0;

	virtual void update(int dt)=0;

	virtual bool getAudioCaps(AudioCaps &caps)=0;
};

}
}

#endif

