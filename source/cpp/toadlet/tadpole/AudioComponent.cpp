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
#include <toadlet/tadpole/PhysicsComponent.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Scene.h>

namespace toadlet{
namespace tadpole{

AudioComponent::AudioComponent(Scene *scene):BaseComponent(),
	mTime(0),
	mGain(Math::ONE),
	mPitch(Math::ONE),
	mRolloff(0),
	mLooping(false),
	mGlobal(false)
{
	mScene=scene;
	mEngine=mScene->getEngine();
	mBound=new Bound(Sphere(2));
}

void AudioComponent::destroy(){
	mAudioBuffer=NULL;
	mAudioStream=NULL;

	if(mAudio!=NULL){
		mAudio->destroy();
		mAudio=NULL;
	}

	BaseComponent::destroy();
}

bool AudioComponent::setAudioBuffer(const String &name,int track){
	return mEngine->getAudioManager()->findAudioBuffer(name,this,track);
}

bool AudioComponent::setAudioBuffer(AudioBuffer *audioBuffer){
	if(mAudio!=NULL && mAudio->getAudioBuffer()==audioBuffer){
		return true;
	}

	if(mAudio!=NULL){
		mAudio->destroy();
	}

	mAudioStream=NULL;
	mAudioBuffer=audioBuffer;

	if(mScene->getCreateAudio()){
		TOADLET_TRY
			mAudio=mEngine->getAudioManager()->createAudio();
		TOADLET_CATCH_ANONYMOUS(){
			mAudio=NULL;
		}
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
	return mEngine->getAudioManager()->findAudioStream(name,this,track);
}

bool AudioComponent::setAudioStream(AudioStream *audioStream){
	if(mAudio!=NULL && mAudio->getAudioStream()==audioStream){
		return true;
	}

	if(mAudio!=NULL){
		mAudio->destroy();
	}

	mAudioBuffer=NULL;
	mAudioStream=audioStream;

	if(mScene->getCreateAudio()){
		TOADLET_TRY
			mAudio=mEngine->getAudioManager()->createAudio();
		TOADLET_CATCH_ANONYMOUS(){
			mAudio=NULL;
		}
	}

	if(mAudio!=NULL){
		mAudio->create(audioStream);

		setAudioParameters();

		return true;
	}
	else{
		return false;
	}
}

bool AudioComponent::play(){
	if(mAudio!=NULL){
		return mAudio->play();
	}
	
	mTime=0;
	return false;
}

bool AudioComponent::stop(){
	if(mAudio!=NULL){
		return mAudio->stop();
	}
	
	mTime=-1;
	return false;
}

bool AudioComponent::getPlaying() const{
	if(mAudio!=NULL){
		return mAudio->getPlaying();
	}
	
	if(mAudioBuffer!=NULL){
		if(mTime>=0 && mTime<mAudioBuffer->getLengthTime()){
			return true;
		}
	}

	return false;
}

bool AudioComponent::getFinished() const{
	if(mAudio!=NULL){
		return mAudio->getFinished();
	}

	if(mAudioBuffer!=NULL){
		if(mTime>=0 && mTime<mAudioBuffer->getLengthTime()){
			return false;
		}
	}
	
	return true;
}

void AudioComponent::frameUpdate(int dt,int scope){
	setAudioFromTransform();

	setAudioFromVelocity();

	if(mAudio==NULL && mAudioBuffer!=NULL && mTime>=0){
		mTime+=dt;
	}
}

void AudioComponent::streamReady(Stream *stream){
	setAudioStream((AudioStream*)stream);
}

void AudioComponent::resourceReady(Resource *resource){
	setAudioBuffer((AudioBuffer*)resource);
}

void AudioComponent::setAudioFromTransform(){
	if(mAudio!=NULL){
		mAudio->setPosition(mParent->getWorldTranslate());
	}
}

void AudioComponent::setAudioFromVelocity(){
	if(mAudio!=NULL && mParent->getPhysics()!=NULL){
		mAudio->setVelocity(mParent->getPhysics()->getVelocity());
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
