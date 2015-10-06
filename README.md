# NV21CameraPreviewToI420Demo
A android demo which covert camera preview data in NV21 format to I420 by libyuv

## Description
There is a `test.yuv` in yuv420p format generated under `/sdcard/` path:

```
private static final String FILE_NAME = Environment.getExternalStorageDirectory().getAbsolutePath() + "/test.yuv";
```

You can use ffplay to play the `test.yuv` by the following command:

```
ffplay -v info -f rawvideo -pixel_format yuv420p -video_size 640x480  test.yuv 
```
