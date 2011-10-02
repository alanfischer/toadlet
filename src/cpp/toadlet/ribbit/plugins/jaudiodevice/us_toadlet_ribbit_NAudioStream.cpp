#include "us_toadlet_ribbit_NAudioStream.h"
#include "JAudioDevice.h"

using namespace toadlet::ribbit;

JNIEXPORT jobject JNICALL Java_us_toadlet_ribbit_NAudioStream_getAudioFormat(JNIEnv *env,jobject obj){
	AudioStream *stream=(AudioStream*)env->CallIntMethod(obj,getNativeHandleStreamID);
	AudioFormat *format=stream->getAudioFormat();

	jobject formatObj=NULL;
	jclass streamClass=env->FindClass("us/toadlet/ribbit/AudioFormat");
	{
		jmethodID initID=env->GetMethodID(streamClass,"<init>","(III)V");
		formatObj=env->NewObject(streamClass,initID,format->bitsPerSample,format->channels,format->samplesPerSecond);
	}
	env->DeleteLocalRef(streamClass);

	return formatObj;
}

JNIEXPORT jint JNICALL Java_us_toadlet_ribbit_NAudioStream_available(JNIEnv *env,jobject obj){
	AudioStream *stream=(AudioStream*)env->CallIntMethod(obj,getNativeHandleStreamID);
	return stream->length();
}

jbyte streamBuffer[2048];

JNIEXPORT jint JNICALL Java_us_toadlet_ribbit_NAudioStream_read(JNIEnv *env,jobject obj,jbyteArray data,jint off,jint len){
	AudioStream *stream=(AudioStream*)env->CallIntMethod(obj,getNativeHandleStreamID);
	len=2048<len?2048:len;
	int amt=stream->read((toadlet::tbyte*)streamBuffer,len);
	env->SetByteArrayRegion(data,off,len,streamBuffer);
	return amt;
}
