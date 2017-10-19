//
// Created by oukun.ok on 2017/10/18.
//
#include "com_okunu_app_gaussblur_BlurJni.h"
#include "GaussBlur.c"

#include <android/bitmap.h>
#include <math.h>


JNIEXPORT void JNICALL Java_com_okunu_app_gaussblur_BlurJni_blurIntArray
(JNIEnv *env, jclass obj, jintArray imgPix, jint w, jint h, jint r){
    jint* pix;
    pix = (*env)->GetIntArrayElements(env,imgPix, 0);
    if(pix == NULL){
        return;
    }
    gaussBlur(pix, w, h, r);
    (*env)->ReleaseIntArrayElements(env, imgPix, pix, 0);
}


JNIEXPORT void JNICALL Java_com_okunu_app_gaussblur_BlurJni_blurBitmap
(JNIEnv *env, jclass obj, jobject bitmap, jint r){
    AndroidBitmapInfo infoIn;
    void* buf;
    int ret;
    if((ret = AndroidBitmap_getInfo(env, bitmap, &infoIn)) < 0){
        return;
    }
    if((ret = AndroidBitmap_lockPixels(env, bitmap, &buf)) < 0){
        return;
    }
    int w = infoIn.width;
    int h = infoIn.height;
    int *pix = (int *)buf;
    gaussBlur(pix, w, h, r);

    AndroidBitmap_unlockPixels(env, bitmap);
}