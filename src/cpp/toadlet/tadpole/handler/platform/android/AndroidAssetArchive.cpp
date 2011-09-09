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
#include <toadlet/tadpole/handler/platform/android/AndroidAssetArchive.h>

namespace toadlet{
namespace tadpole{
namespace handler{

AndroidAssetArchive::AndroidAssetArchive(JNIEnv *jenv,jobject jassetManager){
	env=jenv;
	assetManager=env->NewGlobalRef(jassetManager);
	mEntries=Collection<String>::ptr(new Collection<String>());

	jclass managerClass=env->FindClass("android/content/res/AssetManager");
	{
		openManagerID=env->GetMethodID(managerClass,"open","(Ljava/lang/String;)Ljava/io/InputStream;");
	}
	env->DeleteLocalRef(managerClass);

	jclass streamClass=env->FindClass("java/io/InputStream");
	{
		availableStreamID=env->GetMethodID(streamClass,"available","()I");
		readStreamID=env->GetMethodID(streamClass,"read","([BII)I");
	}
	env->DeleteLocalRef(streamClass);
}

AndroidAssetArchive::~AndroidAssetArchive(){
	destroy();
}

void AndroidAssetArchive::destroy(){
	if(assetManager!=NULL){
		env->DeleteGlobalRef(assetManager);
		assetManager=NULL;
	}

	env=NULL;
}

Stream::ptr AndroidAssetArchive::openStream(const String &name){
	jstring jname=env->NewStringUTF(name);
	jobject inputStream=env->CallObjectMethod(assetManager,openManagerID,jname);
	jthrowable exc=env->ExceptionOccurred();
	if(exc!=NULL){
		env->ExceptionDescribe();
		env->ExceptionClear();
		env->DeleteLocalRef(jname);
		return NULL;
	}
	jint length=env->CallIntMethod(inputStream,availableStreamID);
	jbyteArray bytes=env->NewByteArray(length);
	tbyte *data=new tbyte[length];

	env->CallIntMethod(inputStream,readStreamID,bytes,0,length);
	env->GetByteArrayRegion(bytes,0,length,(jbyte*)data);

	MemoryStream::ptr stream(new MemoryStream(data,length,length,true));
	env->DeleteLocalRef(bytes);
	env->DeleteLocalRef(inputStream);
	env->DeleteLocalRef(jname);
	
	return stream;
}

Collection<String>::ptr AndroidAssetArchive::getEntries(){
	return mEntries;
}

}
}
}
