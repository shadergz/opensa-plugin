#include <cstdint>
#include <cassert>

#include <jni.h>
#include <pthread.h>

#include "opensa_logger.h"
#include "opensa_objects.h"

#include "game/Game_Hooks.h"

#include "SA_Config.h"

OpenSA::OpenSA_Logger gMAIN_SA_Logger;
OpenSA::JVM_Objects gThread_Objects;
JNIEnv* gMAIN_Env = nullptr;

typedef pthread_t worker_thread_t;
pthread_mutex_t gHook_Mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t gHook_Cond = PTHREAD_COND_INITIALIZER;

static worker_thread_t main_thread = 0, hook_thread = 0;

static int gJVM_Status;
static ssize_t gLog_Result;

Hook_I32_t GTASA_Native_Object::event_Release(Hook_Event_t status, const char* message) {
    Hook_I32_t result = -1;
    switch(status) {
    case HOOK_SUCCESS: Android_Success(gMAIN_SA_Logger, result, message); break;
    case HOOK_INFO: Android_Info(gMAIN_SA_Logger, result, message); break;
    case HOOK_FAILED: Android_Error(gMAIN_SA_Logger, result, message); break;
    }
    return result;
}

static constexpr const char* GTASA_NATIVE_OBJECT = "libGTASA.so";

/* When game initialize this function will be called by the 
 * JVM from Android Runtime.
 * The VM calls JNI_OnLoad when the native library is mapped into memory

 * [Useful links]
 * https://android.googlesource.com/platform/dalvik/+/eclair-release/vm/Native.c
 * Keywords: dvmLoadNativeCode; dlopen
 * [Code Parts]
 * handle = dlopen(pathName, RTLD_LAZY);
 * ...
 * vonLoad = dlsym(handle, "JNI_OnLoad");
 *...
 * LOGD("No JNI_OnLoad found in %s %p\n", pathName, classLoader);
 * ...
 * LOGV("+++ calling JNI_OnLoad(%s)\n", pathName)
 * version = (*func)(gDvm.vmList, NULL);
 * ...
 * LOGV("+++ finished JNI_OnLoad %s\n", pathName);
*/

namespace OpenSA_Threads {

    struct Thread_Data {
        void* __Thread_Content;
    };

    static __attribute__((visibility("hidden"))) void* Plugin_StartMAIN(void* SAVED_PTR) {
        __attribute__((unused)) auto* thread_info = static_cast<Thread_Data*>(SAVED_PTR);
        /* Start the hook thread now, we can continues the execution after his return */
        
        /* Fixing wasted wakeups, the mutex must be locked before the signal, and unlocks after the 
         * signal function call 
        */
        pthread_mutex_lock(&gHook_Mutex);
        
        pthread_cond_signal(&gHook_Cond);

        pthread_mutex_unlock(&gHook_Mutex);
        /* Waiting until the hook thread begin in finished state */
        pthread_join(hook_thread, nullptr);

        static const struct timespec sleep_nano = {
            .tv_sec = 4,
        };
        while (true) {
            Android_Info(gMAIN_SA_Logger, gLog_Result, "Inside MAIN\n");
            nanosleep(&sleep_nano, nullptr);
        }

        return static_cast<void*>(thread_info);
    }

    static __attribute__((visibility("hidden"))) void* INIT_Hook_SYSTEM(void* SAVED_PTR) {
        __attribute__((unused)) auto* thread_info = static_cast<Thread_Data*>(SAVED_PTR);

        /* Acquire the lock, Release the lock and wait for the main thread be called */
        pthread_mutex_lock(&gHook_Mutex);
        pthread_cond_wait(&gHook_Cond, &gHook_Mutex);

        OpenSA_Cortex::__Apply_Patch_Level4();

        pthread_mutex_unlock(&gHook_Mutex);
        Android_Info(gMAIN_SA_Logger, gLog_Result, "Hook thread has finished\n");
        return static_cast<void*>(thread_info);
    }

};

extern "C" JNIEXPORT void JNICALL Java_com_rockstargames_gtasa_GTASA_OpenSA_1Resume(JNIEnv* env, jobject GTASA_Context) {
    Android_Info(gMAIN_SA_Logger, gLog_Result, "Resume has been called!\n");
    gThread_Objects.Init_Load_Objects(&gJVM_Status, GTASA_Context);
    gThread_Objects.SpawnToast("OpenSA is running...", OpenSA::Toast_Duration::TOAST_SHORT);

    pthread_create(&main_thread, nullptr, OpenSA_Threads::Plugin_StartMAIN, nullptr);
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* unused) {

    Android_Info(gMAIN_SA_Logger, gLog_Result, 
        "OpenSA loaded into heap! and was hooked by Android Runtime! " 
        "Compiled at: %s:%s\n", __DATE__, __TIME__);
    
    gJVM_Status = vm->GetEnv(reinterpret_cast<void**>(&gMAIN_Env), JNI_VERSION_1_6);
    if (gJVM_Status < 0) {
        Android_Error(gMAIN_SA_Logger, gLog_Result, "Failed to get the JNI env from the main process, assuming native thread\n");
        gJVM_Status = vm->AttachCurrentThread(&gMAIN_Env, nullptr);
        if (gJVM_Status < 0) {
            Android_Error(gMAIN_SA_Logger, gLog_Result, "For some reason, your device can't attach the current thread to JNI env\n");
            return JNI_ERR;
        }
    }

    /* Searching for the native GTASA library */
    gLib_GTASA_Native.find_Base_Address("libGTASA.so");
    /* This is done here, because we won't that the search for libGTASA
     * occurs outside JNI_OnLoad event by functions like: pthread_atfork; 
     * __cxa_finalize@plt or inside similar functions.
    */
#if defined(OPENSA_DEBUG)
    Android_Success(gMAIN_SA_Logger, gLog_Result, "libGTASA.so image base address: %#lx\n", 
        gLib_GTASA_Native.get_Native_Addr());
#endif
    /* Starting the Hook thread, this thread will be locked until the VM call the OpenSA_Resume function */
    pthread_create(&hook_thread, nullptr, OpenSA_Threads::INIT_Hook_SYSTEM, nullptr);

    /* JNI_OnLoad function must returns the JNI needed version */
    return JNI_VERSION_1_6;
}

