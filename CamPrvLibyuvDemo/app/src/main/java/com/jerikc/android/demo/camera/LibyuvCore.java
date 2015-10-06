package com.jerikc.android.demo.camera;

import android.hardware.Camera.CameraInfo;

/**
 * Created by jerikc on 15/10/6.
 */
public class LibyuvCore {
    static {
        System.loadLibrary("yuv_core");
    }
    public native void ConvertToI420(byte[] nv21, CameraInfo info);
    public native void init(int srcW, int srcH, int dstW, int dstH, String url);
    public native void release();
}
