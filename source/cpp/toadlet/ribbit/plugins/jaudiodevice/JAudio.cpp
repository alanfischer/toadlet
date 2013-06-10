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

namespace toadlet{
namespace ribbit{

JAudio::JAudio(JNIEnv *jenv,jobject jobj):
	vm(NULL),
	obj(NULL)
{
	jenv->GetJavaVM(&vm);
	JNIEnv *env=getEnv();
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
		getGainID=env->GetMethodID(audioClass,"getGain","()F");
	}
	env->DeleteLocalRef(audioClass);
}

JAudio::~JAudio(){
	getEnv()->DeleteGlobalRef(obj);
	obj=NULL;
	vm=NULL;
}

bool JAudio::create(AudioBuffer *audioBuffer){
	mAudioBuffer=audioBuffer; // Store the pointer until we have reference counting
	jobject audioBufferObj=(audioBuffer!=NULL)?((JAudioBuffer*)audioBuffer->getRootAudioBuffer())->getJObject():NULL;

	return getEnv()->CallBooleanMethod(obj,createAudioBufferID,audioBufferObj);
}

bool JAudio::create(AudioStream *stream){
	JNIEnv *env=getEnv();

	Log::debug(Categories::TOADLET_RIBBIT,
		"JAudio::create");

	mAudioStream=stream; // Store the pointer until we have reference counting
	jobject streamObj=NULL;

	jclass streamClass=env->FindClass("us/toadlet/ribbit/AudioStream");
	if(stream!=NULL){
		jmethodID initID=env->GetMethodID(streamClass,"<init>","(JZ)V");
		stream->retain();
		streamObj=env->NewObject(streamClass,initID,(jlong)stream,true);
	}
	env->DeleteLocalRef(streamClass);

	jthrowable exc=env->ExceptionOccurred();
	if(exc!=NULL){
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}

	return env->CallBooleanMethod(obj,createAudioStreamID,streamObj);
}

void JAudio::destroy(){
	getEnv()->CallVoidMethod(obj,destroyID);
}

bool JAudio::play(){
	return getEnv()->CallBooleanMethod(obj,playID);
}

bool JAudio::stop(){
	return getEnv()->CallBooleanMethod(obj,stopID);
}

bool JAudio::getPlaying() const{
	return getEnv()->CallBooleanMethod(obj,getPlayingID);
}

bool JAudio::getFinished() const{
	return getEnv()->CallBooleanMethod(obj,getFinishedID);
}

void JAudio::setLooping(bool looping){
	getEnv()->CallVoidMethod(obj,setLoopingID,looping);
}

bool JAudio::getLooping() const{
	return getEnv()->CallBooleanMethod(obj,getLoopingID);
}

void JAudio::setGain(scalar gain){
	getEnv()->CallVoidMethod(obj,setGainID,(float)gain);
}

scalar JAudio::getGain() const{
	return getEnv()->CallFloatMethod(obj,getGainID);
}

JNIEnv *JAudio::getEnv() const{
	JNIEnv *env=NULL;
	vm->AttachCurrentThread(&env,NULL);
	return env;
}

}
}
