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

#ifndef TOADLET_RIBBIT_BASEAUDIO_H
#define TOADLET_RIBBIT_BASEAUDIO_H

#include <toadlet/egg/BaseResource.h>
#include <toadlet/ribbit/Audio.h>

namespace toadlet{
namespace ribbit{

class BaseAudio:public BaseResource,public Audio{
public:
	TOADLET_RESOURCE(BaseAudio,Audio);

	virtual ~BaseAudio(){}

	virtual bool create(AudioBuffer *buffer){return false;}
	virtual bool create(AudioStream *stream){return false;}
	virtual void destroy(){}

	virtual AudioBuffer *getAudioBuffer(){return NULL;}
	virtual AudioStream *getAudioStream(){return NULL;}

	virtual bool play(){return false;}
	virtual bool stop(){return false;}
	virtual bool getPlaying() const{return false;}
	virtual bool getFinished() const{return false;}

	virtual void setLooping(bool looping){}
	virtual bool getLooping() const{return false;}

	virtual void setGain(scalar gain){}
	virtual scalar getGain() const{return 0;}

	virtual void setPitch(scalar pitch){}
	virtual scalar getPitch() const{return 0;}

	virtual void setGlobal(bool global){}
	virtual bool getGlobal() const{return false;}

	virtual void setPosition(const Vector3 &position){}
	virtual void setVelocity(const Vector3 &velocity){}

	virtual void setRolloffFactor(scalar f){}
	virtual scalar getRolloffFactor() const{return 0;}
};

}
}

#endif
