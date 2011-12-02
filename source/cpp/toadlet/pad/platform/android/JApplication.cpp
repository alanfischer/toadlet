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
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::ribbit;
using namespace toadlet::flick;
using namespace toadlet::tadpole;
using namespace toadlet::pad;

TOADLET_C_API AudioDevice* new_JAudioDevice(JNIEnv *env,jobject obj);

extern jmethodID getNativeHandleEngineID;
extern jmethodID getNativeHandleRenderDeviceID;
extern "C" JNIEXPORT void Java_us_toadlet_pad(JNIEnv *env);

namespace toadlet{
namespace pad{

JApplication::JApplication(JNIEnv *jenv,jobject jobj):
	env(NULL),
	obj(NULL),

	mEngine(NULL),
	mLastEngineObj(NULL),
	mRenderDevice(NULL),
	mLastRenderDeviceObj(NULL),
	mAudioDevice(NULL),
	mLastAudioDeviceObj(NULL)
{
	env=jenv;
	obj=env->NewGlobalRef(jobj);

	jclass appClass=env->GetObjectClass(obj);
	{
		createID=env->GetMethodID(appClass,"create","()Z");
		destroyID=env->GetMethodID(appClass,"destroy","()V");
		startID=env->GetMethodID(appClass,"start","()V");
		stopID=env->GetMethodID(appClass,"stop","()V");
		isRunningID=env->GetMethodID(appClass,"isRunning","()Z");
		setTitleID=env->GetMethodID(appClass,"setTitle","(Ljava/lang/CharSequence;)V");
		getTitleID=env->GetMethodID(appClass,"getTitle","()Ljava/lang/CharSequence;");
		getWidthID=env->GetMethodID(appClass,"getWidth","()I");
		getHeightID=env->GetMethodID(appClass,"getHeight","()I");
		setDifferenceMouseID=env->GetMethodID(appClass,"setDifferenceMouse","(Z)V");
		getDifferenceMouseID=env->GetMethodID(appClass,"getDifferenceMouse","()Z");
		getEngineID=env->GetMethodID(appClass,"getEngine","()Lus/toadlet/pad/Engine;");
		getRenderDeviceID=env->GetMethodID(appClass,"getRenderDevice","()Lus/toadlet/pad/RenderDevice;");
		getAudioDeviceID=env->GetMethodID(appClass,"getAudioDevice","()Lus/toadlet/ribbit/AudioDevice;");

		setNativeHandleID=env->GetMethodID(appClass,"setNativeHandle","(I)V");
		getNativeHandleID=env->GetMethodID(appClass,"getNativeHandle","()I");
	}
	env->DeleteLocalRef(appClass);

	env->CallVoidMethod(obj,setNativeHandleID,(int)this);

	Java_us_toadlet_pad(env);
}

JApplication::~JApplication(){
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

bool JApplication::create(String renderDevice,String audioDevice,String motionDevice,String joyDevice){
	return env->CallBooleanMethod(obj,createID);
}

void JApplication::destroy(){
	env->CallVoidMethod(obj,destroyID);
}

void JApplication::start(){
	env->CallVoidMethod(obj,startID);
}

void JApplication::stop(){
	env->CallVoidMethod(obj,stopID);
}

bool JApplication::isRunning() const{
	env->CallBooleanMethod(obj,isRunningID);
}

void JApplication::setTitle(const String &title){
// Disabled since calling it enough calls a crash in Window.setTitle for some reason
#if 0
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
	jstring titleObj=(jstring)env->CallObjectMethod(obj,getTitleID);
	const char *chars=env->GetStringUTFChars(titleObj,NULL);
	String title=chars;
	env->ReleaseStringUTFChars(titleObj,chars);
	return title;
}

int JApplication::getWidth() const{
	env->CallIntMethod(obj,getWidthID);
}

int JApplication::getHeight() const{
	env->CallIntMethod(obj,getHeightID);
}

void JApplication::setDifferenceMouse(bool difference){
	env->CallVoidMethod(obj,setDifferenceMouseID,difference);
}

bool JApplication::getDifferenceMouse() const{
	env->CallBooleanMethod(obj,getDifferenceMouseID);
}

Engine *JApplication::getEngine() const{
	jobject engineObj=env->CallObjectMethod(obj,getEngineID);

	if(mEngine==NULL || mLastEngineObj!=engineObj){
		//if(obj is NEngine){
			mEngine=engineObj!=NULL?(Engine*)env->CallIntMethod(engineObj,getNativeHandleEngineID):NULL;
		//}
		//else{
		//	mEngine=new JENgine(env,engineObj);
		//

		if(mLastEngineObj!=NULL){
			env->DeleteGlobalRef(mLastEngineObj);
		}
		mLastEngineObj=env->NewGlobalRef(engineObj);
		
		env->DeleteLocalRef(engineObj);
	}
	
	return mEngine;
}

RenderDevice *JApplication::getRenderDevice() const{
	jobject deviceObj=env->CallObjectMethod(obj,getRenderDeviceID);

	if(mRenderDevice==NULL || mLastRenderDeviceObj!=deviceObj){
		//if(jobject is NRenderDevice){
			mRenderDevice=deviceObj!=NULL?(RenderDevice*)env->CallIntMethod(deviceObj,getNativeHandleRenderDeviceID):NULL;
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
	jobject deviceObj=env->CallObjectMethod(obj,getAudioDeviceID);

	if(mAudioDevice==NULL || mLastAudioDeviceObj!=deviceObj){
		//if(jobject is NAudioDevice){
		//	mAudioDevice=deviceObj!=NULL?(AudioDevice*)env->CallIntMethod(deviceObj,getNativeHandleAudioDeviceID):NULL;
		//}
		//else{
			mAudioDevice=new_JAudioDevice(env,deviceObj);
		//

		if(mLastAudioDeviceObj!=NULL){
			env->DeleteGlobalRef(mLastAudioDeviceObj);
		}
		mLastAudioDeviceObj=env->NewGlobalRef(deviceObj);

		env->DeleteLocalRef(deviceObj);
	}

	return mAudioDevice;
}

}
}
