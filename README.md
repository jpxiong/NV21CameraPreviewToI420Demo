# NV21CameraPreviewToI420Demo
The demo is an android project which covert camera preview data in NV21 format to I420 by libyuv.

## Description
The `test.yuv` file in yuv420p format will be generated under `/sdcard/` path:

```
private static final String FILE_NAME = Environment.getExternalStorageDirectory().getAbsolutePath() + "/test.yuv";
```

You can use ffplay to play the `test.yuv` by the following command:

```
ffplay -v info -f rawvideo -pixel_format yuv420p -video_size 640x480  test.yuv 
```
