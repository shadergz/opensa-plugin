#include <pthread.h>

#include "opensa_logger.h"
#include "opensa_objects.h"

#include "Address_Map.h"

#include "game/Game_Hooks.h"

namespace OpenSA_Cortex {
    void __Apply_Patch_Level4() {
        Android_Info(gSA_logger, "Applying patches level (4) inside thread %ld\n", pthread_self());

        Address_IO_A64::gExtStorage_path = 
            gGTASA_SO.make_Object<const char*>(Address_IO_A64::STORAGE_PATH_BASE_ADDR);

        // Should be something like: /storage/emulated/0/...
        Android_Info(gSA_logger, "<HOOK> Android Storage Path found: (%s)\n", 
            Address_IO_A64::gExtStorage_path);
        gSA_logger.setups_logFile(Address_IO_A64::gExtStorage_path, 
            "OpenSA/LogSA.txt");
    }

}
