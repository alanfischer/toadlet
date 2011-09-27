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

#include "JAudio.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>

namespace toadlet{
namespace ribbit{

JAudio::JAudio(JNIEnv *jenv,jobject jobj){
	env=jenv;
	obj=jobj;
}

JAudio::~JAudio(){
	destroy();
}

bool JAudio::create(AudioBuffer::ptr audioBuffer){
	mAudioBuffer=audioBuffer;

	jobject jbuffer=((JAudioBuffer*)audioBuffer->getRootAudioBuffer())->getNativeHandle();

	return obj.create(jbuffer);
}

bool JAudio::create(AudioStream::ptr audioStream){
	mAudioStream=audioStream;

	jobject jstream=create NAudioStream(audioStream);

	return obj.create(jstream);
}

void JAudio::destroy(){
	obj.destroy();
}

bool JAudio::play(){
	return obj.play();
}

bool JAudio::stop(){
	return obj.stop();
}

bool JAudio::getPlaying() const{
	return obj.getPlaying();
}

bool JAudio::getFinished() const{
	return obj.getFinished();
}

void JAudio::setLooping(bool looping){
	obj.setLooping(looping);
}

bool JAudio::getLooping() const{
	return obj.getLooping();
}

void JAudio::setGain(scalar gain){
	obj.setGain(gain);
}

void JAudio::fadeToGain(scalar gain,int time){
	obj.fadeToGain(gain,time);
}

scalar JAudio::getGain() const{
	return obj.getGain();
}

}
}
