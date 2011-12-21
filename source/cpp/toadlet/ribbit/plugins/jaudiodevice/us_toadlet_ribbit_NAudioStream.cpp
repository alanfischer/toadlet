#include "JAudioDevice.h"
#include <toadlet/egg/Logger.h>

using namespace toadlet;
using namespace toadlet::ribbit;

extern "C" {

#include "us_toadlet_ribbit_NAudioStream.h"

JNIEXPORT void JNICALL Java_us_toadlet_ribbit_NAudioStream_close(JNIEnv *env,jobject obj){
	Logger::debug(Categories::TOADLET_RIBBIT,
		"Java_us_toadlet_ribbit_NAudioStream_close");

	AudioStream *stream=(AudioStream*)env->GetIntField(obj,nativeHandleID);
	stream->close();
	tbyte *buffer=(tbyte*)env->GetIntField(obj,nativeBufferID);
	delete[] buffer;
	buffer=NULL;
	env->SetIntField(obj,nativeBufferID,(int)buffer);
}

JNIEXPORT jobject JNICALL Java_us_toadlet_ribbit_NAudioStream_getAudioFormat(JNIEnv *env,jobject obj){
	Logger::debug(Categories::TOADLET_RIBBIT,
		"Java_us_toadlet_ribbit_NAudioStream_getAudioFormat");

	AudioStream *stream=(AudioStream*)env->GetIntField(obj,nativeHandleID);
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
	AudioStream *stream=(AudioStream*)env->GetIntField(obj,nativeHandleID);
	return stream->length();
}

JNIEXPORT jint JNICALL Java_us_toadlet_ribbit_NAudioStream_read(JNIEnv *env,jobject obj,jbyteArray data,jint off,jint len){
	AudioStream *stream=(AudioStream*)env->GetIntField(obj,nativeHandleID);
	tbyte *buffer=(tbyte*)env->GetIntField(obj,nativeBufferID);
	int length=(int)env->GetIntField(obj,nativeLengthID);
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
