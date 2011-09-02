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

using namespace toadlet::peeper;
using namespace toadlet::ribbit;
using namespace toadlet::flick;
using namespace toadlet::tadpole;
using namespace toadlet::pad;

extern "C" RenderDevice *new_GLRenderDevice();

namespace toadlet{
namespace pad{

// Method signatures: http://dev.kanngard.net/Permalinks/ID_20050509144235.html
JAndroidApplication::JAndroidApplication(JNIEnv *jenv,jobject jobj):
	env(NULL),
	obj(NULL)
{
	env=jenv;
	obj=env->NewGlobalRef(jobj);

	jclass appClass=env->GetObjectClass(obj);
	{
		createID=env->GetMethodID(appClass,"create","()V");
		destroyID=env->GetMethodID(appClass,"destroy","()V");
		startID=env->GetMethodID(appClass,"start","()V");
		stopID=env->GetMethodID(appClass,"stop","()V");
		getEngineID=env->GetMethodID(appClass,"getEngine","()Lus/toadlet/pad/Engine");
	}
	env->DeleteLocalRef(appClass);
	
	jclass nengineClass=env->FindClass("us/toadlet/pad/NEngine");
	{
		getNativeHandleID=env->GetMethodID(nengineClass,"getNativeHandle","()I");
	}
	env->DeleteLocalRef(nengineClass);
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

Engine *JAndroidApplication::getEngine(){
	Engine *engine=NULL;

	jobject jengine=env->CallObjectMethod(obj,getEngineID);

	engine=(Engine*)env->CallIntMethod(jengine,getNativeHandleID);

	env->DeleteLocalRef(jengine);

	return engine;
}

}
}

JNIEXPORT jobject JNICALL Java_us_toadlet_pad_AndroidApplication_makeEngine(JNIEnv *env,jobject obj){
	Engine *engine=new Engine();
	jobject jengine=NULL;
	
	jclass nengineClass=env->FindClass("us/toadlet/pad/NEngine");
	{
		jmethodID initID=env->GetMethodID(nengineClass,"<init>","(I)V");
		jengine=env->NewObject(nengineClass,initID,(int)engine);
	}
	env->DeleteLocalRef(nengineClass);
	
	return jengine;
}

JNIEXPORT jobject JNICALL Java_us_toadlet_pad_AndroidApplication_makeRenderDevice(JNIEnv *env,jobject obj){
	RenderDevice *device=new_GLRenderDevice();
	jobject jdevice=NULL;
	
	jclass ndeviceClass=env->FindClass("us/toadlet/pad/NRenderDevice");
	{
		jmethodID initID=env->GetMethodID(ndeviceClass,"<init>","(I)V");
		jdevice=env->NewObject(ndeviceClass,initID,(int)device);
	}
	env->DeleteLocalRef(ndeviceClass);

	return jdevice;
}
