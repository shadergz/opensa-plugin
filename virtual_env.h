#ifndef VIRTUAL_ENV_H
#define VIRTUAL_ENV_H

#include <jni.h>

namespace OpenSA {
    /* Toast message duration 
     * https://developer.android.com/guide/topics/ui/notifiers/toasts?hl=en
    */
    enum Toast_Duration { TOAST_SHORT = 0, TOAST_LONG = 1 };

    class JVM_Objects {
        public:
        jobject mThizContext = nullptr;
        jclass mToast_Class = nullptr;
        jmethodID mToast_MakeTextID = nullptr; 

        void Init_Load_Objects(int* env_Status, jobject javaContext);
        void SpawnToast(const char* toast_Message, Toast_Duration toast_Duration);
    };
    
}

#endif
