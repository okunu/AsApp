//
// Created by oukun.ok on 2017/10/18.
//
#define PI 3.1415926

#include <jni.h>
#include <android/log.h>
#include<malloc.h>
#include <math.h>
#include <string.h>

#define LOG_TAG "okunu"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG, __VA_ARGS__)

#define ABS(a) ((a)<(0)?(-a):(a))
#define MAX(a, b) ((a)>(b)?(a):(b))
#define MIN(a, b) ((a)<(b)?(a):(b))

static void gaussBlur(int *pix, int w, int h, int radius) {
    //分别计算正态分布各参数，sigma与高斯模糊半径有关系，2.57既是1除以根号2 PI得来
    float sigma = 1.0 * radius / 2.57;
    float deno = 1.0 / sigma * sqrt(2.0 * PI);
    float nume = -1.0 / (2.0 * sigma * sigma);

    //计算正态分布下的权重值
    float *gaussMatrix = (float *) malloc(sizeof(float) * (radius + radius + 1));
    float gaussSum = 0.0;
    for (int i = 0, x = -radius; x <= radius; ++x, ++i) {
        float g = deno * exp(1.0 * nume * x * x);
        // 注意 x 与 i 的对应关系，x 是相对距离
        gaussMatrix[i] = g;
        gaussSum += g;
    }

    //因为没有计算所有的周围点，权重值和不为1，所以需要归一化。
    int len = radius + radius + 1;
    for (int i = 0; i < len; ++i) {
        gaussMatrix[i] /= gaussSum;
    }
    //申请两个数组，分别用于存放一行或一列的像素值
    int *rowData = (int *) malloc(w * sizeof(int));
    int *listData = (int *) malloc(h * sizeof(int));
    //开始x轴方向模糊
    for (int y = 0; y < h; ++y) {
        //取一行像素数据，注意像素总数组的访问方式是  x + y * w
        memcpy(rowData, pix + y * w, sizeof(int) * w);
        for (int x = 0; x < w; ++x) {
            float r = 0, g = 0, b = 0;
            gaussSum = 0;
            //以当前坐标点 x、y 为中心，查看前后一个模糊半径的周围点，根据正态分布
            //重新计算像素点的颜色值
            for (int i = -radius; i <= radius; ++i) {
                // k 表示周围点的真实坐标
                int k = x + i;
                // 边界上的像素点，它的周围点只有正常的一半，所以要保证 k 的取值范围
                if (k >= 0 && k <= w) {
                    // 取到周围点的像素，并根据 argb 的排列方式，计算 r、g、b分量
                    int color = rowData[k];
                    int cr = (color & 0x00ff0000) >> 16;
                    int cg = (color & 0x0000ff00) >> 8;
                    int cb = (color & 0x000000ff);
                    //真实点坐标为 k，与它对应的权重数组下标是 i + radius
                    //前文中计算正态分布权重时已经说明相关的对应关系。
                    //根据正态分布的权重关系，计算中心点的 r g b各分量
                    int index = i + radius;
                    r += cr * gaussMatrix[index];
                    g += cg * gaussMatrix[index];
                    b += cb * gaussMatrix[index];
                    gaussSum += gaussMatrix[index];
                }
            }
            //因为边界点的存在，gaussSum值不一定为1，所以需要除以gaussSum，归一化。
            int cr = (int) (r / gaussSum);
            int cg = (int) (g / gaussSum);
            int cb = (int) (b / gaussSum);
            //根据权重值与各周围点像素相乘之和，得到新的中间点像素。
            pix[y * w + x] = cr << 16 | cg << 8 | cb | 0xff000000;
        }
    }
    //y轴方向高斯模糊
    for (int x = 0; x < w; ++x) {
        //复制一列颜色数据到数组中
        for (int y = 0; y < h; ++y) {
            listData[y] = pix[y * w + x];
        }
        for (int y = 0; y < h; ++y) {
            float r = 0, g = 0, b = 0;
            gaussSum = 0;
            for (int j = -radius; j <= radius; ++j) {
                int k = y + j;
                if (k >= 0 && k <= h) {
                    int color = listData[k];
                    int cr = (color & 0x00ff0000) >> 16;
                    int cg = (color & 0x0000ff00) >> 8;
                    int cb = (color & 0x000000ff);

                    int index = j + radius;

                    r += cr * gaussMatrix[index];
                    g += cg * gaussMatrix[index];
                    b += cb * gaussMatrix[index];

                    gaussSum += gaussMatrix[index];
                }
            }
            int cr = (int) (r / gaussSum);
            int cg = (int) (g / gaussSum);
            int cb = (int) (b / gaussSum);

            pix[y * w + x] = cr << 16 | cg << 8 | cb | 0xff000000;
        }
    }
    free(gaussMatrix);
    free(rowData);
    free(listData);
}

