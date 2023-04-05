#include <pthread.h>

#include "Game_Hooks.h"

#include "opensa_logger.h"
#include "opensa_objects.h"

int gLaunch_Ret = 0;

namespace OpenSA_Cortex {
    void __Apply_Patch_Level4() {
        Android_Info(gMAIN_SA_Logger, gLaunch_Ret, "Applying patches level (4) inside thread %ld\n", pthread_self());
    }

}
