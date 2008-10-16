#include "siftfeat.h"
#include "LSHMain.h"
#include <string.h>
#include <stdlib.h>
#include "com_netease_space_antispam_picmatcher_PicMatcherImpl.h"

//jstring to char*
char* jstringTostring(JNIEnv* env, jstring jstr)
{
	char* rtn = NULL;
	jclass clsstring = env->FindClass("java/lang/String");
	jstring strencode = env->NewStringUTF("utf-8");
	jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
	jbyteArray barr= (jbyteArray)env->CallObjectMethod(jstr, mid, strencode);
	jsize alen = env->GetArrayLength(barr);
	jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
	if (alen > 0)
	{
		rtn = (char*)malloc(alen + 1);
		memcpy(rtn, ba, alen);
		rtn[alen] = 0;
	}
	env->ReleaseByteArrayElements(barr, ba, 0);
	return rtn;
} 

/*
* Class:     com_netease_space_antispam_picmatcher_PicMatcherImpl
* Method:    showSift
* Signature: (Ljava/lang/String;Ljava/lang/String;ID)I
*/
JNIEXPORT jint JNICALL Java_com_netease_space_antispam_picmatcher_PicMatcherImpl_showSift
(JNIEnv * env, jobject object, jstring imagenamefile, jstring out_file_name, jint img_dbl, jdouble contr_thr)
{
	
	char* myImagenamefile = jstringTostring(env, imagenamefile);
	char* myOut_file_name = jstringTostring(env, out_file_name);
	int re = showSift(myImagenamefile, myOut_file_name, img_dbl, contr_thr);
	free(myImagenamefile);
	free(myOut_file_name);

	return re;
}

/*
* Class:     com_netease_space_antispam_picmatcher_PicMatcherImpl
* Method:    siftImage
* Signature: (Ljava/lang/String;Ljava/lang/String;ID)I
*/
JNIEXPORT jint JNICALL Java_com_netease_space_antispam_picmatcher_PicMatcherImpl_siftImage
(JNIEnv * env, jobject object, jstring imagename, jstring out_file_name, jint img_dbl, jdouble contr_thr, jlong id)
{
	char* myImagename = jstringTostring(env, imagename);
	char* myOut_file_name = jstringTostring(env, out_file_name);
	int re = siftImage(myImagename, myOut_file_name, img_dbl, contr_thr, id);
	free(myImagename);
	free(myOut_file_name);
	return re;
}

/*
* Class:     com_netease_space_antispam_picmatcher_PicMatcherImpl
* Method:    setUpIndex
* Signature: (Ljava/lang/String;Ljava/lang/String;DDII)V
*/
JNIEXPORT void JNICALL Java_com_netease_space_antispam_picmatcher_PicMatcherImpl_setUpIndex
(JNIEnv * env, jobject object, jstring dataFile, jstring index, jdouble R, jdouble W, jint K, jint L)
{
	char* myDataFile = jstringTostring(env, dataFile);
	char* myIndex = jstringTostring(env, index);
	setUpIndex(myDataFile, myIndex, R, W, K, L);
	free(myDataFile);
	free(myIndex);
}

/*
* Class:     com_netease_space_antispam_picmatcher_PicMatcherImpl
* Method:    addToIndex
* Signature: (Ljava/lang/String;Ljava/lang/String;)V
*/
JNIEXPORT void JNICALL Java_com_netease_space_antispam_picmatcher_PicMatcherImpl_addToIndex
(JNIEnv * env, jobject object, jstring dataFile, jstring index)
{
	char* myDataFile = jstringTostring(env, dataFile);
	char* myIndex = jstringTostring(env, index);
	addToIndex(myDataFile, myIndex);
	free(myDataFile);
	free(myIndex);
}

/*
* Class:     com_netease_space_antispam_picmatcher_PicMatcherImpl
* Method:    query
* Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V
*/
JNIEXPORT void JNICALL Java_com_netease_space_antispam_picmatcher_PicMatcherImpl_query
(JNIEnv * env, jobject object, jstring queryFile, jstring index, jstring output)
{
	char* myQueryFile = jstringTostring(env, queryFile);
	char* myIndex = jstringTostring(env, index);
	char* myOutput = jstringTostring(env, output);
	query(myQueryFile, myIndex, myOutput);
	free(myQueryFile);
	free(myIndex);
	free(myOutput);
}


