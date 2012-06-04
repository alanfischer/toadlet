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
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/io/FileStream.h>
#include <toadlet/egg/io/MemoryStream.h>
#include "AndroidAssetArchive.h"

namespace toadlet{
namespace egg{

AndroidAssetArchive::AndroidAssetArchive(JNIEnv *env1,jobject assetManagerObj1):
	env(NULL),
	jvm(NULL)
{
	env=env1;
	env->GetJavaVM(&jvm);
	assetManagerObj=env->NewGlobalRef(assetManagerObj1);

	jclass managerClass=env->FindClass("android/content/res/AssetManager");
	{
		openManagerID=env->GetMethodID(managerClass,"open","(Ljava/lang/String;)Ljava/io/InputStream;");
	}
	env->DeleteLocalRef(managerClass);

	mEntries=Collection<String>::ptr(new Collection<String>());
	mStream=new JStream(env);
}

AndroidAssetArchive::~AndroidAssetArchive(){
	destroy();

	env=NULL;
}

void AndroidAssetArchive::destroy(){
	if(assetManagerObj!=NULL){
		env->DeleteGlobalRef(assetManagerObj);
		assetManagerObj=NULL;
	}
}

Stream::ptr AndroidAssetArchive::openStream(const String &name){
	Logger::debug(Categories::TOADLET_EGG,"AndroidAssetArchive::openStream:"+name);

	jvm->AttachCurrentThread(&env,NULL);

	jstring nameObj=env->NewStringUTF(name);
	jobject streamObj=env->CallObjectMethod(assetManagerObj,openManagerID,nameObj);
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

}
}
