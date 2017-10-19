package com.okunu.app.gaussblur;

import android.graphics.Bitmap;

/**
 * Created by oukun.ok on 2017/10/18.
 */

public class BlurJni {
    public static native void  blurIntArray(int[] imgPix, int w, int h, int r);
    public static native void blurBitmap(Bitmap bitmap, int r);

    static {
        System.loadLibrary("ImageBlur");
    }
}
