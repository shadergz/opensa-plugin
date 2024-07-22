#ifndef TEXTUREDB_H
#define TEXTUREDB_H

#include <string_view>

namespace OpenSA::TextureDB_Runtime {
    uintptr_t GetTexture(const std::string_view texture_Name);
}

#endif

