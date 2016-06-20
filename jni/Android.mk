LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS    := -Wall
LOCAL_MODULE    := busybox

BUSYBOX_PATH := ../busybox
BUSYBOX_SRCS := $(BUSYBOX_PATH)/atomicio.c \
	$(BUSYBOX_PATH)/bignum.c XXX

LOCAL_SRC_FILES := $(BUSYBOX_SRCS)
LOCAL_C_INCLUDES:= busybox busybox/include XXX
LOCAL_LDLIBS    := -lz XXX
LOCAL_LDFLAGS   := -static

include $(BUILD_EXECUTABLE)
