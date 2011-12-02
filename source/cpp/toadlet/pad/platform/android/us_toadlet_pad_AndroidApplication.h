/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class us_toadlet_pad_AndroidApplication */

#ifndef _Included_us_toadlet_pad_AndroidApplication
#define _Included_us_toadlet_pad_AndroidApplication
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     us_toadlet_pad_AndroidApplication
 * Method:    createNativeApplication
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_us_toadlet_pad_AndroidApplication_createNativeApplication
  (JNIEnv *, jobject);

/*
 * Class:     us_toadlet_pad_AndroidApplication
 * Method:    destroyNativeApplication
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_us_toadlet_pad_AndroidApplication_destroyNativeApplication
  (JNIEnv *, jobject);

/*
 * Class:     us_toadlet_pad_AndroidApplication
 * Method:    makeEngine
 * Signature: ()Lus/toadlet/pad/Engine;
 */
JNIEXPORT jobject JNICALL Java_us_toadlet_pad_AndroidApplication_makeEngine
  (JNIEnv *, jobject);

/*
 * Class:     us_toadlet_pad_AndroidApplication
 * Method:    deleteEngine
 * Signature: (Lus/toadlet/pad/Engine;)V
 */
JNIEXPORT void JNICALL Java_us_toadlet_pad_AndroidApplication_deleteEngine
  (JNIEnv *, jobject, jobject);

/*
 * Class:     us_toadlet_pad_AndroidApplication
 * Method:    notifyEngineRenderDevice
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_us_toadlet_pad_AndroidApplication_notifyEngineRenderDevice
  (JNIEnv *, jobject);

/*
 * Class:     us_toadlet_pad_AndroidApplication
 * Method:    notifyEngineAudioDevice
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_us_toadlet_pad_AndroidApplication_notifyEngineAudioDevice
  (JNIEnv *, jobject);

/*
 * Class:     us_toadlet_pad_AndroidApplication
 * Method:    makeRenderDevice
 * Signature: ()Lus/toadlet/pad/RenderDevice;
 */
JNIEXPORT jobject JNICALL Java_us_toadlet_pad_AndroidApplication_makeRenderDevice
  (JNIEnv *, jobject);

/*
 * Class:     us_toadlet_pad_AndroidApplication
 * Method:    deleteRenderDevice
 * Signature: (Lus/toadlet/pad/RenderDevice;)V
 */
JNIEXPORT jobject JNICALL Java_us_toadlet_pad_AndroidApplication_deleteRenderDevice
  (JNIEnv *, jobject, jobject);

#ifdef __cplusplus
}
#endif
#endif