static void gaussBlur1(int *pix, int w, int h, int radius) {
    float sigma = 1.0 * radius / 2.57;    //2.57 * sigam半径之后基本没有贡献 所以取sigma为 r / 2.57
    float deno = 1.0 / (sigma * sqrt(2.0 * PI));
    float nume = -1.0 / (2.0 * sigma * sigma);

    //高斯分布产生的数组
    float *gaussMatrix = (float *) malloc(sizeof(float) * (radius + radius + 1));
    float gaussSum = 0.0;
    for (int i = 0, x = -radius; x <= radius; ++x, ++i) {
        float g = deno * exp(1.0 * nume * x * x);

        gaussMatrix[i] = g;
        gaussSum += g;
    }

    //归1话
    int len = radius + radius + 1;
    for (int i = 0; i < len; ++i)
        gaussMatrix[i] /= gaussSum;

    //临时存储 一行的数据
    int *rowData = (int *) malloc(w * sizeof(int));
    int *listData = (int *) malloc(h * sizeof(int));

    //x方向的模糊
    for (int y = 0; y < h; ++y) {
        //拷贝一行数据
        memcpy(rowData, pix + y * w, sizeof(int) * w);

        for (int x = 0; x < w; ++x) {
            float r = 0, g = 0, b = 0;
            gaussSum = 0;

            for (int i = -radius; i <= radius; ++i) {
                int k = x + i;

                if (0 <= k && k <= w) {
                    //得到像素点的rgb值
                    int color = rowData[k];
                    int cr = (color & 0x00ff0000) >> 16;
                    int cg = (color & 0x0000ff00) >> 8;
                    int cb = (color & 0x000000ff);

                    r += cr * gaussMatrix[i + radius];
                    g += cg * gaussMatrix[i + radius];
                    b += cb * gaussMatrix[i + radius];

                    gaussSum += gaussMatrix[i + radius];
                }
            }

            int cr = (int) (r / gaussSum);
            int cg = (int) (g / gaussSum);
            int cb = (int) (b / gaussSum);

            pix[y * w + x] = cr << 16 | cg << 8 | cb | 0xff000000;
        }
    }

    for (int x = 0; x < w; ++x) {
        //拷贝 一列 数据
        for (int y = 0; y < h; ++y)
            listData[y] = pix[y * w + x];

        for (int y = 0; y < h; ++y) {
            float r = 0, g = 0, b = 0;
            gaussSum = 0;

            for (int j = -radius; j <= radius; ++j) {
                int k = y + j;

                if (0 <= k && k <= h) {
                    int color = listData[k];
                    int cr = (color & 0x00ff0000) >> 16;
                    int cg = (color & 0x0000ff00) >> 8;
                    int cb = (color & 0x000000ff);

                    r += cr * gaussMatrix[j + radius];
                    g += cg * gaussMatrix[j + radius];
                    b += cb * gaussMatrix[j + radius];

                    gaussSum += gaussMatrix[j + radius];
                }
            }

            int cr = (int) (r / gaussSum);
            int cg = (int) (g / gaussSum);
            int cb = (int) (b / gaussSum);

            pix[y * w + x] = cr << 16 | cg << 8 | cb | 0xff000000;
        }
    }

    //清理内存
    free(gaussMatrix);
    free(rowData);
    free(listData);
}

