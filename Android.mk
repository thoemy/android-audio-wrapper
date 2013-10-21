LOCAL_PATH := $(call my-dir)

include $(LOCAL_PATH)/config.mk

L_CFLAGS := -g -Wall

#
# Process config
#

ifeq ($(FRAMEWORK_EXPECTS_ICS_AUDIO_BLOB),true)
  L_CFLAGS += -DICS_AUDIO_BLOB
else
  L_CFLAGS += -DCONVERT_AUDIO_DEVICES_T
endif

ifneq ($(BUILD_AUDIO_POLICY_WRAPPER),true)
  ifeq ($(HTC_ICS_AUDIO_BLOB),true)
    L_CFLAGS += -DNO_HTC_POLICY_MANAGER
  endif
endif

#
# Audio Policy Wrapper
#
ifeq ($(BUILD_AUDIO_POLICY_WRAPPER), true)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    common.cpp \
    aps_wrapper.cpp \
    audio_policy.cpp

LOCAL_SHARED_LIBRARIES := \
    libhardware liblog libutils
LOCAL_STATIC_LIBRARIES := libmedia_helper

LOCAL_CFLAGS := $(L_CFLAGS)

LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_MODULE := audio_policy.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
#include $(BUILD_HEAPTRACKED_SHARED_LIBRARY)
endif

#
# Audio HW Wrapper
#
ifeq ($(BUILD_AUDIO_HW_WRAPPER), true)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    common.cpp \
    audio_hw.cpp

LOCAL_SHARED_LIBRARIES := \
    libhardware liblog libutils
LOCAL_STATIC_LIBRARIES := libmedia_helper

LOCAL_CFLAGS := $(L_CFLAGS)

LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_MODULE := audio.primary.$(TARGET_BOARD_PLATFORM)
LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)
#include $(BUILD_HEAPTRACKED_SHARED_LIBRARY)
endif
