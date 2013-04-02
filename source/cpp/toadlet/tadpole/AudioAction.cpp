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

#include <toadlet/tadpole/AudioAction.h>

namespace toadlet{
namespace tadpole{

AudioAction::AudioAction(AudioComponent *audio):
	//mListeners,
	mAudio(audio),
	//mAudioStream,
	mRunning(false)
{
}

void AudioAction::setAudio(AudioComponent *audio){
	mAudio=audio;
}

void AudioAction::start(){
	if(mAudio!=NULL){
		mAudio->play();
	}

	mRunning=true;

	int i;
	for(i=0;i<mListeners.size();++i){
		mListeners[i]->actionStarted(this);
	}
}

void AudioAction::stop(){
	if(mAudio!=NULL){
		mAudio->stop();
	}

	mRunning=false;

	int i;
	for(i=0;i<mListeners.size();++i){
		mListeners[i]->actionStopped(this);
	}
}

void AudioAction::update(int dt){
	if(mRunning==false){
		return;
	}

	if(mRunning && mAudio->getFinished()){
		stop();
	}
}

}
}
