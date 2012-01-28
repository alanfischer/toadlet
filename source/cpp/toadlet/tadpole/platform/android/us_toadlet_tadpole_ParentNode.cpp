#include <toadlet/tadpole/node/ParentNode.h>

using namespace toadlet::tadpole;

extern "C" {

#include "us_toadlet_tadpole_ParentNode.h"

jfieldID ParentNode_nativeHandle=0;

void Java_us_toadlet_tadpole_ParentNode(JNIEnv *env){
	jclass nodeClass=env->FindClass("us/toadlet/tadpole/Node");
	{
		ParentNode_nativeHandle=env->GetFieldID(nodeClass,"mNativeHandle","I");
	}
	env->DeleteLocalRef(nodeClass);
}

JNIEXPORT jboolean JNICALL Java_us_toadlet_tadpole_ParentNode_attach(JNIEnv *env,jobject obj,jobject nodeObj){
	Java_us_toadlet_tadpole_ParentNode(env); // hack

	ParentNode *parentNode=(ParentNode*)env->GetIntField(obj,ParentNode_nativeHandle);
	Node *node=(Node*)env->GetIntField(nodeObj,ParentNode_nativeHandle);

	return parentNode->attach(node);
}

JNIEXPORT jboolean JNICALL Java_us_toadlet_tadpole_ParentNode_remove(JNIEnv *env,jobject obj,jobject nodeObj){
	ParentNode *parentNode=(ParentNode*)env->GetIntField(obj,ParentNode_nativeHandle);
	Node *node=(Node*)env->GetIntField(nodeObj,ParentNode_nativeHandle);

	return parentNode->remove(node);
}

}
