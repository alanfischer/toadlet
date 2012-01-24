#include <toadlet/tadpole/node/ParentNode.h>

using namespace toadlet::tadpole;

extern "C" {

#include "us_toadlet_pad_ParentNode.h"

jfieldID ParentNode_nativeHandle=0;

void Java_us_toadlet_pad_ParentNode(JNIEnv *env){
	jclass nodeClass=env->FindClass("us/toadlet/pad/Node");
	{
		ParentNode_nativeHandle=env->GetFieldID(nodeClass,"mNativeHandle","I");
	}
	env->DeleteLocalRef(nodeClass);
}

JNIEXPORT jboolean JNICALL Java_us_toadlet_pad_ParentNode_attach(JNIEnv *env,jobject obj,jobject nodeObj){
	Java_us_toadlet_pad_ParentNode(env); // hack

	ParentNode *parentNode=(ParentNode*)env->GetIntField(obj,ParentNode_nativeHandle);
	Node *node=(Node*)env->GetIntField(nodeObj,ParentNode_nativeHandle);

	return parentNode->attach(node);
}

JNIEXPORT jboolean JNICALL Java_us_toadlet_pad_ParentNode_remove(JNIEnv *env,jobject obj,jobject nodeObj){
	ParentNode *parentNode=(ParentNode*)env->GetIntField(obj,ParentNode_nativeHandle);
	Node *node=(Node*)env->GetIntField(nodeObj,ParentNode_nativeHandle);

	return parentNode->remove(node);
}

}
