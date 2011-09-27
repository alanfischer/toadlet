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

#include "JATAudio.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>

namespace toadlet{
namespace ribbit{

JATAudio::JATAudio(JNIEnv *jenv,jobject jobj){
	env=jenv;
	obj=jobj;
}

JATAudio::~JATAudio(){
	destroy();
}

bool JATAudio::create(AudioBuffer::ptr audioBuffer){
	mAudioBuffer=audioBuffer;

	jobject jbuffer=((JATAudioBuffer*)audioBuffer->getRootAudioBuffer())->getNativeHandle();

	return obj.create(jbuffer);
}

bool JATAudio::create(AudioStream::ptr audioStream){
	mAudioStream=audioStream;

	jobject jstream=create NAudioStream(audioStream);

	return obj.create(jstream);
}

void JATAudio::destroy(){
	obj.destroy();
}

bool JATAudio::play(){
	return obj.play();
}

bool JATAudio::stop(){
	return obj.stop();
}

bool JATAudio::getPlaying() const{
	return obj.getPlaying();
}

bool JATAudio::getFinished() const{
	return obj.getFinished();
}

void JATAudio::setLooping(bool looping){
	obj.setLooping(looping);
}

bool JATAudio::getLooping() const{
	return obj.getLooping();
}

void JATAudio::setGain(scalar gain){
	obj.setGain(gain);
}

void JATAudio::fadeToGain(scalar gain,int time){
	obj.fadeToGain(gain,time);
}

scalar JATAudio::getGain() const{
	return obj.getGain();
}

}
}
