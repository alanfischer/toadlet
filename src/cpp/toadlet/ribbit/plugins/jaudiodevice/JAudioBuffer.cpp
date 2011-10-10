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

#include "JAudioBuffer.h"
#include <toadlet/egg/Logger.h>

namespace toadlet{
namespace ribbit{

JAudioBuffer::JAudioBuffer(JNIEnv *jenv,jobject jobj):BaseResource(){
	env=jenv;
	obj=env->NewGlobalRef(jobj);

	jclass bufferClass=env->GetObjectClass(obj);
	{
		createID=env->GetMethodID(bufferClass,"create","(Lus/toadlet/ribbit/AudioStream;)Z");
		destroyID=env->GetMethodID(bufferClass,"destroy","()V");
	}
	env->DeleteLocalRef(bufferClass);
}

JAudioBuffer::~JAudioBuffer(){
	env->DeleteGlobalRef(obj);
	obj=NULL;
	env=NULL;
}

bool JAudioBuffer::create(AudioStream::ptr stream){
	Logger::alert(Categories::TOADLET_RIBBIT,
		"JAudioBuffer::create");

	mAudioStream=stream; // Store the pointer until we have reference counting
	jobject streamObj=NULL;

	jclass streamClass=env->FindClass("us/toadlet/ribbit/NAudioStream");
	{
		jmethodID initID=env->GetMethodID(streamClass,"<init>","(III)V");
		int length=2048;
		tbyte *buffer=new tbyte[length]; 
		streamObj=env->NewObject(streamClass,initID,(int)stream.get(),buffer,length);
	}
	env->DeleteLocalRef(streamClass);

	jthrowable exc=env->ExceptionOccurred();
	if(exc!=NULL){
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}

	return env->CallBooleanMethod(obj,createID,streamObj);
}

void JAudioBuffer::destroy(){
	env->CallVoidMethod(obj,destroyID);
}

}
}
