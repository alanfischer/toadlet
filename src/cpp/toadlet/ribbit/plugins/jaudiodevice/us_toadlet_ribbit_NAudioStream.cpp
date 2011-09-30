#include "us_toadlet_ribbit_NAudioStream.h"
#include <toadlet/ribbit/JAudioDevice.h>

using namespace toadlet::ribbit;

JNIEXPORT jobject JNICALL Java_us_toadlet_ribbit_NAudioStream_getAudioFormat(JNIEnv *env,jobject obj){
	AudioStream *stream=obj->getStream();
	AudioFormat *format=stream->getAudioFormat();

	return create AudioFormat(format);
}

JNIEXPORT jint JNICALL Java_us_toadlet_ribbit_NAudioStream_available(JNIEnv *env,jobject obj){
	AudioStream *stream=obj->getStream();
	return stream->available();
}

JNIEXPORT jint JNICALL Java_us_toadlet_ribbit_NAudioStream_read(JNIEnv *env,jobject obj,jbyteArray data,jint off,jint len){
	AudioStream *stream=obj->getStream();
	tbyte tdata=new tbyte[len];
	int amt=stream->read(data,len);
	setByteArray(data,tdata,len);
	return amt;
}
