LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    common.cpp \
    aps_wrapper.cpp \
    audio_policy.cpp

LOCAL_SHARED_LIBRARIES := \
    libhardware liblog libutils libaudioparameter

LOCAL_CFLAGS := -g -Wall -DCONVERT_AUDIO_DEVICES_T

LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_MODULE := audio_policy.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
#include $(BUILD_HEAPTRACKED_SHARED_LIBRARY)


include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    common.cpp \
    audio_hw.cpp

LOCAL_SHARED_LIBRARIES := \
    libhardware liblog libutils libaudioparameter

LOCAL_CFLAGS := -g -Wall -DCONVERT_AUDIO_DEVICES_T

LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_MODULE := audio.primary.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
#include $(BUILD_HEAPTRACKED_SHARED_LIBRARY)