static int *StackBlur(int *pix, int w, int h, int radius) {
    int wm = w - 1;
    int hm = h - 1;
    int wh = w * h;
    int div = radius + radius + 1;

    int *r = (int *) malloc(wh * sizeof(int));
    int *g = (int *) malloc(wh * sizeof(int));
    int *b = (int *) malloc(wh * sizeof(int));
    int rsum, gsum, bsum, x, y, i, p, yp, yi, yw;

    int *vmin = (int *) malloc(MAX(w, h) * sizeof(int));

    int divsum = (div + 1) >> 1;
    divsum *= divsum;
    int *dv = (int *) malloc(256 * divsum * sizeof(int));
    for (i = 0; i < 256 * divsum; i++) {
        dv[i] = (i / divsum);
    }

    yw = yi = 0;

    int(*stack)[3] = (int (*)[3]) malloc(div * 3 * sizeof(int));
    int stackpointer;
    int stackstart;
    int *sir;
    int rbs;
    int r1 = radius + 1;
    int routsum, goutsum, boutsum;
    int rinsum, ginsum, binsum;

    for (y = 0; y < h; y++) {
        rinsum = ginsum = binsum = routsum = goutsum = boutsum = rsum = gsum = bsum = 0;
        for (i = -radius; i <= radius; i++) {
            p = pix[yi + (MIN(wm, MAX(i, 0)))];
            sir = stack[i + radius];
            sir[0] = (p & 0xff0000) >> 16;
            sir[1] = (p & 0x00ff00) >> 8;
            sir[2] = (p & 0x0000ff);

            rbs = r1 - ABS(i);
            rsum += sir[0] * rbs;
            gsum += sir[1] * rbs;
            bsum += sir[2] * rbs;
            if (i > 0) {
                rinsum += sir[0];
                ginsum += sir[1];
                binsum += sir[2];
            } else {
                routsum += sir[0];
                goutsum += sir[1];
                boutsum += sir[2];
            }
        }
        stackpointer = radius;

        for (x = 0; x < w; x++) {

            r[yi] = dv[rsum];
            g[yi] = dv[gsum];
            b[yi] = dv[bsum];

            rsum -= routsum;
            gsum -= goutsum;
            bsum -= boutsum;

            stackstart = stackpointer - radius + div;
            sir = stack[stackstart % div];

            routsum -= sir[0];
            goutsum -= sir[1];
            boutsum -= sir[2];

            if (y == 0) {
                vmin[x] = MIN(x + radius + 1, wm);
            }
            p = pix[yw + vmin[x]];

            sir[0] = (p & 0xff0000) >> 16;
            sir[1] = (p & 0x00ff00) >> 8;
            sir[2] = (p & 0x0000ff);

            rinsum += sir[0];
            ginsum += sir[1];
            binsum += sir[2];

            rsum += rinsum;
            gsum += ginsum;
            bsum += binsum;

            stackpointer = (stackpointer + 1) % div;
            sir = stack[(stackpointer) % div];

            routsum += sir[0];
            goutsum += sir[1];
            boutsum += sir[2];

            rinsum -= sir[0];
            ginsum -= sir[1];
            binsum -= sir[2];

            yi++;
        }
        yw += w;
    }
    for (x = 0; x < w; x++) {
        rinsum = ginsum = binsum = routsum = goutsum = boutsum = rsum = gsum = bsum = 0;
        yp = -radius * w;
        for (i = -radius; i <= radius; i++) {
            yi = MAX(0, yp) + x;

            sir = stack[i + radius];

            sir[0] = r[yi];
            sir[1] = g[yi];
            sir[2] = b[yi];

            rbs = r1 - ABS(i);

            rsum += r[yi] * rbs;
            gsum += g[yi] * rbs;
            bsum += b[yi] * rbs;

            if (i > 0) {
                rinsum += sir[0];
                ginsum += sir[1];
                binsum += sir[2];
            } else {
                routsum += sir[0];
                goutsum += sir[1];
                boutsum += sir[2];
            }

            if (i < hm) {
                yp += w;
            }
        }
        yi = x;
        stackpointer = radius;
        for (y = 0; y < h; y++) {
            // Preserve alpha channel: ( 0xff000000 & pix[yi] )
            pix[yi] = (0xff000000 & pix[yi]) | (dv[rsum] << 16) | (dv[gsum] << 8) | dv[bsum];

            rsum -= routsum;
            gsum -= goutsum;
            bsum -= boutsum;

            stackstart = stackpointer - radius + div;
            sir = stack[stackstart % div];

            routsum -= sir[0];
            goutsum -= sir[1];
            boutsum -= sir[2];

            if (x == 0) {
                vmin[y] = MIN(y + r1, hm) * w;
            }
            p = x + vmin[y];

            sir[0] = r[p];
            sir[1] = g[p];
            sir[2] = b[p];

            rinsum += sir[0];
            ginsum += sir[1];
            binsum += sir[2];

            rsum += rinsum;
            gsum += ginsum;
            bsum += binsum;

            stackpointer = (stackpointer + 1) % div;
            sir = stack[stackpointer];

            routsum += sir[0];
            goutsum += sir[1];
            boutsum += sir[2];

            rinsum -= sir[0];
            ginsum -= sir[1];
            binsum -= sir[2];

            yi += w;
        }
    }

    free(r);
    free(g);
    free(b);
    free(vmin);
    free(dv);
    free(stack);
    return (pix);
}