#include <jni.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <android/log.h>
#include <stdint.h>
#include "app.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>



extern "C" {
int   gAppAlive   = 1;
int   gInitiated	= 0;
int Xpos,Ypos;

static int  sWindowWidth  = 700;
static int  sWindowHeight = 1280;
static int  sDemoStopped  = 0;
static long sTimeOffset   = 0;
static int  sTimeOffsetInit = 0;
static long sTimeStopped  = 0;

////////////////////For Native Threads//////////////////////////
// Native worker thread arguments
struct NativeWorkerArgs
{
	jint id;
	jint iterations;
};

// Method ID can be cached
static jmethodID gOnNativeMessage = NULL;

// Java VM interface pointer
static JavaVM* gVm = NULL;

// Global reference to object
static jobject gObj = NULL;

// Mutex instance
static pthread_mutex_t mutex;

/////////////////////////////////////////////////////////////////
///////////////////For Native Threads/////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

 void  nativeInit (JNIEnv* env,jobject obj)
{
	// Initialize mutex
	if (0 != pthread_mutex_init(&mutex, NULL))
	{
		// Get the exception class
		jclass exceptionClazz = env->FindClass(
				"java/lang/RuntimeException");

		// Throw exception
		env->ThrowNew(exceptionClazz, "Unable to initialize mutex");
		goto exit;
	}

	// If object global reference is not set
	if (NULL == gObj)
	{
		// Create a new global reference for the object
		gObj = env->NewGlobalRef(obj);

		if (NULL == gObj)
		{
			goto exit;
		}
	}

exit:
	return;
}

JNIEXPORT void JNICALL Java_com_edgar_claret_ClaretActivity_nativeFree (JNIEnv* env,jobject obj)
{
	// If object global reference is set

	free_mem();

	if (NULL != gObj)
	{
		// Delete the global reference
		env->DeleteGlobalRef(gObj);
		gObj = NULL;
	}

	// Destory mutex
	if (0 != pthread_mutex_destroy(&mutex))
	{
		// Get the exception class
		jclass exceptionClazz = env->FindClass(
				"java/lang/RuntimeException");

		// Throw exception
		env->ThrowNew(exceptionClazz, "Unable to destroy mutex");
	}
}
//////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////


static long
_getTime(void)
{
    struct timeval  now;

    gettimeofday(&now, NULL);
    return (long)(now.tv_sec*1000 + now.tv_usec/1000);
}

void ToJavaForce(JNIEnv* env, jobject obj, const char* str){

	if(str == NULL)return;
	jstring jstr = env->NewStringUTF(str);

	jclass clazz = env->FindClass("com/edgar/claret/ClaretActivity");
	jmethodID calcnacl = env->GetMethodID(clazz,"calcnacl", "(Ljava/lang/String;)V");
	env->CallVoidMethod(obj,calcnacl,jstr);
	//env->ReleaseStringUTFChars(jstr,str);
}


/* Call to initialize the graphics state */
JNIEXPORT void JNICALL
Java_com_edgar_claret_DemoRenderer_nativeInit( JNIEnv*  env , jobject obj)
{
		//importGLInit();
    appInit();
    gAppAlive  = 1;
    LOGE("NAtive Init......... :D");
}

JNIEXPORT void JNICALL
Java_com_edgar_claret_DemoRenderer_nativeResize( JNIEnv*  env, jobject  thiz, jint w, jint h )
{
    sWindowWidth  = w;
    sWindowHeight = h;
    LOGI("Claret ES resize w=%d h=%d", w, h);
}

/* Call to finalize the graphics state */
JNIEXPORT void JNICALL
Java_com_edgar_claret_DemoRenderer_nativeDone( JNIEnv*  env, jobject obj )
{
    appDeinit();
    //importGLDeinit();
    nativeInit(env,obj);
    LOGE("Native DOne....... :(");


}

/* This is called to indicate to the render loop that it should
 * stop as soon as possible.
 */

void _pause()
{

	LOGE("Pause..... :(");

}

void _resume()
{
  /* we resumed the animation, so adjust the time offset
   * to take care of the pause interval. */
	LOGE("Resume..... :)");

}


JNIEXPORT void JNICALL
Java_com_edgar_claret_DemoGLSurfaceView_nativeTogglePauseResume( JNIEnv*  env, jobject  thiz, jfloat posx, jfloat posy, jint multi )
{

	touch((int)posx,(int)posy, multi);

}

JNIEXPORT void JNICALL
Java_com_edgar_claret_DemoGLSurfaceView_nativeMotion(JNIEnv*  env, jobject  obj, jfloat posx, jfloat posy )
{

	motion((int)posx,(int)posy);
}

JNIEXPORT void JNICALL
Java_com_edgar_claret_DemoGLSurfaceView_nativePause( JNIEnv*  env, jobject obj )
{
    _pause();
}

JNIEXPORT void JNICALL
Java_com_edgar_claret_DemoGLSurfaceView_nativeResume( JNIEnv*  env, jobject obj )
{
    _resume();
}

/* Call to render the next GL frame */
JNIEXPORT jfloatArray JNICALL
Java_com_edgar_claret_DemoRenderer_nativeRender( JNIEnv*  env ,  jobject  obj, jint dscuforce, jint glon)
{

		jfloatArray result;
		result = env->NewFloatArray(5);
		if(result == NULL){
			LOGE("Native Float Array NULL...");
			exit(0);
		}

		env->SetFloatArrayRegion(result,0,4,appRender(env,obj,sWindowWidth, sWindowHeight,dscuforce,glon));
    return result;
    //LOGE("Native REnder.......");

}





}
