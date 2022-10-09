#include <cstdint>
#include <cassert>

#include <jni.h>
#include <pthread.h>

#include <hookrt/object.h>
#include <hookrt/info.h>

#include <opensa_logger.h>

using namespace hookrt::object;
using namespace hookrt::info;

typedef pthread_t worker_thread_t;

static Client_Log::OpenSA_Logger MAIN_SA_Logger;

static worker_thread_t main_thread = 0, hook_thread = 0;
static ssize_t g_log_result;

static constexpr const char* GTASA_NATIVE_OBJECT = "libGTASA.so";

struct GTASA_Native_Object : public Native_Object {
public:
    GTASA_Native_Object() {}
    ~GTASA_Native_Object() {}
    Hook_I32_t native_Notify(Notify_Event_t status, const char* message);

};

Hook_I32_t GTASA_Native_Object::native_Notify(Notify_Event_t status, const char* message) {
    Hook_I32_t result = -1;
    switch(status) {
    case HOOK_SUCCESS: Android_Success(MAIN_SA_Logger, result, message); break;
    case HOOK_INFO: Android_Info(MAIN_SA_Logger, result, message); break;
    case HOOK_FAILED: Android_Error(MAIN_SA_Logger, result, message); break;
    }
    return result;
}

static GTASA_Native_Object gNative_GTASA_Object;

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
        return static_cast<void*>(thread_info);
    }

    static __attribute__((visibility("hidden"))) void* INIT_Hook_SYSTEM(void* SAVED_PTR) {
        __attribute__((unused)) auto* thread_info = static_cast<Thread_Data*>(SAVED_PTR);

        static const struct timespec sleep_nano = {
            .tv_sec = 2,
        };

        while (true) {
            nanosleep(&sleep_nano, nullptr);
            Android_Info(MAIN_SA_Logger, g_log_result, "OpenSA is running...\n");
        }

        return static_cast<void*>(thread_info);
    }

};

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {

    Android_Info(MAIN_SA_Logger, g_log_result, 
        "OpenSA loaded into heap! and was hooked by Android Runtime! " 
        "Compiled at: %s:%s\n", __DATE__, __TIME__);
    
    /* Searching for the native GTASA library */
    gNative_GTASA_Object.find_Base_Address("libGTASA.so");
    /* This is done here, because we won't that the search for libGTASA
     * occurs outside JNI_OnLoad event by functions like: pthread_atfork; 
     * __cxa_finalize@plt or inside similar functions.
    */
    Android_Success(MAIN_SA_Logger, g_log_result, "libGTASA.so image base address: %#lx\n", 
        gNative_GTASA_Object.get_Native_Addr());

    pthread_create(&main_thread, nullptr, OpenSA_Threads::INIT_Hook_SYSTEM, nullptr);
    pthread_create(&hook_thread, nullptr, OpenSA_Threads::Plugin_StartMAIN, nullptr);

    /* JNI_OnLoad function must returns the JNI needed version */
    return JNI_VERSION_1_6;
}

