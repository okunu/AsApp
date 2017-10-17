//
// Created by ok on 2017/10/15.
//
#include "ok_com_asapp_MainActivity.h"
#include <ImageBlur.c>

const double PI = 3.1415926535898;

JNIEXPORT jstring JNICALL Java_ok_com_asapp_MainActivity_getStringFromNative
        (JNIEnv *env, jobject obj){
    char *str="String from native C";
    LOGI("test log 222222");
    return (*env)->NewStringUTF(env, str);
}

JNIEXPORT void JNICALL Java_ok_com_asapp_MainActivity_blurIntArray
(JNIEnv *env, jobject obj, jintArray array, jint w, jint h, jint radius){
    jint *pix;
    pix = (*env)->GetIntArrayElements(env, array, 0);

    pix = StackBlur(pix, w, h, radius);

    (*env)->ReleaseIntArrayElements(env, array, pix, 0);
}

JNIEXPORT void JNICALL Java_ok_com_asapp_MainActivity_gaussBlue
(JNIEnv *env, jobject obj, jobject bitmap){
    AndroidBitmapInfo info;
    int ret;
    if((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0){
        LOGI("getbitmap info failed");
        return;
    }
    int w = info.width;
    int h = info.height;
    LOGI("bitmap w = %d, height = %d", info.width, info.height);
    void *srcBuf;
    if (ANDROID_BITMAP_RESULT_SUCCESS != AndroidBitmap_lockPixels(env, bitmap, &srcBuf)) {
        LOGI("lock src bitmap failed");
        return;
    }
    int *pix = (int *)srcBuf;
    StackBlur(pix, w, h, 4);

    AndroidBitmap_unlockPixels(env, bitmap);
}
