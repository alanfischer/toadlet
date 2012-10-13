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

AudioComponent::AudioComponent(Engine *engine):BaseComponent(),
	mGain(Math::ONE),
	mPitch(Math::ONE),
	mRolloff(0),
	mLooping(false),
	mGlobal(false)
{
	mEngine=engine;
}

void AudioComponent::destroy(){
	if(mAudio!=NULL){
		mAudio->destroy();
		mAudio=NULL;
	}

	BaseComponent::destroy();
}

bool AudioComponent::setAudioBuffer(const String &name,int track){
	return setAudioBuffer(mEngine->getAudioManager()->findAudioBuffer(name,track));
}

bool AudioComponent::setAudioBuffer(AudioBuffer *audioBuffer){
	if(mAudio!=NULL && mAudio->getAudioBuffer()==audioBuffer){
		return true;
	}

	if(mAudio!=NULL){
		mAudio->destroy();
	}

	TOADLET_TRY
		mAudio=mEngine->getAudioManager()->createAudio();
	TOADLET_CATCH(const Exception &ex){ex;
		mAudio=NULL;
	}
	if(mAudio!=NULL){
		mAudio->create(audioBuffer);

		setAudioParameters();

		return true;
	}
	else{
		return false;
	}
}

bool AudioComponent::setAudioStream(const String &name,int track){
	return setAudioStream(mEngine->getAudioManager()->findAudioStream(name,track));
}

bool AudioComponent::setAudioStream(AudioStream *stream){
	if(mAudio!=NULL && mAudio->getAudioStream()==stream){
		return true;
	}

	if(mAudio!=NULL){
		mAudio->destroy();
	}

	TOADLET_TRY
		mAudio=mEngine->getAudioManager()->createAudio();
	TOADLET_CATCH_ANONYMOUS(){
		mAudio=NULL;
	}
	if(mAudio!=NULL){
		mAudio->create(stream);

		setAudioParameters();

		return true;
	}
	else{
		return false;
	}
}

void AudioComponent::frameUpdate(int dt,int scope){
	setAudioFromTransform();
}

void AudioComponent::setAudioFromTransform(){
	if(mAudio!=NULL){
		mAudio->setPosition(mParent->getWorldTranslate());
	}
}

void AudioComponent::setAudioParameters(){
	if(mAudio!=NULL){
		mAudio->setGain(mGain);
		mAudio->setPitch(mPitch);
		mAudio->setRolloffFactor(mRolloff);
		mAudio->setLooping(mLooping);
		mAudio->setGlobal(mGlobal);
	}
}

}
}
