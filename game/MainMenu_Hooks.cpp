
#include "Game_Hooks.h"

namespace OpenSA {

    void GTASA_MobileMenu__::_OpenSA_OnLoadMethod(void* in_Game_Menu) {
        assert(mOriginal_LoadMethod != nullptr);
        mOriginal_LoadMethod(in_Game_Menu);
    }
}
