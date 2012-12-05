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
#include "JStream.h"

namespace toadlet{
namespace egg{

JStream::JStream(JNIEnv *env1,jobject streamObj):
	istreamClass(NULL),ostreamClass(NULL),
	istreamObj(NULL),ostreamObj(NULL),
	bufferLength(0),
	bufferObj(NULL),
	current(0)
{
	env=env1;

	istreamClass=(jclass)env->NewGlobalRef(env->FindClass("java/io/InputStream"));
	{
		closeIStreamID=env->GetMethodID(istreamClass,"close","()V");
		readIStreamID=env->GetMethodID(istreamClass,"read","([BII)I");
		availableIStreamID=env->GetMethodID(istreamClass,"available","()I");
		markIStreamID=env->GetMethodID(istreamClass,"mark","(I)V");
		resetIStreamID=env->GetMethodID(istreamClass,"reset","()V");
		skipIStreamID=env->GetMethodID(istreamClass,"skip","(J)J");
	}

	ostreamClass=(jclass)env->NewGlobalRef(env->FindClass("java/io/OutputStream"));
	{
		closeOStreamID=env->GetMethodID(ostreamClass,"close","()V");
		writeOStreamID=env->GetMethodID(ostreamClass,"write","([BII)V");
	}

	bufferLength=2048;
	bufferObj=(jbyteArray)env->NewGlobalRef(env->NewByteArray(bufferLength));

	if(streamObj!=NULL){
		open(streamObj);
	}
}

JStream::~JStream(){
	close();

	env->DeleteGlobalRef(bufferObj);
	env->DeleteGlobalRef(istreamClass);
	env->DeleteGlobalRef(ostreamClass);
}

bool JStream::open(jobject streamObj){
	close();

	if(streamObj==NULL){
		return false;
	}

	if(env->IsInstanceOf(streamObj,istreamClass)){
		istreamObj=env->NewGlobalRef(streamObj);

		env->CallVoidMethod(streamObj,markIStreamID,65536);
	}
	else if(env->IsInstanceOf(streamObj,ostreamClass)){
		ostreamObj=env->NewGlobalRef(streamObj);
	}
}

void JStream::close(){
	if(istreamObj!=NULL){
		env->CallVoidMethod(istreamObj,closeIStreamID);
		istreamObj=NULL;
	}
	else if(ostreamObj!=NULL){
		env->CallVoidMethod(ostreamObj,closeOStreamID);
		ostreamObj=NULL;
	}

	jthrowable exc=env->ExceptionOccurred();
	if(exc!=NULL){
		env->ExceptionDescribe();
		env->ExceptionClear();
	}
}

int JStream::read(tbyte *buffer,int length){
	if(istreamObj==NULL){
		return -1;
	}

	int amount=0;
	while(length>0){
		int a=length<bufferLength?length:bufferLength;
		a=env->CallIntMethod(istreamObj,readIStreamID,bufferObj,0,a);
		if(env->ExceptionOccurred()!=NULL){
			env->ExceptionDescribe();
			env->ExceptionClear();
			return -1;
		}
		if(a>0){
			env->GetByteArrayRegion(bufferObj,0,a,(jbyte*)buffer+amount);
		}
		else{
			break;
		}
		length-=a;
		amount+=a;
		current+=a;
	}

	return amount;
}

int JStream::write(const tbyte *buffer,int length){
	if(ostreamObj==NULL){
		return -1;
	}

	int amount=0;
	while(length>0){
		int a=length<bufferLength?length:bufferLength;
		env->SetByteArrayRegion(bufferObj,0,a,(jbyte*)buffer+amount);
		env->CallVoidMethod(ostreamObj,writeOStreamID,bufferObj,0,a);
		if(env->ExceptionOccurred()!=NULL){
			env->ExceptionDescribe();
			env->ExceptionClear();
			return -1;
		}
		length-=a;
		amount+=a;
		current+=a;
	}
	return amount;
}

bool JStream::reset(){
	if(istreamObj==NULL){
		return false;
	}

	env->CallVoidMethod(istreamObj,resetIStreamID);
	if(env->ExceptionOccurred()!=NULL){
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}
	
	current=0;
	
	return true;
}

int JStream::length(){
	if(istreamObj==NULL){
		return 0;
	}

	int available=env->CallIntMethod(istreamObj,availableIStreamID);
	if(env->ExceptionOccurred()!=NULL){
		env->ExceptionDescribe();
		env->ExceptionClear();
		return 0;
	}
	return current+available;
}

int JStream::position(){return current;}

bool JStream::seek(int offs){
	if(reset()==false){
		return false;
	}

	jlong amount=env->CallLongMethod(istreamObj,skipIStreamID,(jlong)offs);
	if(env->ExceptionOccurred()!=NULL){
		env->ExceptionDescribe();
		env->ExceptionClear();
		return false;
	}

	current=amount;
	return true;
}

}
}
