#include <pthread.h>

#include "opensa_logger.h"
#include "opensa_objects.h"

#include "Address_Map.h"

#include "game/Game_Hooks.h"

int gLaunch_Ret = 0;

namespace OpenSA_Cortex {
    void __Apply_Patch_Level4() {
        Android_Info(gMAIN_SA_Logger, gLaunch_Ret, "Applying patches level (4) inside thread %ld\n", pthread_self());

        gAddrMap_IO_extstorage_path = gLib_GTASA_Native.make_Object<const char*>(Address_IO_A64::STORAGE_PATH_BASE_ADDR);

        // Should be something like: /storage/emulated/0/...
        Android_Info(gMAIN_SA_Logger, gLaunch_Ret, "<HOOK> Android Storage Path found: '%s'\n", gAddrMap_IO_extstorage_path);
        gMAIN_SA_Logger.setups_logFile(gAddrMap_IO_extstorage_path, "OpenSA/LogSA.txt");
    }

}
