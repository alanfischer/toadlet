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

namespace toadlet{
namespace ribbit{

TOADLET_C_API AudioDevice* new_JAudioDevice(JNIEnv *jenv,jobject jobj){
	return new JAudioDevice(jenv,jobj);
}

JAudioDevice::JAudioDevice(JNIEnv *jenv,jobject jobj):
	vm(NULL),
	obj(NULL)
{
	jenv->GetJavaVM(&vm);
	JNIEnv *env=getEnv();
	obj=env->NewGlobalRef(jobj);

	jclass oclass=env->GetObjectClass(obj);
	{
		createID=env->GetMethodID(oclass,"create","()Z");
		destroyID=env->GetMethodID(oclass,"destroy","()V");

		createAudioBufferID=env->GetMethodID(oclass,"createAudioBuffer","()Lus/toadlet/ribbit/AudioBuffer;");
		createAudioID=env->GetMethodID(oclass,"createAudio","()Lus/toadlet/ribbit/Audio;");
		updateID=env->GetMethodID(oclass,"update","(I)V");
	}
	env->DeleteLocalRef(oclass);

	jthrowable exc=env->ExceptionOccurred();
	if(exc!=NULL){
		env->ExceptionDescribe();
		env->ExceptionClear();
	}
}

JAudioDevice::~JAudioDevice(){
	getEnv()->DeleteGlobalRef(obj);
	obj=NULL;
	vm=NULL;
}

bool JAudioDevice::create(int *options){
	Log::alert(Categories::TOADLET_RIBBIT,
		"creating JAudioDevice");

	return getEnv()->CallBooleanMethod(obj,createID);
}

void JAudioDevice::destroy(){
	Log::alert(Categories::TOADLET_RIBBIT,
		"destroying JAudioDevice");

	return getEnv()->CallVoidMethod(obj,destroyID);
}

AudioBuffer *JAudioDevice::createAudioBuffer(){
	jobject audioBufferObj=getEnv()->CallObjectMethod(obj,createAudioBufferID);

	return new JAudioBuffer(getEnv(),audioBufferObj);
}

Audio *JAudioDevice::createAudio(){
	jobject audioObj=getEnv()->CallObjectMethod(obj,createAudioID);

	return new JAudio(getEnv(),audioObj);
}

void JAudioDevice::update(int dt){
	return getEnv()->CallVoidMethod(obj,updateID,dt);
}

JNIEnv *JAudioDevice::getEnv() const{
	JNIEnv *env=NULL;
	vm->AttachCurrentThread(&env,NULL);
	return env;
}

}
}
