#include <jni.h>
#include <pthread.h>

#include <cstdint>
/* #include <memory> */

#include <hookrt/object.h>

#include <opensa_logger.h>

using namespace hookrt::info;
using namespace hookrt::object;

typedef pthread_t worker_thread_t;

/* std::shared_ptr<Client_Log::OpenSA_Logger> MAIN_SA_Logger; */
static Client_Log::OpenSA_Logger MAIN_SA_Logger;

static worker_thread_t main_thread = 0, hook_thread = 0;

static constexpr const char* GTASA_NATIVE_OBJECT = "libGTASA.so";

class GTASA_Native_Object : public Native_Object {
public:
    GTASA_Native_Object(Native_Info& info) {
        
    }
    GTASA_Native_Object() {}
};

static GTASA_Native_Object gNative_GTASA_object;

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

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) 
{
    /* Searching for the native GTASA library */
    Native_Info hooked_LibGTASA("libGTASA.so");
    /* This copy is done here, because we won't that the search for libGTASA
     * occurs outside JNI_OnLoad event by functions like: pthread_atfork; 
     * __cxa_finalize@plt or inside similar functions.
    */
    gNative_GTASA_object = hooked_LibGTASA;
    /* JNI_OnLoad function must returns the JNI needed version */
    return JNI_VERSION_1_6;
}

