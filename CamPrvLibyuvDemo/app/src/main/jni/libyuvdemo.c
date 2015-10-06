#include <jni.h>
#include <string.h>
#include <stdio.h>
#include <android/log.h>
#include "com_jerikc_android_demo_camera_LibyuvCore.h"
#include "libyuv/include/libyuv.h"

#define MODUL_NAME "yuv_core"
#define LOG(msg, args...) __android_log_print(ANDROID_LOG_INFO, MODUL_NAME, msg, ## args)

typedef struct YuvCoreContext {
    FILE* outfile;
    uint8_t *src_frame;
    size_t src_size;
    uint8_t *i420;
    size_t i420_size;
    int src_width;
    int src_height;
    int dst_width;
    int dst_height;
} YuvCoreContext;

struct YuvCoreContext *pYuvCoreContext = NULL;

/**
 * throw java exceptions
 **/
void throwJavaException(JNIEnv* env, const char *name, const char *msg) {
    jclass cls = (*env)->FindClass(env, name);
    if (cls != NULL )
        (*env)->ThrowNew(env, cls, msg);
    (*env)->DeleteLocalRef(env, cls);
}

static void write_data_to_file(uint8_t *p_buf, int size) {
    fwrite(p_buf, 1, size, pYuvCoreContext->outfile);
}

/*
 * Class:     com_jerikc_android_demo_camera_LibyuvCore
 * Method:    ConvertToI420
 * Signature: ([BII)V
 */
JNIEXPORT void JNICALL Java_com_jerikc_android_demo_camera_LibyuvCore_ConvertToI420
  (JNIEnv *env, jobject jThis, jbyteArray data, jobject cameraInfo) {
    int nv21_len = (*env)->GetArrayLength(env, data);

    if (pYuvCoreContext->src_size != nv21_len) {
        // free the old one
        free(pYuvCoreContext->src_frame);
        pYuvCoreContext->src_frame = NULL;

        // malloc the new one
        pYuvCoreContext->src_frame = (uint8_t*) malloc(nv21_len);
        pYuvCoreContext->src_size = nv21_len;
    }

    (*env)->GetByteArrayRegion(env, data, 0, nv21_len, pYuvCoreContext->src_frame);

    jclass camera_info_class = (*env)->GetObjectClass(env, cameraInfo);
    jfieldID field_id = (*env)->GetFieldID(env, camera_info_class,
            "orientation", "I");
    jint rotation = (*env)->GetIntField(env, cameraInfo, field_id);
    field_id = (*env)->GetFieldID(env, camera_info_class, "facing", "I");
    jint facing = (*env)->GetIntField(env, cameraInfo, field_id);
    (*env)->DeleteLocalRef(env, camera_info_class);

    int Ysize = pYuvCoreContext->dst_width * pYuvCoreContext->dst_height;
    size_t i420_size = Ysize * 3 / 2;

    if (pYuvCoreContext->i420_size != i420_size) {
        // free the old one
        free(pYuvCoreContext->i420);
        pYuvCoreContext->i420 = NULL;

        // malloc the new one
        pYuvCoreContext->i420 = (uint8_t*) malloc(i420_size);
        pYuvCoreContext->i420_size = i420_size;
    }

    int rotate = rotation;

    uint8_t* pDstY = pYuvCoreContext->i420;
    uint8_t* pDstU = pYuvCoreContext->i420 + Ysize;  // w * h >> 2
    uint8_t* pDstV = pDstU + (Ysize >> 2); // w * h >> 2

    /*
    int ConvertToI420(const uint8* src_frame, size_t src_size,
              uint8* dst_y, int dst_stride_y,
              uint8* dst_u, int dst_stride_u,
              uint8* dst_v, int dst_stride_v,
              int crop_x, int crop_y,
              int src_width, int src_height,
              int dst_width, int dst_height,
              enum RotationMode rotation,
              uint32 format);
     */
    int retVal = ConvertToI420(pYuvCoreContext->src_frame, pYuvCoreContext->src_size,
            pDstY, pYuvCoreContext->dst_width,
            pDstU, (pYuvCoreContext->dst_width + 1) / 2,
            pDstV, (pYuvCoreContext->dst_width + 1) / 2,
            0, 0,
            pYuvCoreContext->src_width, pYuvCoreContext->src_height,
            pYuvCoreContext->src_width, pYuvCoreContext->src_height,
            rotate, FOURCC_NV21);

    LOG("%s retVal=%d", __FUNCTION__, retVal);
    if (!retVal) {
        write_data_to_file(pYuvCoreContext->i420, pYuvCoreContext->i420_size);
    }
}

/*
 * Class:     com_jerikc_android_demo_camera_LibyuvCore
 * Method:    init
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_jerikc_android_demo_camera_LibyuvCore_init
  (JNIEnv *env, jobject jThis, jint src_w, jint src_h, jint dst_w, jint dst_h, jstring url) {
    const char* output_file_path = (*env)->GetStringUTFChars(env, url, (jboolean) 0);

    pYuvCoreContext = (YuvCoreContext *)malloc(sizeof(YuvCoreContext));
    pYuvCoreContext->src_width = src_w;
    pYuvCoreContext->src_height = src_h;
    pYuvCoreContext->dst_width = dst_w;
    pYuvCoreContext->dst_height = dst_h;
    pYuvCoreContext->src_frame = NULL;
    pYuvCoreContext->src_size = 0;
    pYuvCoreContext->i420 = NULL;
    pYuvCoreContext->i420_size = 0;

    pYuvCoreContext->outfile = fopen(output_file_path, "wb");
    if (!pYuvCoreContext->outfile) {
        free(pYuvCoreContext);
        throwJavaException(env, "java/io/IOException",
                "Could not open the output file");
        return;
    }
    (*env)->ReleaseStringUTFChars(env, url, output_file_path);

}

JNIEXPORT void JNICALL Java_com_jerikc_android_demo_camera_LibyuvCore_release(JNIEnv* env, jobject thiz) {

    if (pYuvCoreContext->src_frame) {
        free(pYuvCoreContext->src_frame);
        pYuvCoreContext->src_frame = NULL;
    }

    if (pYuvCoreContext->i420) {
        free(pYuvCoreContext->i420);
        pYuvCoreContext->i420 = NULL;
    }

    if (pYuvCoreContext->outfile) {
        fclose(pYuvCoreContext->outfile);
    }

    if (pYuvCoreContext) {
        free(pYuvCoreContext);
        pYuvCoreContext = NULL;
    }
}