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

#include "JAndroidApplication.h"
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

// Method signatures: http://dev.kanngard.net/Permalinks/ID_20050509144235.html
JAndroidApplication::JAndroidApplication(JNIEnv *jenv,jobject jobj):
	env(NULL),
	obj(NULL),

	mRenderDevice(NULL),
	mLastRenderDeviceObj(NULL),
	mAudioDevice(NULL),
	mLastAudioDeviceObj(NULL)
{
	env=jenv;
	obj=env->NewGlobalRef(jobj);

	jclass appClass=env->GetObjectClass(obj);
	{
		createID=env->GetMethodID(appClass,"create","()V");
		destroyID=env->GetMethodID(appClass,"destroy","()V");
		startID=env->GetMethodID(appClass,"start","()V");
		stopID=env->GetMethodID(appClass,"stop","()V");
		getWidthID=env->GetMethodID(appClass,"getWidth","()I");
		getHeightID=env->GetMethodID(appClass,"getHeight","()I");
		setDifferenceMouseID=env->GetMethodID(appClass,"setDifferenceMouse","(Z)V");
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

JAndroidApplication::~JAndroidApplication(){
	env->DeleteGlobalRef(obj);
	obj=NULL;
	env=NULL;
}

void JAndroidApplication::create(String renderDevice,String audioDevice,String motionDevice){
	env->CallVoidMethod(obj,createID);
}

void JAndroidApplication::destroy(){
	env->CallVoidMethod(obj,destroyID);
}

void JAndroidApplication::start(){
	env->CallVoidMethod(obj,startID);
}

void JAndroidApplication::stop(){
	env->CallVoidMethod(obj,stopID);
}

int JAndroidApplication::getWidth(){
	env->CallIntMethod(obj,getWidthID);
}

int JAndroidApplication::getHeight(){
	env->CallIntMethod(obj,getHeightID);
}

void JAndroidApplication::setDifferenceMouse(bool difference){
	env->CallVoidMethod(obj,setDifferenceMouseID,difference);
}

Engine *JAndroidApplication::getEngine(){
	Engine *engine=NULL;

	jobject jengine=env->CallObjectMethod(obj,getEngineID);

	if(jengine!=NULL){
		engine=(Engine*)env->CallIntMethod(jengine,getNativeHandleEngineID);

		env->DeleteLocalRef(jengine);
	}

	return engine;
}

RenderDevice *JAndroidApplication::getRenderDevice(){
	jobject deviceObj=env->CallObjectMethod(obj,getRenderDeviceID);

	if(mRenderDevice==NULL || mLastRenderDeviceObj!=deviceObj){
		//if(jobject is NRenderDevice){
			mRenderDevice=(RenderDevice*)env->CallIntMethod(deviceObj,getNativeHandleRenderDeviceID);
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

AudioDevice *JAndroidApplication::getAudioDevice(){
	jobject deviceObj=env->CallObjectMethod(obj,getAudioDeviceID);

	if(mAudioDevice==NULL || mLastAudioDeviceObj!=deviceObj){
		//if(jobject is NAudioDevice){
		//	mAudioDevice=(AudioDevice*)env->CallIntMethod(deviceObj,getNativeHandleAudioDeviceID);
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

