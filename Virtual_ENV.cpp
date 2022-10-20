#include "opensa_logger.h"
#include "opensa_objects.h"

static int gMessageStatus = 0;

namespace OpenSA {

    void JVM_Objects::Init_Load_Objects(int* env_Status, jobject javaContext) {
        Android_Info(gMAIN_SA_Logger, gMessageStatus, "Loading some JNI methods\n");
        /* Loading Toast class and methods */
        if (javaContext == nullptr)
        {
            Android_Error(gMAIN_SA_Logger, gMessageStatus, "The context object is nullptr, this is an cruel error\n");
            return;
        }
        mThizContext = javaContext;

        mToast_Class = gMAIN_Env->FindClass("android/widget/Toast");
        mToast_MakeTextID = gMAIN_Env->GetStaticMethodID(mToast_Class, "makeText", "(Landroid/content/Context;Ljava/lang/CharSequence;I)Landroid/widget/Toast;");
        
        if (mToast_MakeTextID == nullptr)
            Android_Error(gMAIN_SA_Logger, gMessageStatus, "Can't locate the Toast method, Toast methods are disable\n");
        
    }

    void JVM_Objects::SpawnToast(const char* toast_Message, Toast_Duration toast_Duration) {
        if (mToast_MakeTextID == nullptr)
            return;
        
        Android_Info(gMAIN_SA_Logger, gMessageStatus, "Spawning the Toast message: %s\n", toast_Message);

        jstring tObject_Message = gMAIN_Env->NewStringUTF(toast_Message);
        if (gMAIN_Env->ExceptionOccurred()) {
            gMAIN_Env->ExceptionDescribe();
            return;
        }
        jobject toastObject = gMAIN_Env->CallStaticObjectMethod(mToast_Class, mToast_MakeTextID, mThizContext, 
            tObject_Message, toast_Duration);
        jmethodID toastShow = gMAIN_Env->GetMethodID(mToast_Class, "show", "()V");
        gMAIN_Env->CallVoidMethod(toastObject, toastShow);
        /* Deleting the reference, the JVM atomically delete own objects when the native method returns, 
         * but at this moment, this resource is self controlled but us, this means that we need to automaticaly free
         * the memory for avoid run out.
        */
        gMAIN_Env->DeleteLocalRef(tObject_Message);
    }

}
