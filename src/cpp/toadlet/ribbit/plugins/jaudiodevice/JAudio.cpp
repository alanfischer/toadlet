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
	obj=env->NewGlobalRef(jobj);

	jclass audioClass=env->GetObjectClass(obj);
	{
		createAudioBufferID=env->GetMethodID(audioClass,"create","(Lus/toadlet/ribbit/AudioBuffer;)Z");
		createAudioStreamID=env->GetMethodID(audioClass,"create","(Lus/toadlet/ribbit/AudioStream;)Z");
		destroyID=env->GetMethodID(audioClass,"destroy","()V");

		playID=env->GetMethodID(audioClass,"play","()Z");
		stopID=env->GetMethodID(audioClass,"stop","()Z");

		getPlayingID=env->GetMethodID(audioClass,"getPlaying","()Z");
		getFinishedID=env->GetMethodID(audioClass,"getFinished","()Z");

		setLoopingID=env->GetMethodID(audioClass,"setLooping","(Z)V");
		getLoopingID=env->GetMethodID(audioClass,"getLooping","()Z");

		setGainID=env->GetMethodID(audioClass,"setGain","(F)V");
		fadeToGainID=env->GetMethodID(audioClass,"fadeToGain","(FI)V");
		getGainID=env->GetMethodID(audioClass,"getGain","()F");
	}
	env->DeleteLocalRef(audioClass);
}

JAudio::~JAudio(){
	env->DeleteGlobalRef(obj);
	obj=NULL;
	env=NULL;
}

bool JAudio::create(AudioBuffer::ptr audioBuffer){
	Logger::alert(Categories::TOADLET_RIBBIT,
		"JAudio::create");

	mAudioBuffer=audioBuffer; // Store the pointer until we have reference counting
	jobject audioBufferObj=(audioBuffer!=NULL)?((JAudioBuffer*)audioBuffer->getRootAudioBuffer())->getJObject():NULL;

	return env->CallBooleanMethod(obj,createAudioBufferID,audioBufferObj);
}

bool JAudio::create(AudioStream::ptr audioStream){
	Logger::alert(Categories::TOADLET_RIBBIT,
		"JAudio::create");

	mAudioStream=audioStream; // Store the pointer until we have reference counting
	jobject audioStreamObj=NULL;
	jclass streamClass=env->FindClass("us/toadlet/ribbit/NAudioStream");
	if(audioStream!=NULL){
		jmethodID initID=env->GetMethodID(streamClass,"<init>","(III)V");
		int length=2048;
		tbyte *buffer=new tbyte[length];
		audioStreamObj=env->NewObject(streamClass,initID,(int)audioStream.get(),(int)buffer,length);
	}
	env->DeleteLocalRef(streamClass);

	jthrowable exc=env->ExceptionOccurred();
	if(exc!=NULL){
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}

	return env->CallBooleanMethod(obj,createAudioStreamID,audioStreamObj);
}

void JAudio::destroy(){
	env->CallVoidMethod(obj,destroyID);
}

bool JAudio::play(){
	return env->CallBooleanMethod(obj,playID);
}

bool JAudio::stop(){
	return env->CallBooleanMethod(obj,stopID);
}

bool JAudio::getPlaying() const{
	return env->CallBooleanMethod(obj,getPlayingID);
}

bool JAudio::getFinished() const{
	return env->CallBooleanMethod(obj,getFinishedID);
}

void JAudio::setLooping(bool looping){
	env->CallVoidMethod(obj,setLoopingID,looping);
}

bool JAudio::getLooping() const{
	return env->CallBooleanMethod(obj,getLoopingID);
}

void JAudio::setGain(scalar gain){
	env->CallVoidMethod(obj,setGainID,(float)gain);
}

void JAudio::fadeToGain(scalar gain,int time){
	env->CallVoidMethod(obj,fadeToGainID,(float)gain,time);
}

scalar JAudio::getGain() const{
	return env->CallFloatMethod(obj,getGainID);
}

}
}
