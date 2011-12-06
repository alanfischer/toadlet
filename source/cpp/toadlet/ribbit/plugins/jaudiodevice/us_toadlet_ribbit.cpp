#include <jni.h>

namespace toadlet{
namespace ribbit{

jfieldID nativeHandleID=0;
jfieldID nativeBufferID=0;
jfieldID nativeLengthID=0;

}
}

using namespace toadlet::ribbit;

extern "C" JNIEXPORT void Java_us_toadlet_ribbit(JNIEnv *env){
	jclass streamClass=env->FindClass("us/toadlet/ribbit/NAudioStream");
	{
		nativeHandleID=env->GetFieldID(streamClass,"mNativeHandle","I");
		nativeBufferID=env->GetFieldID(streamClass,"mNativeBuffer","I");
		nativeLengthID=env->GetFieldID(streamClass,"mNativeLength","I");
	}
	env->DeleteLocalRef(streamClass);
}

#include "us_toadlet_ribbit_NAudioStream.cpp"
