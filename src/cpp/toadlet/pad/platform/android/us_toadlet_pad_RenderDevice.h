/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class us_toadlet_pad_RenderDevice */

#ifndef _Included_us_toadlet_pad_RenderDevice
#define _Included_us_toadlet_pad_RenderDevice
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     us_toadlet_pad_RenderDevice
 * Method:    create
 * Signature: (Lus/toadlet/peeper/RenderTarget;[I)Z
 */
JNIEXPORT jboolean JNICALL Java_us_toadlet_pad_RenderDevice_create
  (JNIEnv *, jobject, jobject, jintArray);

/*
 * Class:     us_toadlet_pad_RenderDevice
 * Method:    destroy
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_us_toadlet_pad_RenderDevice_destroy
  (JNIEnv *, jobject);

/*
 * Class:     us_toadlet_pad_RenderDevice
 * Method:    setRenderTarget
 * Signature: (Lus/toadlet/peeper/RenderTarget;)Z
 */
JNIEXPORT jboolean JNICALL Java_us_toadlet_pad_RenderDevice_setRenderTarget
  (JNIEnv *, jobject, jobject);

#ifdef __cplusplus
}
#endif
#endif