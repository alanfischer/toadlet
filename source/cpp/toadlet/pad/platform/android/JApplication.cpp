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

#include "JApplication.h"
#include <toadlet/egg/platform/android/JStream.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::ribbit;
using namespace toadlet::flick;
using namespace toadlet::tadpole;
using namespace toadlet::pad;

TOADLET_C_API AudioDevice* new_JAudioDevice(JNIEnv *env,jobject obj);
TOADLET_C_API InputDevice* new_JInputDevice(JNIEnv *env,jobject obj);

namespace toadlet{
namespace pad{

JApplication::JApplication(JNIEnv *jenv,jobject jobj):
	vm(NULL),
	obj(NULL),

	mEngine(NULL),
	mLastEngineObj(NULL),
	mRenderDevice(NULL),
	mLastRenderDeviceObj(NULL),
	mAudioDevice(NULL),
	mLastAudioDeviceObj(NULL)
{
	int i;
	for(i=0;i<InputDevice::InputType_MAX;++i){
		mInputDevices[i]=NULL;
		mLastInputDeviceObjs[i]=NULL;
	}

	jenv->GetJavaVM(&vm);
	JNIEnv *env=jenv;
	obj=env->NewGlobalRef(jobj);

	jclass oclass=env->GetObjectClass(obj);
	{
		createID=env->GetMethodID(oclass,"create","()Z");
		destroyID=env->GetMethodID(oclass,"destroy","()V");
		startID=env->GetMethodID(oclass,"start","()V");
		stopID=env->GetMethodID(oclass,"stop","()V");
		isRunningID=env->GetMethodID(oclass,"isRunning","()Z");
		setTitleID=env->GetMethodID(oclass,"setTitle","(Ljava/lang/CharSequence;)V");
		getTitleID=env->GetMethodID(oclass,"getTitle","()Ljava/lang/CharSequence;");
		getWidthID=env->GetMethodID(oclass,"getWidth","()I");
		getHeightID=env->GetMethodID(oclass,"getHeight","()I");
		setDifferenceMouseID=env->GetMethodID(oclass,"setDifferenceMouse","(Z)V");
		getDifferenceMouseID=env->GetMethodID(oclass,"getDifferenceMouse","()Z");
		getEngineID=env->GetMethodID(oclass,"getEngine","()Lus/toadlet/tadpole/Engine;");
		getRenderDeviceID=env->GetMethodID(oclass,"getRenderDevice","()Lus/toadlet/peeper/RenderDevice;");
		getAudioDeviceID=env->GetMethodID(oclass,"getAudioDevice","()Lus/toadlet/ribbit/AudioDevice;");
		getInputDeviceID=env->GetMethodID(oclass,"getInputDevice","(I)Lus/toadlet/flick/InputDevice;");
	}
	env->DeleteLocalRef(oclass);

	oclass=env->FindClass("us/toadlet/tadpole/Engine");
	{
		Engine_nativeHandle=env->GetFieldID(oclass,"swigCPtr","J");
	}
	env->DeleteLocalRef(oclass);
	
	oclass=env->FindClass("us/toadlet/peeper/RenderDevice");
	{
		RenderDevice_nativeHandle=env->GetFieldID(oclass,"swigCPtr","J");
	}
	env->DeleteLocalRef(oclass);
}

JApplication::~JApplication(){
	JNIEnv *env=getEnv();

	if(obj!=NULL){
		env->DeleteGlobalRef(obj);
		obj=NULL;
	}

	if(mLastEngineObj!=NULL){
		env->DeleteGlobalRef(mLastEngineObj);
		mLastEngineObj=NULL;
	}

	if(mLastRenderDeviceObj!=NULL){
		env->DeleteGlobalRef(mLastRenderDeviceObj);
		mLastRenderDeviceObj=NULL;
	}

	if(mLastAudioDeviceObj!=NULL){
		env->DeleteGlobalRef(mLastAudioDeviceObj);
		mLastAudioDeviceObj=NULL;
	}

	env=NULL;
}

bool JApplication::create(String renderDevice,String audioDevice){
	return getEnv()->CallBooleanMethod(obj,createID);
}

void JApplication::destroy(){
	getEnv()->CallVoidMethod(obj,destroyID);
}

void JApplication::start(){
	getEnv()->CallVoidMethod(obj,startID);
}

void JApplication::stop(){
	getEnv()->CallVoidMethod(obj,stopID);
}

bool JApplication::isRunning() const{
	getEnv()->CallBooleanMethod(obj,isRunningID);
}

void JApplication::setTitle(const String &title){
// Disabled since calling it enough calls a crash in Window.setTitle for some reason
#if 0
	JNIEnv *env=getEnv();

	jstring titleObj=env->NewStringUTF(title);
	if(env->ExceptionOccurred()!=NULL){
		env->ExceptionDescribe();
		env->ExceptionClear();
		return;
	}
	env->CallVoidMethod(obj,setTitleID,titleObj);
	env->DeleteLocalRef(titleObj);
#endif
}

String JApplication::getTitle() const{
	JNIEnv *env=getEnv();

	jstring titleObj=(jstring)env->CallObjectMethod(obj,getTitleID);
	const char *chars=env->GetStringUTFChars(titleObj,NULL);
	String title=chars;
	env->ReleaseStringUTFChars(titleObj,chars);
	return title;
}

int JApplication::getWidth() const{
	getEnv()->CallIntMethod(obj,getWidthID);
}

int JApplication::getHeight() const{
	getEnv()->CallIntMethod(obj,getHeightID);
}

void JApplication::setDifferenceMouse(bool difference){
	getEnv()->CallVoidMethod(obj,setDifferenceMouseID,difference);
}

bool JApplication::getDifferenceMouse() const{
	getEnv()->CallBooleanMethod(obj,getDifferenceMouseID);
}

Engine *JApplication::getEngine() const{
	JNIEnv *env=getEnv();

	jobject engineObj=env->CallObjectMethod(obj,getEngineID);

	if(mEngine==NULL || mLastEngineObj!=engineObj){
		mEngine=engineObj!=NULL?(Engine*)env->GetLongField(engineObj,Engine_nativeHandle):NULL;

		if(mLastEngineObj!=NULL){
			env->DeleteGlobalRef(mLastEngineObj);
		}
		mLastEngineObj=env->NewGlobalRef(engineObj);
		
		env->DeleteLocalRef(engineObj);
	}
	
	return mEngine;
}

RenderDevice *JApplication::getRenderDevice() const{
	JNIEnv *env=getEnv();

	jobject deviceObj=env->CallObjectMethod(obj,getRenderDeviceID);

	if(mRenderDevice==NULL || mLastRenderDeviceObj!=deviceObj){
		//if(jobject is NRenderDevice){
			mRenderDevice=deviceObj!=NULL?(RenderDevice*)env->GetLongField(deviceObj,RenderDevice_nativeHandle):NULL;
		//}
		//else{
		//	mRenderDevice=new JRenderDevice(env,deviceObj);
		//

		if(mLastRenderDeviceObj!=NULL){
			env->DeleteGlobalRef(mLastRenderDeviceObj);
		}
		mLastRenderDeviceObj=env->NewGlobalRef(deviceObj);
		
		env->DeleteLocalRef(deviceObj);
	}
	
	return mRenderDevice;
}

AudioDevice *JApplication::getAudioDevice() const{
	JNIEnv *env=getEnv();

	jobject deviceObj=env->CallObjectMethod(obj,getAudioDeviceID);

	if(mAudioDevice==NULL || mLastAudioDeviceObj!=deviceObj){
		//if(jobject is NAudioDevice){
		//	mAudioDevice=deviceObj!=NULL?(AudioDevice*)env->GetIntField(deviceObj,audioDeviceNativeHandleID):NULL;
		//}
		//else{
			mAudioDevice=deviceObj!=NULL?new_JAudioDevice(env,deviceObj):NULL;
		//

		if(mLastAudioDeviceObj!=NULL){
			env->DeleteGlobalRef(mLastAudioDeviceObj);
		}
		mLastAudioDeviceObj=env->NewGlobalRef(deviceObj);

		env->DeleteLocalRef(deviceObj);
	}

	return mAudioDevice;
}

InputDevice *JApplication::getInputDevice(InputDevice::InputType i) const{
	JNIEnv *env=getEnv();

	jobject deviceObj=env->CallObjectMethod(obj,getInputDeviceID,i);

	if(mInputDevices[i]==NULL || mLastInputDeviceObjs[i]!=deviceObj){
		//if(jobject is NInputDevice){
		//	mAudioDevice=deviceObj!=NULL?(AudioDevice*)env->CallIntMethod(deviceObj,getNativeHandleAudioDeviceID):NULL;
		//}
		//else{
			mInputDevices[i]=deviceObj!=NULL?new_JInputDevice(env,deviceObj):NULL;
		//

		if(mLastInputDeviceObjs[i]!=NULL){
			env->DeleteGlobalRef(mLastInputDeviceObjs[i]);
		}
		mLastInputDeviceObjs[i]=env->NewGlobalRef(deviceObj);

		env->DeleteLocalRef(deviceObj);
	}

	return mInputDevices[i];
}

Stream::ptr JApplication::makeStream(JNIEnv *env,jobject obj){
	return new JStream(env,obj);
}

JNIEnv *JApplication::getEnv() const{
	JNIEnv *env=NULL;
	vm->AttachCurrentThread(&env,NULL);
	return env;
}

}
}
