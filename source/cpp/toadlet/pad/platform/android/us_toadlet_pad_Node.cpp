#include <toadlet/tadpole/node/Node.h>
#include <toadlet/tadpole/Engine.h>

using namespace toadlet::tadpole;

extern "C" {

#include "us_toadlet_pad_Node.h"

jfieldID Node_nativeHandle=0;

void Java_us_toadlet_pad_Node(JNIEnv *env){
	jclass nodeClass=env->FindClass("us/toadlet/pad/Node");
	{
		Node_nativeHandle=env->GetFieldID(nodeClass,"mNativeHandle","I");
	}
	env->DeleteLocalRef(nodeClass);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_Node_setScope(JNIEnv *env,jobject obj,jint scope){
	Java_us_toadlet_pad_Node(env); // hack

	Node *node=(Node*)env->GetIntField(obj,Node_nativeHandle);

	node->setScope(scope);
}

}
