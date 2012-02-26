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

#include <toadlet/tadpole/node/AudioNode.h>
#include <toadlet/tadpole/Engine.h>

namespace toadlet{
namespace tadpole{
namespace node{

TOADLET_NODE_IMPLEMENT(AudioNode,Categories::TOADLET_TADPOLE_NODE+".AudioNode");

AudioNode::AudioNode():super(){}

Node *AudioNode::create(Scene *scene){
	super::create(scene);
	return this;
}

void AudioNode::destroy(){
	if(mAudio!=NULL){
		mAudio->destroy();
		mAudio=NULL;
	}

	super::destroy();
}

Node *AudioNode::set(Node *node){
	super::set(node);
	
	AudioNode *audioNode=(AudioNode*)node;
	if(audioNode->getAudioStream()!=NULL){
		Logger::warning("can't clone an AudioNode with an AudioStream");
	}
	if(audioNode->getAudioBuffer()!=NULL){
		setAudioBuffer(audioNode->getAudioBuffer());
	}
	
	setLooping(audioNode->getLooping());
	setGain(audioNode->getGain());
	setRolloffFactor(audioNode->getRolloffFactor());
	setGlobal(audioNode->getGlobal());
	setPitch(audioNode->getPitch());
	if(audioNode->getPlaying()){
		play();
	}

	return this;
}

bool AudioNode::setAudioBuffer(const String &name){
	return setAudioBuffer(mEngine->getAudioBufferManager()->findAudioBuffer(name));
}

bool AudioNode::setAudioBuffer(const AudioBuffer::ptr &audioBuffer){
	if(mAudio!=NULL && mAudio->getAudioBuffer()==audioBuffer){
		return true;
	}

	if(mAudio!=NULL){
		mAudio->destroy();
	}

	if(mEngine->getAudioDevice()!=NULL){
		mAudio=Audio::ptr(mEngine->getAudioDevice()->createAudio());
	}
	if(mAudio!=NULL){
		mAudio->create(audioBuffer);
		return true;
	}
	else{
		return false;
	}
}

bool AudioNode::setAudioStream(const String &name){
	return setAudioStream(mEngine->getAudioBufferManager()->findAudioStream(name));
}

bool AudioNode::setAudioStream(AudioStream::ptr stream){
	if(mAudio!=NULL && mAudio->getAudioStream()==stream){
		return true;
	}

	if(mAudio!=NULL){
		mAudio->destroy();
	}

	mAudio=Audio::ptr(mEngine->getAudioDevice()->createAudio());
	if(mAudio!=NULL){
		mAudio->create(stream);
		return true;
	}
	else{
		return false;
	}
}

}
}
}
