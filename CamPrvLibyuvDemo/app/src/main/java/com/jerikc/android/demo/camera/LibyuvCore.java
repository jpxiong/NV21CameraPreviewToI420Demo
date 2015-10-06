package com.jerikc.android.demo.camera;

/**
 * Created by jerikc on 15/10/6.
 */
public class LibyuvCore {
    static {
        System.loadLibrary("yuv_core");
    }
    public native void ConvertToI420(byte[] nv21, int w, int h);
    public native void init(String url);
    public native void release();
}
