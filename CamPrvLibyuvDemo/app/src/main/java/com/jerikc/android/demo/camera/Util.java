package com.jerikc.android.demo.camera;

import android.content.Context;
import android.view.Surface;
import android.view.WindowManager;

/**
 * Created by jerikc on 15/10/6.
 */
public class Util {
    public static int getDisplayDefaultRotation(Context ctx) {
        WindowManager windowManager =  (WindowManager) ctx.getSystemService(Context.WINDOW_SERVICE);
        return windowManager.getDefaultDisplay().getRotation();
    }

    public static int getDeviceRotationDegree(Context ctx) {
        switch (getDisplayDefaultRotation(ctx)) {
            // normal portrait
            case Surface.ROTATION_0:
                return 0;
            // expected landscape
            case Surface.ROTATION_90:
                return 90;
            // upside down portrait
            case Surface.ROTATION_180:
                return 180;
            // "upside down" landscape
            case Surface.ROTATION_270:
                return 270;
        }
        return 0;
    }
}
