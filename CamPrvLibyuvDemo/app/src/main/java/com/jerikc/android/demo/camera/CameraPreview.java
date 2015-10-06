package com.jerikc.android.demo.camera;

/**
 * Created by jerikc on 15/10/6.
 */

import android.content.Context;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.os.Environment;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.io.IOException;
import java.nio.ByteBuffer;

/** A basic Camera preview class */
public class CameraPreview extends SurfaceView implements SurfaceHolder.Callback {
    private static final String TAG = "CameraPreview";

    private static final String FILE_NAME = Environment.getExternalStorageDirectory().getAbsolutePath() + "/test.yuv";

    private SurfaceHolder mHolder;
    private Camera.CameraInfo mCameraInfo;
    private Camera mCamera;
    private Context mContext;
    private PreviewCallback mPreviewCallback;
    private LibyuvCore mLibyuvCore;

    public CameraPreview(Context context, Camera camera, Camera.CameraInfo cameraInfo) {
        super(context);
        mCamera = camera;
        mCameraInfo = cameraInfo;
        mContext = context;

        // Install a SurfaceHolder.Callback so we get notified when the
        // underlying surface is created and destroyed.
        mHolder = getHolder();
        mHolder.addCallback(this);
        // deprecated setting, but required on Android versions prior to 3.0
        mHolder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);

        mPreviewCallback = new PreviewCallback();
        mLibyuvCore = new LibyuvCore();


    }

    private void adjustCameraOrientation() {
        Log.d(TAG, "handle setting camera orientation, mCameraInfo.facing:" + mCameraInfo.facing);
        int degrees = Util.getDeviceRotationDegree(mContext);
        int result;
        if (mCameraInfo.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
            result = (mCameraInfo.orientation + degrees) % 360;
            result = (360 - result) % 360;  // compensate the mirror
        } else {  // back-facing
            result = (mCameraInfo.orientation - degrees + 360) % 360;
        }

        mCamera.setDisplayOrientation(result);
    }

    public void surfaceCreated(SurfaceHolder holder) {
        // The Surface has been created, now tell the camera where to draw the preview.
        try {
            mCamera.setPreviewDisplay(holder);
            adjustCameraOrientation();

            int width = 640;
            int height = 480;

            Camera.Parameters params = mCamera.getParameters();
            params.setPreviewFormat(ImageFormat.NV21);
            params.setPreviewSize(width, height);
            mCamera.setParameters(params);

            if (!Util.isLandscape(mContext)) {
                // port
                int temp = width;
                width = height;
                height = temp;
            }

            int prvWidth = params.getPreviewSize().width;
            int prvHeight = params.getPreviewSize().height;
            mLibyuvCore.init(prvWidth, prvHeight, width, height, FILE_NAME);

            Log.i(TAG, "width:" + width + ", height:" + height + ",prvWidth:" + prvWidth + ",prvHeight:" + prvHeight);

            int previewBufferSize = prvWidth * prvHeight + (prvWidth * prvHeight + 1) / 2;

            for (int i = 0; i < 3; i++) {
                mCamera.addCallbackBuffer(ByteBuffer.allocateDirect(previewBufferSize).array());
            }

            mCamera.setPreviewCallback(mPreviewCallback);
            Log.i(TAG, "setPreviewCallback");
            mCamera.startPreview();
        } catch (IOException e) {
            Log.d(TAG, "Error setting camera preview: " + e.getMessage());
        }
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
        if (mLibyuvCore != null) {
            mLibyuvCore.release();
        }
    }

    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        // If your preview can change or rotate, take care of those events here.
        // Make sure to stop the preview before resizing or reformatting it.

        if (mHolder.getSurface() == null){
            // preview surface does not exist
            return;
        }

        // stop preview before making changes
        try {
            mCamera.stopPreview();
        } catch (Exception e){
            // ignore: tried to stop a non-existent preview
        }

        // set preview size and make any resize, rotate or
        // reformatting changes here

        // start preview with new settings
        try {
            mCamera.setPreviewDisplay(mHolder);
            mCamera.setPreviewCallback(mPreviewCallback); // set again
            Log.i(TAG, "setPreviewCallback again");
            mCamera.startPreview();

            Log.i(TAG, "preview format:" + mCamera.getParameters().getPreviewFormat());

        } catch (Exception e){
            Log.d(TAG, "Error starting camera preview: " + e.getMessage());
        }
    }

    private class PreviewCallback implements Camera.PreviewCallback {

        @Override
        public void onPreviewFrame(byte[] data, Camera camera) {
            if (data == null) {
                return;
            }
            mLibyuvCore.ConvertToI420(data, mCameraInfo);
            camera.addCallbackBuffer(data);
        }
    }
}
