/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_netease_space_antispam_picmatcher_PicMatcherImpl */

#ifndef _Included_com_netease_space_antispam_picmatcher_PicMatcherImpl
#define _Included_com_netease_space_antispam_picmatcher_PicMatcherImpl
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_netease_space_antispam_picmatcher_PicMatcherImpl
 * Method:    showSift
 * Signature: (Ljava/lang/String;Ljava/lang/String;ID)I
 */
JNIEXPORT jint JNICALL Java_com_netease_space_antispam_picmatcher_PicMatcherImpl_showSift
  (JNIEnv *, jobject, jstring, jstring, jint, jdouble);

/*
 * Class:     com_netease_space_antispam_picmatcher_PicMatcherImpl
 * Method:    siftImage
 * Signature: (Ljava/lang/String;Ljava/lang/String;ID)I
 */
JNIEXPORT jint JNICALL Java_com_netease_space_antispam_picmatcher_PicMatcherImpl_siftImage
  (JNIEnv *, jobject, jstring, jstring, jint, jdouble);

/*
 * Class:     com_netease_space_antispam_picmatcher_PicMatcherImpl
 * Method:    setUpIndex
 * Signature: (Ljava/lang/String;Ljava/lang/String;DDII)V
 */
JNIEXPORT void JNICALL Java_com_netease_space_antispam_picmatcher_PicMatcherImpl_setUpIndex
  (JNIEnv *, jobject, jstring, jstring, jdouble, jdouble, jint, jint);

/*
 * Class:     com_netease_space_antispam_picmatcher_PicMatcherImpl
 * Method:    query
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_netease_space_antispam_picmatcher_PicMatcherImpl_query
  (JNIEnv *, jobject, jstring, jstring, jstring);

#ifdef __cplusplus
}
#endif
#endif
