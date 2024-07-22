#include "texture_db.h"

#include "opensa_logger.h"
#include "opensa_objects.h"

#include "address_map.h"

#include "game_hooks.h"

namespace OpenSA::TextureDB_Runtime {
    uintptr_t GetTexture(const std::string_view texture_Name) {
        auto dbGetTexture = gGTASA_SO.make_Method<uintptr_t (*)(const char*)>(
            Address_IO_A64::GET_TEXTURE_FUNC_ADDR);

        // Getting the texure from the engine runtime database
        uintptr_t textureRwRef = (*dbGetTexture)(texture_Name.data());
        if (!textureRwRef) {
            Android_Error(gSA_logger, "GetTexture: Can't get a new texture called %s\n",
                texture_Name.data());
            return 0;
        }

        // Accessing the reference count field member of `struct RwTexture` (refCount)
        uint textureRefCount = *reinterpret_cast<uint*>(textureRwRef + 0x64);
        // Increment the reference count, because if don't: the Garbage Collector from the engine could
        // by any time destroy our texture!
        textureRefCount++;
        *reinterpret_cast<uint*>(textureRwRef + 0x64) = textureRefCount;

        Android_Info(gSA_logger, "GetTexture: New texture called (%s) created at %p\n",
                texture_Name.data(), textureRwRef);

        return textureRwRef;
    }
}
