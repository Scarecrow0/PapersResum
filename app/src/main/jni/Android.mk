LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)


OpenCV_INSTALL_MODULES := on
OpenCV_CAMERA_MODULES := off

OPENCV_LIB_TYPE :=STATIC

ifeq ("$(wildcard $(OPENCV_MK_PATH))","")

#include ../../../../native/jni/OpenCV.mk
include /home/scarecrow/PapersResum/native/jni/OpenCV.mk

else
include $(OPENCV_MK_PATH)
endif

LOCAL_MODULE := MyCannyLib

LOCAL_SRC_FILES := canny_demo.cpp   \
                   canny_lib.cpp \
                   canny_lib_src/Line.cpp \
                   canny_lib_src/main.cpp \
                   canny_lib_src/MinHeap.cpp


#LOCAL_LDLIBS +=  -lm -llog
LOCAL_LDLIBS += -llog

include $(BUILD_SHARED_LIBRARY)