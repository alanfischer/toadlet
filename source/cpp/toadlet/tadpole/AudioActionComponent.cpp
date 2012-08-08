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

#include <toadlet/tadpole/AudioActionComponent.h>

namespace toadlet{
namespace tadpole{

AudioActionComponent::AudioActionComponent(const String &name):BaseComponent(name){}

bool AudioActionComponent::parentChanged(Node *node){
	if(mParent!=NULL){
		mParent->actionRemoved(this);
	}

	BaseComponent::parentChanged(node);

	if(mParent!=NULL){
		mParent->actionAttached(this);
	}

	return true;
}

void AudioActionComponent::setAudio(AudioComponent *audio){
	mAudio=audio;
}

void AudioActionComponent::setAudioStream(const String &stream){
	mAudioStream=stream;
}

void AudioActionComponent::start(){
	mAudio->setAudioStream(mAudioStream);
	mAudio->play();
}

void AudioActionComponent::stop(){
	mAudio->stop();
}

}
}
