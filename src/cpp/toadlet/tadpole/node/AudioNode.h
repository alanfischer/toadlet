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

	bool setAudioBuffer(const ribbit::AudioBuffer::ptr &audioBuffer);
	bool setAudioStream(egg::io::InputStream::ptr in,const egg::String &mimeType);

	inline bool play(){return mAudio!=NULL?mAudio->play():false;}
	inline bool stop(){return mAudio!=NULL?mAudio->stop():false;}
	inline bool getPlaying() const{return mAudio!=NULL?mAudio->getPlaying():false;}
	inline bool getFinished() const{return mAudio!=NULL?mAudio->getFinished():false;}

	inline void setGain(scalar gain){mAudio!=NULL?mAudio->setGain(gain):0;}
	inline void fadeToGain(scalar gain,int time){mAudio!=NULL?mAudio->fadeToGain(gain,time):0;}
	inline scalar getGain() const{return mAudio!=NULL?mAudio->getGain():0;}

	inline void setLooping(bool looping){mAudio!=NULL?mAudio->setLooping(looping):0;}
	inline bool getLooping() const{return mAudio!=NULL?mAudio->getLooping():false;}

	inline void setPitch(scalar pitch){mAudio!=NULL?mAudio->setPitch(pitch):0;}
	inline scalar getPitch() const{return mAudio!=NULL?mAudio->getPitch():false;}

	inline void setGlobal(bool global){mAudio!=NULL?mAudio->setGlobal(global):0;}
	inline bool getGlobal() const{return mAudio!=NULL?mAudio->getGlobal():false;}

	inline void setRolloffFactor(scalar f){mAudio!=NULL?mAudio->setRolloffFactor(f):0;}
	inline scalar getRolloffFactor() const{return mAudio!=NULL?mAudio->getRolloffFactor():0;}

	inline void setVelocity(const Vector3 &velocity){mAudio!=NULL?mAudio->setVelocity(velocity):0;}

protected:
	ribbit::Audio::ptr mAudio;
};

}
}
}

#endif

