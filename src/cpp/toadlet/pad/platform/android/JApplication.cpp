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

TOADLET_C_API RenderDevice* new_GLRenderDevice();
TOADLET_C_API AudioDevice* new_JAudioDevice(JNIEnv *env,jobject obj);

void Java_us_toadlet_pad(JNIEnv *env);

jmethodID getRootRenderTargetRenderTargetID=0;
jmethodID getNativeHandleRenderDeviceID=0;
jmethodID getNativeHandleAudioDeviceID=0;
jmethodID getNativeHandleEngineID=0;
jmethodID getNativeHandleAppletID=0;
jmethodID getNativeHandleApplicationID=0;

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

void Java_us_toadlet_pad(JNIEnv *env){
	jclass targetClass=env->FindClass("us/toadlet/peeper/RenderTarget");
	{
		getRootRenderTargetRenderTargetID=env->GetMethodID(targetClass,"getRootRenderTarget","()Lus/toadlet/peeper/RenderTarget;");
	}
	env->DeleteLocalRef(targetClass);

	jclass deviceClass=env->FindClass("us/toadlet/pad/RenderDevice");
	{
		getNativeHandleRenderDeviceID=env->GetMethodID(deviceClass,"getNativeHandle","()I");
	}
	env->DeleteLocalRef(deviceClass);

	jclass engineClass=env->FindClass("us/toadlet/pad/Engine");
	{
		getNativeHandleEngineID=env->GetMethodID(engineClass,"getNativeHandle","()I");
	}
	env->DeleteLocalRef(engineClass);

	jclass appletClass=env->FindClass("us/toadlet/pad/NApplet");
	{
		getNativeHandleAppletID=env->GetMethodID(appletClass,"getNativeHandle","()I");
	}
	env->DeleteLocalRef(appletClass);

	jclass appClass=env->FindClass("us/toadlet/pad/AndroidApplication");
	{
		getNativeHandleApplicationID=env->GetMethodID(appClass,"getNativeHandle","()I");
	}
	env->DeleteLocalRef(appClass);
}

#include "us_toadlet_pad_AndroidApplication.cpp"
#include "us_toadlet_pad_NApplet.cpp"
#include "us_toadlet_pad_RenderDevice.cpp"
#include "us_toadlet_pad_Engine.cpp"

