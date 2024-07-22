#ifndef ADDRESS_MAP_H
#define ADDRESS_MAP_H

#include <cstdint>

namespace Address_IO_A64 {
    extern const char* gExtStorage_path;

    enum {
        // Reference name: StorageBaseRootBuffer
        // REF COUNT  = 4; SOURCE = IMPORTED; NAMESPACE = GLOBAL; 
        // REF ACCESS = PTR_StorageBaseRootBuffer_00951328
        STORAGE_PATH_BASE_ADDR = 0x008b46a8,

        GET_TEXTURE_FUNC_ADDR  = 0x00286718,
    };
};

#endif


