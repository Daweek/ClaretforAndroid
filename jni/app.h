#define APP_H_INCLUDED
#include <jni.h>
#include <errno.h>
#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "Debug C Code", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "Debug C Code", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,"Debug C Code", __VA_ARGS__))


#ifdef __cplusplus
extern "C" {
#endif


#define WINDOW_DEFAULT_WIDTH    640
#define WINDOW_DEFAULT_HEIGHT   480

#define WINDOW_BPP              16


// The simple framework expects the application code to define these functions.
extern void appInit();
extern void appDeinit();
extern void free_mem();
extern float * appRender(JNIEnv* env, jobject obj,int width, int height, int dscuforce, int glon);
extern void motion(int x, int y);
extern void touch(int x, int y,int multi);
extern void Native_PosixThreads(JNIEnv* env, jobject obj);

//extern void appRender(long tick, int width, int height);

/* Value is non-zero when application is alive, and 0 when it is closing.
 * Defined by the application framework.
 */
extern int gAppAlive;
extern int gInitiated;
extern int Xpos,Ypos;

/////For Posix Threads taken from Android C++ with NDK chapter 7 source code

#undef com_apress_threads_MainActivity_MODE_PRIVATE
#define com_apress_threads_MainActivity_MODE_PRIVATE 0L
#undef com_apress_threads_MainActivity_MODE_WORLD_READABLE
#define com_apress_threads_MainActivity_MODE_WORLD_READABLE 1L
#undef com_apress_threads_MainActivity_MODE_WORLD_WRITEABLE
#define com_apress_threads_MainActivity_MODE_WORLD_WRITEABLE 2L
#undef com_apress_threads_MainActivity_MODE_APPEND
#define com_apress_threads_MainActivity_MODE_APPEND 32768L
#undef com_apress_threads_MainActivity_MODE_MULTI_PROCESS
#define com_apress_threads_MainActivity_MODE_MULTI_PROCESS 4L
#undef com_apress_threads_MainActivity_BIND_AUTO_CREATE
#define com_apress_threads_MainActivity_BIND_AUTO_CREATE 1L
#undef com_apress_threads_MainActivity_BIND_DEBUG_UNBIND
#define com_apress_threads_MainActivity_BIND_DEBUG_UNBIND 2L
#undef com_apress_threads_MainActivity_BIND_NOT_FOREGROUND
#define com_apress_threads_MainActivity_BIND_NOT_FOREGROUND 4L
#undef com_apress_threads_MainActivity_BIND_ABOVE_CLIENT
#define com_apress_threads_MainActivity_BIND_ABOVE_CLIENT 8L
#undef com_apress_threads_MainActivity_BIND_ALLOW_OOM_MANAGEMENT
#define com_apress_threads_MainActivity_BIND_ALLOW_OOM_MANAGEMENT 16L
#undef com_apress_threads_MainActivity_BIND_WAIVE_PRIORITY
#define com_apress_threads_MainActivity_BIND_WAIVE_PRIORITY 32L
#undef com_apress_threads_MainActivity_BIND_IMPORTANT
#define com_apress_threads_MainActivity_BIND_IMPORTANT 64L
#undef com_apress_threads_MainActivity_BIND_ADJUST_WITH_ACTIVITY
#define com_apress_threads_MainActivity_BIND_ADJUST_WITH_ACTIVITY 64L
#undef com_apress_threads_MainActivity_CONTEXT_INCLUDE_CODE
#define com_apress_threads_MainActivity_CONTEXT_INCLUDE_CODE 1L
#undef com_apress_threads_MainActivity_CONTEXT_IGNORE_SECURITY
#define com_apress_threads_MainActivity_CONTEXT_IGNORE_SECURITY 2L
#undef com_apress_threads_MainActivity_CONTEXT_RESTRICTED
#define com_apress_threads_MainActivity_CONTEXT_RESTRICTED 4L
#undef com_apress_threads_MainActivity_RESULT_CANCELED
#define com_apress_threads_MainActivity_RESULT_CANCELED 0L
#undef com_apress_threads_MainActivity_RESULT_OK
#define com_apress_threads_MainActivity_RESULT_OK -1L
#undef com_apress_threads_MainActivity_RESULT_FIRST_USER
#define com_apress_threads_MainActivity_RESULT_FIRST_USER 1L
#undef com_apress_threads_MainActivity_DEFAULT_KEYS_DISABLE
#define com_apress_threads_MainActivity_DEFAULT_KEYS_DISABLE 0L
#undef com_apress_threads_MainActivity_DEFAULT_KEYS_DIALER
#define com_apress_threads_MainActivity_DEFAULT_KEYS_DIALER 1L
#undef com_apress_threads_MainActivity_DEFAULT_KEYS_SHORTCUT
#define com_apress_threads_MainActivity_DEFAULT_KEYS_SHORTCUT 2L
#undef com_apress_threads_MainActivity_DEFAULT_KEYS_SEARCH_LOCAL
#define com_apress_threads_MainActivity_DEFAULT_KEYS_SEARCH_LOCAL 3L
#undef com_apress_threads_MainActivity_DEFAULT_KEYS_SEARCH_GLOBAL
#define com_apress_threads_MainActivity_DEFAULT_KEYS_SEARCH_GLOBAL 4L




#ifdef __cplusplus
}
#endif



