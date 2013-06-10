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

#include <toadlet/egg/Error.h>
#include <toadlet/egg/Log.h>
#include <toadlet/egg/io/FileStream.h>
#include <toadlet/egg/io/MemoryStream.h>
#include "AndroidAssetArchive.h"

namespace toadlet{
namespace egg{

enum{
	ACCESS_UNKNOWN=0,
	ACCESS_RANDOM=1,
	ACCESS_STREAMING=2,
	ACCESS_BUFFER=3
};

AndroidAssetArchive::AndroidAssetArchive(JNIEnv *jenv,jobject jobj):
	vm(NULL),
	obj(NULL)
{
	jenv->GetJavaVM(&vm);
	JNIEnv *env=getEnv();
	obj=env->NewGlobalRef(jobj);

	jclass managerClass=env->FindClass("android/content/res/AssetManager");
	{
		openManagerID=env->GetMethodID(managerClass,"open","(Ljava/lang/String;I)Ljava/io/InputStream;");
	}
	env->DeleteLocalRef(managerClass);

	mEntries=Collection<String>::ptr(new Collection<String>());
	mStream=new JStream(env);
}

AndroidAssetArchive::~AndroidAssetArchive(){
	getEnv()->DeleteGlobalRef(obj);
	obj=NULL;
	vm=NULL;
}

Stream::ptr AndroidAssetArchive::openStream(const String &name){
	JNIEnv *env=getEnv();

	Log::debug(Categories::TOADLET_EGG,"AndroidAssetArchive::openStream:"+name);

	jstring nameObj=env->NewStringUTF(name);
	jobject streamObj=env->CallObjectMethod(obj,openManagerID,nameObj,ACCESS_RANDOM);
	jthrowable exc=env->ExceptionOccurred();
	if(exc!=NULL){
		env->ExceptionDescribe();
		env->ExceptionClear();
	}
	env->DeleteLocalRef(nameObj);
	if(exc!=NULL){
		return NULL;
	}

	JStream::ptr stream;
	if(mStream->closed()){
		stream=mStream; // Reuse our stream if its closed
	}
	else{
		stream=new JStream(env);
	}
	stream->open(streamObj);
	env->DeleteLocalRef(streamObj);
	if(stream->closed()){
		stream=NULL;
	}
	return stream;
}

Collection<String>::ptr AndroidAssetArchive::getEntries(){
	return mEntries;
}

JNIEnv *AndroidAssetArchive::getEnv() const{
	JNIEnv *env=NULL;
	vm->AttachCurrentThread(&env,NULL);
	return env;
}

}
}
