#pragma once

#include <cstdint>

namespace toncar {
namespace constants {

constexpr uint16_t kTileSizePixels{8 * 8};
/// 8px*8px;4bpp -> 32 bytes for a tile
constexpr uint16_t kSpriteTileSizeBytes{kTileSizePixels / 2};

}  // namespace constants

namespace memory {

constexpr uintptr_t kIo{0x04000000};
constexpr uintptr_t kPalram{0x05000000};
constexpr uintptr_t kVram{0x06000000};
constexpr uintptr_t kOam{0x07000000};

}  // namespace memory
}  // namespace toncar
