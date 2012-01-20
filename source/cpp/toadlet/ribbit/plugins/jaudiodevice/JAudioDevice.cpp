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

#include "JAudioDevice.h"
#include "JAudio.h"
#include "JAudioBuffer.h"
#include <toadlet/egg/Logger.h>

namespace toadlet{
namespace ribbit{

TOADLET_C_API AudioDevice* new_JAudioDevice(JNIEnv *jenv,jobject jobj){
	return new JAudioDevice(jenv,jobj);
}

JAudioDevice::JAudioDevice(JNIEnv *jenv,jobject jobj){
	env=jenv;
	obj=env->NewGlobalRef(jobj);

	jclass deviceClass=env->GetObjectClass(obj);
	{
		createID=env->GetMethodID(deviceClass,"create","()Z");
		destroyID=env->GetMethodID(deviceClass,"destroy","()V");

		createAudioBufferID=env->GetMethodID(deviceClass,"createAudioBuffer","()Lus/toadlet/ribbit/AudioBuffer;");
		createAudioID=env->GetMethodID(deviceClass,"createAudio","()Lus/toadlet/ribbit/Audio;");
		updateID=env->GetMethodID(deviceClass,"update","(I)V");
	}
	env->DeleteLocalRef(deviceClass);

	jthrowable exc=env->ExceptionOccurred();
	if(exc!=NULL){
		env->ExceptionDescribe();
		env->ExceptionClear();
	}
}

JAudioDevice::~JAudioDevice(){
	env->DeleteGlobalRef(obj);
	obj=NULL;
	env=NULL;
}

bool JAudioDevice::create(int *options){
	Logger::alert(Categories::TOADLET_RIBBIT,
		"creating JAudioDevice");

	return env->CallBooleanMethod(obj,createID);
}

void JAudioDevice::destroy(){
	Logger::alert(Categories::TOADLET_RIBBIT,
		"destroying JAudioDevice");

	return env->CallVoidMethod(obj,destroyID);
}

AudioBuffer *JAudioDevice::createAudioBuffer(){
	jobject audioBufferObj=env->CallObjectMethod(obj,createAudioBufferID);

	return new JAudioBuffer(env,audioBufferObj);
}

Audio *JAudioDevice::createAudio(){
	jobject audioObj=env->CallObjectMethod(obj,createAudioID);

	return new JAudio(env,audioObj);
}

void JAudioDevice::update(int dt){
	return env->CallVoidMethod(obj,updateID,dt);
}

}
}
