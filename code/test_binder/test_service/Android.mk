# Copyright 2006 The Android Open Source Project

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	main.cpp \
	TestService.cpp \
	ITestServer.cpp

LOCAL_SHARED_LIBRARIES := \
        libutils \
        libbinder

base := $(LOCAL_PATH)/../..

LOCAL_MODULE:= test_service

include $(BUILD_EXECUTABLE)
