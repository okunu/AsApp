//
// Created by ok on 2017/10/15.
//
#include "ok_com_asapp_MainActivity.h"

const double PI = 3.1415926535898;

JNIEXPORT jstring JNICALL Java_ok_com_asapp_MainActivity_getStringFromNative
        (JNIEnv *env, jobject obj){
    char *str="String from native C";
    LOGI("test log 222222");
    return (*env)->NewStringUTF(env, str);
}

JNIEXPORT void JNICALL Java_ok_com_asapp_MainActivity_gaussBlue
(JNIEnv *env, jobject obj, jobject bitmap){
    AndroidBitmapInfo info;
    int ret;
    if((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0){
        LOGI("getbitmap info failed");
        return;
    }
    LOGI("bitmap w = %d, height = %d", info.width, info.height);
    int w = info.width;
    int h = info.height;
    void *srcBuf;
    if (ANDROID_BITMAP_RESULT_SUCCESS != AndroidBitmap_lockPixels(env, bitmap, &srcBuf)) {
        LOGI("lock src bitmap failed");
        return;
    }
    int *pix = (int *)srcBuf;

    int radius = 25;
    float sigma = (float)(1.0 * radius / 2.57);
    float deno = (float)(1.0 / (sigma * sqrt(2.0 * PI)));
    float nume = (float)(-1.0 / 2.0 * sigma * sigma);
    float* guassMatrix = (float*)malloc(sizeof(float) * (radius + radius + 1));
    float guassSum = 0.0;

    for(int i = 0, x = -radius; x < radius; ++x, ++i){
        float g = (float)(deno * exp(1.0 * nume * x * x));
        guassMatrix[i] = g;
        guassSum += g;
    }
    int len = radius + radius + 1;
    for(int i = 0; i < len; i++){
        guassMatrix[i] /= guassSum;
    }
    int* rowData = (int*)malloc(w * sizeof(int));
    int* listData = (int*)malloc(h * sizeof(int));
    for(int y = 0; y < h; ++y){
        memcpy(rowData, pix + y * w, sizeof(int) * w);
        for(int x = 0; x < w; ++x){
            float r = 0, g = 0, b = 0;
            guassSum = 0;
            for(int i = -radius; i <= radius; ++i){
                int k = x + i;
                if(0 <= k && k <= w){
                    int color = rowData[k];
                    int cr = (color & 0x00ff0000) >> 16;
                    int cg = (color & 0x0000ff00) >> 8;
                    int cb = (color & 0x000000ff);
                    r += cr * guassMatrix[i + radius];
                    g += cg * guassMatrix[i + radius];
                    b += cb * guassMatrix[i + radius];
                    guassSum += guassMatrix[i + radius];
                }
            }
            int cr = (int)(r / guassSum);
            int cg = (int)(g / guassSum);
            int cb = (int)(b / guassSum);
            pix[y * w + x] = cr << 16 | cg << 8 | cb | 0xff000000;
        }
    }

    for (int x = 0; x < w; ++x){
        for (int y = 0; y < h; ++y){
            listData[y] = pix[y * w + x];
        }
        for(int y =  0; y < h; ++y){
            float r = 0, g = 0, b = 0;
            guassSum = 0;
            for(int j = -radius; j <= radius; ++j){
                int k = y + j;
                if(0 <=k && k <= h){
                    int color = listData[k];
                    int cr = (color & 0x00ff0000) >> 16;
                    int cg = (color & 0x0000ff00) >> 8;
                    int cb = (color & 0x000000ff);
                    r += cr * guassMatrix[j + radius];
                    g += cg * guassMatrix[j + radius];
                    b += cb * guassMatrix[j + radius];
                    guassSum += guassMatrix[j + radius];
                }
            }
            int cr = (int)(r / guassSum);
            int cg = (int)(g / guassSum);
            int cb = (int)(b / guassSum);
            pix[y * w + x] = cr << 16 | cg << 8 | cb | 0xff000000;
        }
    }
    AndroidBitmap_unlockPixels(env, bitmap);
    free(guassMatrix);
    free(rowData);
    free(listData);
}
