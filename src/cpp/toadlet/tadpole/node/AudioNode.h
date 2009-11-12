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

#ifndef TOADLET_TADPOLE_NODE_AUDIONODE_H
#define TOADLET_TADPOLE_NODE_AUDIONODE_H

#include <toadlet/tadpole/node/Node.h>
#include <toadlet/ribbit/Audio.h>

namespace toadlet{
namespace tadpole{
namespace node{

/// @todo: Add proper world position & direction updating of the Audio, same with LightNode
class TOADLET_API AudioNode:public Node{
public:
	TOADLET_NODE(AudioNode,Node);

	AudioNode();
	virtual Node *create(Engine *engine);
	virtual void destroy();

	bool loadAudioBuffer(const ribbit::AudioBuffer::ptr &audioBuffer);
	bool loadAudioStream(egg::io::InputStream::ptr in,const egg::String &extension);

	inline bool play(){return mAudio->play();}
	inline bool stop(){return mAudio->stop();}
	inline bool getPlaying() const{return mAudio->getPlaying();}
	inline bool getFinished() const{return mAudio->getFinished();}

	inline void setGain(scalar gain){mAudio->setGain(gain);}
	inline void fadeToGain(scalar gain,int time){mAudio->fadeToGain(gain,time);}
	inline scalar getGain() const{return mAudio->getGain();}

	inline void setLooping(bool looping){mAudio->setLooping(looping);}
	inline bool getLooping() const{return mAudio->getLooping();}

	inline void setPitch(scalar pitch){mAudio->setPitch(pitch);}
	inline scalar getPitch() const{return mAudio->getPitch();}

	inline void setGlobal(bool global){mAudio->setGlobal(global);}
	inline bool getGlobal() const{return mAudio->getGlobal();}

	inline void setRolloffFactor(scalar f){mAudio->setRolloffFactor(f);}
	inline scalar getRolloffFactor() const{return mAudio->getRolloffFactor();}

	inline void setVelocity(const Vector3 &velocity){mAudio->setVelocity(velocity);}

protected:
	ribbit::Audio::ptr mAudio;
};

}
}
}

#endif

