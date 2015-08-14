# Copyright (C) 2014 The QpidNetwork Project
# HttpClient Module Makefile
#
# Created on: 2014/10/27
# Author: Max.Chiu
# Email: Kingsleyyau@gmail.com
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := httpclient

LOCAL_MODULE_FILENAME := libhttpclient

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../third_party/openssl/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/../third_party/curl/include

LOCAL_CFLAGS = -fpermissive

LOCAL_STATIC_LIBRARIES += common
LOCAL_STATIC_LIBRARIES += http

#LOCAL_LDFLAGS += -L$(LOCAL_PATH)/../third_party/openssl/lib/$(TARGET_ARCH)

#LOCAL_LDLIBS += -llog -lz

LOCAL_CPPFLAGS  := -std=c++11

REAL_PATH := $(realpath $(LOCAL_PATH))
LOCAL_SRC_FILES := $(call all-cpp-files-under, $(REAL_PATH))

include $(BUILD_STATIC_LIBRARY)