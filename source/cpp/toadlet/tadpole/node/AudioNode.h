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
	virtual Node *create(Scene *scene);
	virtual void destroy();
	virtual Node *set(Node *node);

	bool setAudioBuffer(const String &name);
	bool setAudioBuffer(const AudioBuffer::ptr &audioBuffer);

	bool setAudioStream(const String &name);
	bool setAudioStream(AudioStream::ptr stream);

	AudioBuffer::ptr getAudioBuffer(){return mAudio->getAudioBuffer();}
	AudioStream::ptr getAudioStream(){return mAudio->getAudioStream();}
	
	inline bool play(){if(mAudio!=NULL){return mAudio->play();}else{return false;}}
	inline bool stop(){if(mAudio!=NULL){return mAudio->stop();}else{return false;}}
	inline bool getPlaying() const{if(mAudio!=NULL){return mAudio->getPlaying();}else{return false;}}
	inline bool getFinished() const{if(mAudio!=NULL){return mAudio->getFinished();}else{return false;}}

	inline void setGain(scalar gain){if(mAudio!=NULL){mAudio->setGain(gain);}}
	inline scalar getGain() const{if(mAudio!=NULL){return mAudio->getGain();}else{return 0;}}

	inline void setLooping(bool looping){if(mAudio!=NULL){mAudio->setLooping(looping);}}
	inline bool getLooping() const{if(mAudio!=NULL){return mAudio->getLooping();}else{return false;}}

	inline void setPitch(scalar pitch){if(mAudio!=NULL){mAudio->setPitch(pitch);}}
	inline scalar getPitch() const{if(mAudio!=NULL){return mAudio->getPitch();}else{return 0;}}

	inline void setGlobal(bool global){if(mAudio!=NULL){mAudio->setGlobal(global);}}
	inline bool getGlobal() const{if(mAudio!=NULL){return mAudio->getGlobal();}else{return false;}}

	inline void setRolloffFactor(scalar f){if(mAudio!=NULL){mAudio->setRolloffFactor(f);}}
	inline scalar getRolloffFactor() const{if(mAudio!=NULL){return mAudio->getRolloffFactor();}else{return 0;}}

	/// @todo: This needs to be integrated into the tadpole plugin
	/// @todo: Also we need to set the Audio position on a position update
	inline void setVelocity(const Vector3 &velocity){if(mAudio!=NULL){mAudio->setVelocity(velocity);}}

protected:
	Audio::ptr mAudio;
};

}
}
}

#endif
