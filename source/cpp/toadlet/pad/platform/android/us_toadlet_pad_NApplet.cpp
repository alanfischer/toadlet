#include <toadlet/pad/Applet.h>

using namespace toadlet::pad;

extern "C" {

#include "us_toadlet_pad_NApplet.h"

JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_create(JNIEnv *env,jobject obj){
	Applet *applet=(Applet*)env->GetIntField(obj,nativeHandleAppletID);
	applet->create();
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_destroy(JNIEnv *env,jobject obj){
	Applet *applet=(Applet*)env->GetIntField(obj,nativeHandleAppletID);
	applet->destroy();
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_update(JNIEnv *env,jobject obj,jint dt){
	Applet *applet=(Applet*)env->GetIntField(obj,nativeHandleAppletID);
	applet->update(dt);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_render(JNIEnv *env,jobject obj,jobject deviceobj){
	Applet *applet=(Applet*)env->GetIntField(obj,nativeHandleAppletID);
	RenderDevice *device=(RenderDevice*)env->CallIntMethod(deviceobj,getNativeHandleRenderDeviceID);
	applet->render(device);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_resized(JNIEnv *env,jobject obj,jint width,jint height){
	Applet *applet=(Applet*)env->GetIntField(obj,nativeHandleAppletID);
	applet->resized(width,height);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_focusGained(JNIEnv *env,jobject obj){
	Applet *applet=(Applet*)env->GetIntField(obj,nativeHandleAppletID);
	applet->focusGained();
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_focusLost(JNIEnv *env,jobject obj){
	Applet *applet=(Applet*)env->GetIntField(obj,nativeHandleAppletID);
	applet->focusLost();
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_keyPressed(JNIEnv *env,jobject obj,jint key){
	Applet *applet=(Applet*)env->GetIntField(obj,nativeHandleAppletID);
	applet->keyPressed(key);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_keyReleased(JNIEnv *env,jobject obj,jint key){
	Applet *applet=(Applet*)env->GetIntField(obj,nativeHandleAppletID);
	applet->keyReleased(key);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_mousePressed(JNIEnv *env,jobject obj,jint x,jint y,jint button){
	Applet *applet=(Applet*)env->GetIntField(obj,nativeHandleAppletID);
	applet->mousePressed(x,y,button);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_mouseMoved(JNIEnv *env,jobject obj,jint x,jint y){
	Applet *applet=(Applet*)env->GetIntField(obj,nativeHandleAppletID);
	applet->mouseMoved(x,y);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_mouseReleased(JNIEnv *env,jobject obj,jint x,jint y,jint button){
	Applet *applet=(Applet*)env->GetIntField(obj,nativeHandleAppletID);
	applet->mouseReleased(x,y,button);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_mouseScrolled(JNIEnv *env,jobject obj,jint x,jint y,jint scroll){
	Applet *applet=(Applet*)env->GetIntField(obj,nativeHandleAppletID);
	applet->mouseScrolled(x,y,scroll);
}

}
