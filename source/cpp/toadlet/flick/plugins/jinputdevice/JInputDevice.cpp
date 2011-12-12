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

#include "JInputDevice.h"
#include <toadlet/egg/Logger.h>

extern "C" JNIEXPORT void Java_us_toadlet_flick(JNIEnv *env);

namespace toadlet{
namespace flick{

TOADLET_C_API InputDevice* new_JInputDevice(JNIEnv *jenv,jobject jobj){
	return new JInputDevice(jenv,jobj);
}

JInputDevice::JInputDevice(JNIEnv *jenv,jobject jobj){
	env=jenv;
	obj=env->NewGlobalRef(jobj);

	jclass deviceClass=env->GetObjectClass(obj);
	{
		createID=env->GetMethodID(deviceClass,"create","()Z");
		destroyID=env->GetMethodID(deviceClass,"destroy","()V");

		getTypeID=env->GetMethodID(deviceClass,"getType","()I");
		startID=env->GetMethodID(deviceClass,"start","()Z");
		updateID=env->GetMethodID(deviceClass,"update","(I)V");
		stopID=env->GetMethodID(deviceClass,"stop","()V");

		setListenerID=env->GetMethodID(deviceClass,"setListener","(Lus/toadlet/flick/InputDeviceListener;)V");
		setSampleTimeID=env->GetMethodID(deviceClass,"setSampleTime","(I)V");
		setAlphaID=env->GetMethodID(deviceClass,"setAlpha","(F)V");
	}
	env->DeleteLocalRef(deviceClass);

	if(env->ExceptionOccurred()!=0){
		env->ExceptionDescribe();
		env->ExceptionClear();
		return;
	}

	Java_us_toadlet_flick(env);
}

JInputDevice::~JInputDevice(){
	env->DeleteGlobalRef(obj);
	obj=NULL;
	env=NULL;
}

bool JInputDevice::create(){
	Logger::alert(Categories::TOADLET_RIBBIT,
		"creating JInputDevice");

	return env->CallBooleanMethod(obj,createID);
}

void JInputDevice::destroy(){
	Logger::alert(Categories::TOADLET_RIBBIT,
		"destroying JInputDevice");

	return env->CallVoidMethod(obj,destroyID);
}

InputDevice::InputType JInputDevice::getType(){
	return (InputType)env->CallIntMethod(obj,getTypeID);
}

bool JInputDevice::start(){
	return env->CallBooleanMethod(obj,startID);
}

void JInputDevice::update(int dt){
	env->CallVoidMethod(obj,updateID,dt);
}

void JInputDevice::stop(){
	env->CallVoidMethod(obj,stopID);
}

void JInputDevice::setListener(InputDeviceListener *listener){
	jobject listenerObj=NULL;

	jclass listenerClass=env->FindClass("us/toadlet/flick/NInputDeviceListener");
	{
		jmethodID initID=env->GetMethodID(listenerClass,"<init>","(I)V");
		listenerObj=env->NewObject(listenerClass,initID,(int)listener);
	}
	env->DeleteLocalRef(listenerClass);
	
	jthrowable exc=env->ExceptionOccurred();
	if(exc!=NULL){
		env->ExceptionDescribe();
		env->ExceptionClear();
		return;
	}
	
	env->CallVoidMethod(obj,setListenerID,listenerObj);
}

void JInputDevice::setSampleTime(int dt){
	env->CallVoidMethod(obj,setSampleTimeID,dt);
}

void JInputDevice::setAlpha(scalar alpha){
	env->CallVoidMethod(obj,setAlphaID,Math::toFloat(alpha));
}

}
}


