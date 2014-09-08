################Static Library libdscuda_tcp.a
LIB_LOCAL_PATH := $(call my-dir)
include /usr/local/DSCUDA/dscudapkg1.7.5.1/src/Android/jni/Android.mk
LOCAL_PATH := $(LIB_LOCAL_PATH)

################Static Library DS-CUDA Routine
include $(CLEAR_VARS)

LOCAL_MODULE	:= MR3call

LOCAL_CFLAGS := -O0 -g -P -ffast-math -funroll-loops -fpermissive -I. -I/usr/local/cuda/include \
-I/usr/local/cuda-4.1/NVIDIA_GPU_Computing_SDK/C/common/inc \
-I/usr/local/cuda/samples/common/inc -DTCP_ONLY=1

LOCAL_SRC_FILES :=  mr3.cpp
LOCAL_STATIC_LIBRARIES := dscuda_tcp1.7.5.1
LOCAL_LDLIBS := -ldl -llog 
include $(BUILD_STATIC_LIBRARY)
################Compile Claret main program
include $(CLEAR_VARS)

LOCAL_MODULE := claret

LOCAL_CFLAGS := -g -W -DANDROID_NDK -DDISABLE_IMPORTGL 

LOCAL_SRC_FILES := \
    claret.c \
    app-android.cpp \

LOCAL_STATIC_LIBRARIES := MR3call
LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog

include $(BUILD_SHARED_LIBRARY)
