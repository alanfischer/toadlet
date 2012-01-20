#include "JAudioDevice.h"
#include <toadlet/egg/Logger.h>

using namespace toadlet;
using namespace toadlet::ribbit;

extern "C" {

#include "us_toadlet_ribbit_NAudioStream.h"

jfieldID NAudioStream_nativeHandle=0;
jfieldID NAudioStream_nativeBuffer=0;
jfieldID NAudioStream_nativeLength=0;

void Java_us_toadlet_ribbit_NAudioStream(JNIEnv *env){
	jclass streamClass=env->FindClass("us/toadlet/ribbit/NAudioStream");
	{
		NAudioStream_nativeHandle=env->GetFieldID(streamClass,"mNativeHandle","I");
		NAudioStream_nativeBuffer=env->GetFieldID(streamClass,"mNativeBuffer","I");
		NAudioStream_nativeLength=env->GetFieldID(streamClass,"mNativeLength","I");
	}
	env->DeleteLocalRef(streamClass);
}

JNIEXPORT void JNICALL Java_us_toadlet_ribbit_NAudioStream_close(JNIEnv *env,jobject obj){
	Logger::debug(Categories::TOADLET_RIBBIT,
		"Java_us_toadlet_ribbit_NAudioStream_close");

	AudioStream *stream=(AudioStream*)env->GetIntField(obj,NAudioStream_nativeHandle);
	stream->close();
	tbyte *buffer=(tbyte*)env->GetIntField(obj,NAudioStream_nativeBuffer);
	delete[] buffer;
	buffer=NULL;
	env->SetIntField(obj,NAudioStream_nativeBuffer,(int)buffer);
}

JNIEXPORT jobject JNICALL Java_us_toadlet_ribbit_NAudioStream_getAudioFormat(JNIEnv *env,jobject obj){
	Logger::debug(Categories::TOADLET_RIBBIT,
		"Java_us_toadlet_ribbit_NAudioStream_getAudioFormat");

	AudioStream *stream=(AudioStream*)env->GetIntField(obj,NAudioStream_nativeHandle);
	AudioFormat *format=stream->getAudioFormat();

	jobject formatObj=NULL;
	jclass streamClass=env->FindClass("us/toadlet/ribbit/AudioFormat");
	{
		jmethodID initID=env->GetMethodID(streamClass,"<init>","(III)V");
		formatObj=env->NewObject(streamClass,initID,format->getBitsPerSample(),format->getChannels(),format->getSamplesPerSecond());
	}
	env->DeleteLocalRef(streamClass);

	return formatObj;
}

JNIEXPORT jint JNICALL Java_us_toadlet_ribbit_NAudioStream_available(JNIEnv *env,jobject obj){
	AudioStream *stream=(AudioStream*)env->GetIntField(obj,NAudioStream_nativeHandle);
	return stream->length();
}

JNIEXPORT jint JNICALL Java_us_toadlet_ribbit_NAudioStream_read(JNIEnv *env,jobject obj,jbyteArray data,jint off,jint len){
	AudioStream *stream=(AudioStream*)env->GetIntField(obj,NAudioStream_nativeHandle);
	tbyte *buffer=(tbyte*)env->GetIntField(obj,NAudioStream_nativeBuffer);
	int length=(int)env->GetIntField(obj,NAudioStream_nativeLength);
	int total=0,amt=0;
	while(len>0){
		amt=length<len?length:len;
		amt=stream->read(buffer,amt);
		if(amt<=0){
			break;
		}
		env->SetByteArrayRegion(data,total,amt,(jbyte*)buffer);
		total+=amt;
		len-=amt;
	}

	if(total==0){
		total=-1;
	}
	return total;
}

}
