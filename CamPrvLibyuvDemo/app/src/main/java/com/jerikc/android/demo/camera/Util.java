package com.jerikc.android.demo.camera;

import android.content.Context;
import android.content.res.Configuration;
import android.graphics.Point;
import android.view.Display;
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

    public static Point getScreenSize(Context context) {
        Point size = new Point();
        WindowManager wm = (WindowManager) context.getSystemService(Context.WINDOW_SERVICE);
        Display display = wm.getDefaultDisplay();
        display.getSize(size);
        return size;
    }

    public static boolean isLandscape(Context ctx) {
        Point size = getScreenSize(ctx);
        if (size.x > size.y) {
            // land
            return true;
        }
        // port
        return false;
    }
}
