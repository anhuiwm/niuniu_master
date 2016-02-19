LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libevent_static

LOCAL_MODULE_FILENAME := libevent

LOCAL_SRC_FILES := \
evutil_rand.c \
event.c \
evthread.c \
buffer.c \
bufferevent.c \
bufferevent_sock.c \
bufferevent_filter.c \
bufferevent_pair.c \
listener.c \
bufferevent_ratelim.c \
evmap.c \
log.c \
evutil.c \
strlcpy.c \
epoll.c \
select.c \
poll.c \
signal.c \
netkit/net_common.cpp \
netkit/connector.cpp \
netkit/listner.cpp \
netkit/read_callback.cpp 



LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/include \
                    $(LOCAL_PATH)/compat \
                    $(LOCAL_PATH)/include 

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/include \
                    $(LOCAL_PATH)/compat \
                    $(LOCAL_PATH)/include 

#LOCAL_LDLIBS := -lrt
#LOCAL_EXPORT_LDLIBS := -lrt

#LOCAL_WHOLE_STATIC_LIBRARIES := cocos_libpng_static

# define the macro to compile through support/zip_support/ioapi.c
LOCAL_CFLAGS := -DHAVE_CONFIG_H
LOCAL_EXPORT_CFLAGS := -DHAVE_CONFIG_H

include $(BUILD_STATIC_LIBRARY)

#$(call import-module,libjpeg)
#$(call import-module,libpng)
#$(call import-module,libtiff)
#$(call import-module,libwebp)
