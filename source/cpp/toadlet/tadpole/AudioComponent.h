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

#ifndef TOADLET_TADPOLE_AUDIOCOMPONENT_H
#define TOADLET_TADPOLE_AUDIOCOMPONENT_H

#include <toadlet/ribbit/Audio.h>
#include <toadlet/tadpole/BaseComponent.h>
#include <toadlet/tadpole/Engine.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API AudioComponent:public BaseComponent{
public:
	TOADLET_COMPONENT(AudioComponent);

	AudioComponent(Engine *engine);
	virtual void destroy();

	bool setAudioBuffer(const String &name,int track=0);
	bool setAudioBuffer(AudioBuffer *audioBuffer);

	bool setAudioStream(const String &name,int track=0);
	bool setAudioStream(AudioStream *stream);

	AudioBuffer *getAudioBuffer(){return mAudio!=NULL?mAudio->getAudioBuffer():NULL;}
	AudioStream *getAudioStream(){return mAudio!=NULL?mAudio->getAudioStream():NULL;}
	
	inline bool play(){if(mAudio!=NULL){return mAudio->play();}else{return false;}}
	inline bool stop(){if(mAudio!=NULL){return mAudio->stop();}else{return false;}}
	inline bool getPlaying() const{if(mAudio!=NULL){return mAudio->getPlaying();}else{return false;}}
	inline bool getFinished() const{if(mAudio!=NULL){return mAudio->getFinished();}else{return false;}}

	inline void setGain(scalar gain){mGain=gain;if(mAudio!=NULL){mAudio->setGain(gain);}}
	inline scalar getGain() const{if(mAudio!=NULL){return mAudio->getGain();}else{return mGain;}}

	inline void setPitch(scalar pitch){mPitch=pitch;if(mAudio!=NULL){mAudio->setPitch(pitch);}}
	inline scalar getPitch() const{if(mAudio!=NULL){return mAudio->getPitch();}else{return mPitch;}}

	inline void setRolloffFactor(scalar rolloff){mRolloff=rolloff;if(mAudio!=NULL){mAudio->setRolloffFactor(rolloff);}}
	inline scalar getRolloffFactor() const{if(mAudio!=NULL){return mAudio->getRolloffFactor();}else{return mRolloff;}}

	inline void setLooping(bool looping){mLooping=looping;if(mAudio!=NULL){mAudio->setLooping(looping);}}
	inline bool getLooping() const{if(mAudio!=NULL){return mAudio->getLooping();}else{return mLooping;}}

	inline void setGlobal(bool global){mGlobal=global;if(mAudio!=NULL){mAudio->setGlobal(global);}}
	inline bool getGlobal() const{if(mAudio!=NULL){return mAudio->getGlobal();}else{return mGlobal;}}

	void frameUpdate(int dt,int scope);

protected:
	void setAudioFromTransform();
	void setAudioFromVelocity();
	void setAudioParameters();

	Engine *mEngine;
	Audio::ptr mAudio;
	scalar mGain;
	scalar mPitch;
	scalar mRolloff;
	bool mLooping;
	bool mGlobal;
};

}
}

#endif

