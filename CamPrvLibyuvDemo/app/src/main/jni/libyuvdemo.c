#include <jni.h>
#include <string.h>
#include <stdio.h>
#include <android/log.h>
#include "com_jerikc_android_demo_camera_LibyuvCore.h"
#include "libyuv/include/libyuv.h"

#define MODUL_NAME "yuv_core"
#define LOG(msg, args...) __android_log_print(ANDROID_LOG_INFO, MODUL_NAME, msg, ## args)

FILE* outfile = NULL;
uint8_t *src_frame = NULL;
uint8_t *i420 = NULL;

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
    fwrite(p_buf, 1, size, outfile);
}

/*
 * Class:     com_jerikc_android_demo_camera_LibyuvCore
 * Method:    ConvertToI420
 * Signature: ([BII)V
 */
JNIEXPORT void JNICALL Java_com_jerikc_android_demo_camera_LibyuvCore_ConvertToI420
  (JNIEnv *env, jobject jThis, jbyteArray data, jint w, jint h) {
    int nv21_len = (*env)->GetArrayLength(env, data);
    if (!src_frame) {
        src_frame = (uint8_t*) malloc(nv21_len);
    }

    (*env)->GetByteArrayRegion(env, data, 0, nv21_len, src_frame);

    int Ysize = w * h;
    size_t src_size = Ysize * 1.5;

    if (!i420) {
        i420 = (uint8_t*) malloc(src_size);
    }

    uint8_t* pDstY = i420;
    uint8_t* pDstU = i420 + Ysize;
    uint8_t* pDstV = pDstU + (Ysize / 4);

    enum RotationMode mode = kRotate0;

    int retVal = ConvertToI420(src_frame, src_size,
            pDstY, w,
            pDstU, w / 2,
            pDstV, w / 2,
            0, 0,
            w, h,
            w, h,
            mode, FOURCC_NV21);

    LOG("%s retVal=%d", __FUNCTION__, retVal);
    if (!retVal) {
        write_data_to_file(i420, src_size);
    }

}

/*
 * Class:     com_jerikc_android_demo_camera_LibyuvCore
 * Method:    init
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_jerikc_android_demo_camera_LibyuvCore_init
  (JNIEnv *env, jobject jThis, jstring url) {
    const char* output_file_path = (*env)->GetStringUTFChars(env, url, (jboolean) 0);
    outfile = fopen(output_file_path, "wb");
    if (outfile == NULL) {
        throwJavaException(env, "java/io/IOException",
                "Could not open the output file");
        return;
    }
    (*env)->ReleaseStringUTFChars(env, url, output_file_path);
}

JNIEXPORT void JNICALL Java_com_jerikc_android_demo_camera_LibyuvCore_release(JNIEnv* env, jobject thiz) {

    if (src_frame) {
        free(src_frame);
        src_frame = NULL;
    }

    if (i420) {
        free(i420);
        i420 = NULL;
    }

    if (outfile) {
        fclose(outfile);
    }
}