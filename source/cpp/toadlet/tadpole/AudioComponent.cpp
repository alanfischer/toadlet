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

#include <toadlet/tadpole/AudioComponent.h>
#include <toadlet/tadpole/Engine.h>

namespace toadlet{
namespace tadpole{

AudioComponent::AudioComponent(Engine *engine):BaseComponent(){
	mEngine=engine;
}

void AudioComponent::destroy(){
	if(mAudio!=NULL){
		mAudio->destroy();
		mAudio=NULL;
	}

	BaseComponent::destroy();
}

bool AudioComponent::setAudioBuffer(const String &name){
	return setAudioBuffer(mEngine->getAudioManager()->findAudioBuffer(name));
}

bool AudioComponent::setAudioBuffer(AudioBuffer *audioBuffer){
	if(mAudio!=NULL && mAudio->getAudioBuffer()==audioBuffer){
		return true;
	}

	if(mAudio!=NULL){
		mAudio->destroy();
	}

	mAudio=mEngine->getAudioManager()->createAudio();
	if(mAudio!=NULL){
		mAudio->create(audioBuffer);
		return true;
	}
	else{
		return false;
	}
}

bool AudioComponent::setAudioStream(const String &name){
	return setAudioStream(mEngine->getAudioManager()->findAudioStream(name));
}

bool AudioComponent::setAudioStream(AudioStream *stream){
	if(mAudio!=NULL && mAudio->getAudioStream()==stream){
		return true;
	}

	if(mAudio!=NULL){
		mAudio->destroy();
	}

	mAudio=mEngine->getAudioManager()->createAudio();
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
