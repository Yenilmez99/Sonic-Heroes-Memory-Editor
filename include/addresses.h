#pragma once

#include <cstdint>
#include <vector>

namespace shmemeditor {

    namespace addresses {

        constexpr uintptr_t Ring = 0x009DD70C;
        constexpr uintptr_t PowerBase = 0x00A4C268;
        constexpr uintptr_t Time = 0x009DD708;
        constexpr uintptr_t Point = 0x009DD6C0;
        constexpr uintptr_t TeamBlastBar = 0x009DD72C;
        constexpr uintptr_t TeamBlastState = 0x009DD73C;
        constexpr uintptr_t FlyBar = 0x00789FE4;
        constexpr uintptr_t CharacterOverwriteArea = 0x008BEB84;
        constexpr uintptr_t SpeedCharacterBase = 0x00A4B1B0;
        constexpr uintptr_t ActiveRoleBase = 0x00A778AC;
        constexpr uintptr_t Lives = 0x009DD74C;
        constexpr uintptr_t GameState = 0x008D66F0;

    }

    namespace offsets {
        const std::vector<ptrdiff_t> Power = {0x208};
        const std::vector<ptrdiff_t> CharacterPosition = {0x114};
        const std::vector<ptrdiff_t> ActiveRole = {0x18,0x148};
    }

}