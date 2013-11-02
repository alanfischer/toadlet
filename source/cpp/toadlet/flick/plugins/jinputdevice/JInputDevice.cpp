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

extern "C" JNIEXPORT void Java_us_toadlet_flick(JNIEnv *env);

namespace toadlet{
namespace flick{

TOADLET_C_API InputDevice* new_JInputDevice(JNIEnv *jenv,jobject jobj){
	return new JInputDevice(jenv,jobj);
}

JInputDevice::JInputDevice(JNIEnv *jenv,jobject jobj):
	vm(NULL),
	obj(NULL)
{
	jenv->GetJavaVM(&vm);
	JNIEnv *env=jenv;
	obj=env->NewGlobalRef(jobj);

	jclass oclass=env->GetObjectClass(obj);
	{
		createID=env->GetMethodID(oclass,"create","()Z");
		destroyID=env->GetMethodID(oclass,"destroy","()V");

		getTypeID=env->GetMethodID(oclass,"getType","()I");
		startID=env->GetMethodID(oclass,"start","()Z");
		updateID=env->GetMethodID(oclass,"update","(I)V");
		stopID=env->GetMethodID(oclass,"stop","()V");

		setListenerID=env->GetMethodID(oclass,"setListener","(Lus/toadlet/flick/InputDeviceListener;)V");
		setSampleTimeID=env->GetMethodID(oclass,"setSampleTime","(I)V");
		setAlphaID=env->GetMethodID(oclass,"setAlpha","(F)V");
	}
	env->DeleteLocalRef(oclass);

	if(env->ExceptionOccurred()!=0){
		env->ExceptionDescribe();
		env->ExceptionClear();
		return;
	}
}

JInputDevice::~JInputDevice(){
	getEnv()->DeleteGlobalRef(obj);
	obj=NULL;
	vm=NULL;
}

bool JInputDevice::create(){
	Log::alert(Categories::TOADLET_RIBBIT,
		"creating JInputDevice");

	return getEnv()->CallBooleanMethod(obj,createID);
}

void JInputDevice::destroy(){
	Log::alert(Categories::TOADLET_RIBBIT,
		"destroying JInputDevice");

	return getEnv()->CallVoidMethod(obj,destroyID);
}

InputDevice::InputType JInputDevice::getType(){
	return (InputType)getEnv()->CallIntMethod(obj,getTypeID);
}

bool JInputDevice::start(){
	return getEnv()->CallBooleanMethod(obj,startID);
}

void JInputDevice::update(int dt){
	getEnv()->CallVoidMethod(obj,updateID,dt);
}

void JInputDevice::stop(){
	getEnv()->CallVoidMethod(obj,stopID);
}

void JInputDevice::setListener(InputDeviceListener *listener){
	JNIEnv *env=getEnv();

	jobject listenerObj=NULL;
	if(listener!=NULL){
		jclass listenerClass=env->FindClass("us/toadlet/flick/BaseInputDeviceListener");
		{
			jmethodID initID=env->GetMethodID(listenerClass,"<init>","(JZ)V");
			listenerObj=env->NewObject(listenerClass,initID,(jlong)listener,false);
		}
		env->DeleteLocalRef(listenerClass);
		
		jthrowable exc=env->ExceptionOccurred();
		if(exc!=NULL){
			env->ExceptionDescribe();
			env->ExceptionClear();
			return;
		}
	}	
	env->CallVoidMethod(obj,setListenerID,listenerObj);
}

void JInputDevice::setSampleTime(int dt){
	getEnv()->CallVoidMethod(obj,setSampleTimeID,dt);
}

void JInputDevice::setAlpha(scalar alpha){
	getEnv()->CallVoidMethod(obj,setAlphaID,Math::toFloat(alpha));
}

JNIEnv *JInputDevice::getEnv() const{
	JNIEnv *env=NULL;
	vm->AttachCurrentThread(&env,NULL);
	return env;
}

}
}


