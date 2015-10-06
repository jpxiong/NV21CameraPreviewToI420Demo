LOCAL_PATH := $(call my-dir)
MY_PATH := $(call my-dir)

include $(MY_PATH)/libyuv/Android.mk

LOCAL_PATH := $(MY_PATH)
include $(CLEAR_VARS)
LOCAL_MODULE    := libyuv_core
LOCAL_SRC_FILES := libyuvdemo.c
LOCAL_C_INCLUDES := $(MY_PATH)/ $(MY_PATH)/libyuv/include
LOCAL_LDLIBS := -llog
LOCAL_STATIC_LIBRARIES :=  libyuv_static
include $(BUILD_SHARED_LIBRARY)