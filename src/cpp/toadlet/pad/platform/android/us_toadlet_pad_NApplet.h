/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class us_toadlet_pad_NApplet */

#ifndef _Included_us_toadlet_pad_NApplet
#define _Included_us_toadlet_pad_NApplet
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     us_toadlet_pad_NApplet
 * Method:    create
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_create
  (JNIEnv *, jobject);

/*
 * Class:     us_toadlet_pad_NApplet
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_destroy
  (JNIEnv *, jobject);

/*
 * Class:     us_toadlet_pad_NApplet
 * Method:    update
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_update
  (JNIEnv *, jobject, jint);

/*
 * Class:     us_toadlet_pad_NApplet
 * Method:    render
 * Signature: (Lus/toadlet/pad/RenderDevice;)V
 */
JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_render
  (JNIEnv *, jobject, jobject);

/*
 * Class:     us_toadlet_pad_NApplet
 * Method:    resized
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_resized
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     us_toadlet_pad_NApplet
 * Method:    focusGained
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_focusGained
  (JNIEnv *, jobject);

/*
 * Class:     us_toadlet_pad_NApplet
 * Method:    focusLost
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_focusLost
  (JNIEnv *, jobject);

/*
 * Class:     us_toadlet_pad_NApplet
 * Method:    keyPressed
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_keyPressed
  (JNIEnv *, jobject, jint);

/*
 * Class:     us_toadlet_pad_NApplet
 * Method:    keyReleased
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_keyReleased
  (JNIEnv *, jobject, jint);

/*
 * Class:     us_toadlet_pad_NApplet
 * Method:    mousePressed
 * Signature: (III)V
 */
JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_mousePressed
  (JNIEnv *, jobject, jint, jint, jint);

/*
 * Class:     us_toadlet_pad_NApplet
 * Method:    mouseMoved
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_mouseMoved
  (JNIEnv *, jobject, jint, jint);

/*
 * Class:     us_toadlet_pad_NApplet
 * Method:    mouseReleased
 * Signature: (III)V
 */
JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_mouseReleased
  (JNIEnv *, jobject, jint, jint, jint);

/*
 * Class:     us_toadlet_pad_NApplet
 * Method:    mouseScrolled
 * Signature: (III)V
 */
JNIEXPORT void JNICALL Java_us_toadlet_pad_NApplet_mouseScrolled
  (JNIEnv *, jobject, jint, jint, jint);

#ifdef __cplusplus
}
#endif
#endif