#include "us_toadlet_pad_NApplet.h"
#include <toadlet/pad/Applet.h>

using namespace toadlet::pad;

JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_create(JNIEnv *env,jobject obj){
	Java_us_toadlet_pad(env);
	Applet *applet=(Applet*)env->CallIntMethod(obj,getNativeHandleAppletID);
	applet->create();
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_destroy(JNIEnv *env,jobject obj){
	Java_us_toadlet_pad(env);
	Applet *applet=(Applet*)env->CallIntMethod(obj,getNativeHandleAppletID);
	applet->destroy();
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_update(JNIEnv *env,jobject obj,jint dt){
	Java_us_toadlet_pad(env);
	Applet *applet=(Applet*)env->CallIntMethod(obj,getNativeHandleAppletID);
	applet->update(dt);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_render(JNIEnv *env,jobject obj,jobject deviceobj){
	Java_us_toadlet_pad(env);
	Applet *applet=(Applet*)env->CallIntMethod(obj,getNativeHandleAppletID);
	RenderDevice *device=(RenderDevice*)env->CallIntMethod(deviceobj,getNativeHandleRenderDeviceID);
	applet->render(device);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_resized(JNIEnv *env,jobject obj,jint width,jint height){
	Java_us_toadlet_pad(env);
	Applet *applet=(Applet*)env->CallIntMethod(obj,getNativeHandleAppletID);
	applet->resized(width,height);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_focusGained(JNIEnv *env,jobject obj){
	Java_us_toadlet_pad(env);
	Applet *applet=(Applet*)env->CallIntMethod(obj,getNativeHandleAppletID);
	applet->focusGained();
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_focusLost(JNIEnv *env,jobject obj){
	Java_us_toadlet_pad(env);
	Applet *applet=(Applet*)env->CallIntMethod(obj,getNativeHandleAppletID);
	applet->focusLost();
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_keyPressed(JNIEnv *env,jobject obj,jint key){
	Java_us_toadlet_pad(env);
	Applet *applet=(Applet*)env->CallIntMethod(obj,getNativeHandleAppletID);
	Logger::alert("keyPressed!");
	applet->keyPressed(key);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_keyReleased(JNIEnv *env,jobject obj,jint key){
	Java_us_toadlet_pad(env);
	Applet *applet=(Applet*)env->CallIntMethod(obj,getNativeHandleAppletID);
	applet->keyReleased(key);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_mousePressed(JNIEnv *env,jobject obj,jint x,jint y,jint button){
	Java_us_toadlet_pad(env);
	Applet *applet=(Applet*)env->CallIntMethod(obj,getNativeHandleAppletID);
	applet->mousePressed(x,y,button);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_mouseMoved(JNIEnv *env,jobject obj,jint x,jint y){
	Java_us_toadlet_pad(env);
	Applet *applet=(Applet*)env->CallIntMethod(obj,getNativeHandleAppletID);
	applet->mouseMoved(x,y);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_mouseReleased(JNIEnv *env,jobject obj,jint x,jint y,jint button){
	Java_us_toadlet_pad(env);
	Applet *applet=(Applet*)env->CallIntMethod(obj,getNativeHandleAppletID);
	applet->mouseReleased(x,y,button);
}

JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_mouseScrolled(JNIEnv *env,jobject obj,jint x,jint y,jint scroll){
	Java_us_toadlet_pad(env);
	Applet *applet=(Applet*)env->CallIntMethod(obj,getNativeHandleAppletID);
	applet->mouseScrolled(x,y,scroll);
}
