#include <toadlet/tadpole/node/Node.h>
#include <toadlet/tadpole/Engine.h>

using namespace toadlet::tadpole;

extern "C" {

#include "us_toadlet_tadpole_Node.h"

jfieldID Node_nativeHandle=0;

void Java_us_toadlet_tadpole_Node(JNIEnv *env){
	jclass nodeClass=env->FindClass("us/toadlet/tadpole/Node");
	{
		Node_nativeHandle=env->GetFieldID(nodeClass,"mNativeHandle","I");
	}
	env->DeleteLocalRef(nodeClass);
}

JNIEXPORT void JNICALL Java_us_toadlet_tadpole_Node_destroy(JNIEnv *env,jobject obj){
	Java_us_toadlet_tadpole_Node(env); // hack

	Node *node=(Node*)env->GetIntField(obj,Node_nativeHandle);
	
	node->destroy();
}

JNIEXPORT void JNICALL Java_us_toadlet_tadpole_Node_setRotate(JNIEnv *env,jobject obj,jfloatArray axisObj,jfloat angle){
	Java_us_toadlet_tadpole_Node(env); // hack

	Node *node=(Node*)env->GetIntField(obj,Node_nativeHandle);

	float *axis=env->GetFloatArrayElements(axisObj,NULL);
	
	node->setRotate(axis,angle);
	
	env->ReleaseFloatArrayElements(axisObj,axis,0);
}

JNIEXPORT void JNICALL Java_us_toadlet_tadpole_Node_setScope(JNIEnv *env,jobject obj,jint scope){
	Java_us_toadlet_tadpole_Node(env); // hack

	Node *node=(Node*)env->GetIntField(obj,Node_nativeHandle);

	node->setScope(scope);
}

JNIEXPORT jint JNICALL Java_us_toadlet_tadpole_Node_getScope(JNIEnv *env,jobject obj){
	Java_us_toadlet_tadpole_Node(env); // hack

	Node *node=(Node*)env->GetIntField(obj,Node_nativeHandle);

	return node->getScope();
}

JNIEXPORT jboolean JNICALL Java_us_toadlet_tadpole_Node_attach(JNIEnv *env,jobject obj,jobject childObj){
	Java_us_toadlet_tadpole_Node(env); // hack

	Node *node=(Node*)env->GetIntField(obj,Node_nativeHandle);
	Node *child=(Node*)env->GetIntField(childObj,Node_nativeHandle);

	return node->attach(child);
}

JNIEXPORT jboolean JNICALL Java_us_toadlet_tadpole_Node_remove(JNIEnv *env,jobject obj,jobject childObj){
	Java_us_toadlet_tadpole_Node(env); // hack

	Node *node=(Node*)env->GetIntField(obj,Node_nativeHandle);
	Node *child=(Node*)env->GetIntField(childObj,Node_nativeHandle);

	return node->remove(child);
}

}
