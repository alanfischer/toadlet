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

#ifndef TOADLET_RIBBIT_BASEAUDIODEVICE_H
#define TOADLET_RIBBIT_BASEAUDIODEVICE_H

#include <toadlet/ribbit/AudioDevice.h>

namespace toadlet{
namespace ribbit{

class BaseAudioDevice:public Object,public AudioDevice{
public:
	TOADLET_OBJECT(BaseAudioDevice);

	virtual ~BaseAudioDevice(){}

	virtual bool create(int options=0){return false;}
	virtual void destroy(){}

	virtual void activate(){}
	virtual void deactivate(){}

	virtual AudioBuffer *createAudioBuffer(){return NULL;}
	virtual Audio *createAudio(){return NULL;}

	virtual void setListenerTranslate(const Vector3 &translate){}
	virtual void setListenerRotate(const Quaternion &rotate){}
	virtual void setListenerVelocity(const Vector3 &velocity){}
	virtual void setListenerGain(scalar gain){}

	virtual void update(int dt){}

	virtual bool getAudioCaps(AudioCaps &caps){return false;}
};

}
}

#endif

