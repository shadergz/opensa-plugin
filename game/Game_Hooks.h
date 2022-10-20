#ifndef GAME_HOOKS_H
#define GAME_HOOKS_H

#include <cassert>

#include <hookrt/object.h>
#include <hookrt/info.h>

using namespace hookrt::object;
using namespace hookrt::info;

struct GTASA_Native_Object : public Native_Object {
public:
    GTASA_Native_Object() {}
    ~GTASA_Native_Object() {}
    Hook_I32_t native_Notify(Notify_Event_t status, const char* message);

};

extern GTASA_Native_Object gLib_GTASA_Native;

/* Game Main Menu class 
 * This class will be used for hooking the in game main menu,
 * when the user is in the Main Menu, his can select from starts a New game, open
 * the Settings Menu and more.
*/

namespace OpenSA {
    class GTASA_MobileMenu__ {

        void _OpenSA_OnLoadMethod(void* in_Game_Menu);

        void (*mOriginal_LoadMethod)(void* game_Menu) = nullptr;

    };

    extern class GTASA_MobileMenu__* __MobileMenu;
}

namespace OpenSA_Cortex {
    /* Some patches needs to be applied for the correctness maintenance and 
     * functionality provide from the game, OpenSA was divide this patches in a group called "Cortex Level",
     * at level 4 the needs patches are: ""
    */
    void __Apply_Patch_Level4();
}

#endif
