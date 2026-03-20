#pragma once

#include <asm/memcpy.h>
#include <libtoncar/toncar.h>
#include <mgba/logger.h>
#include <sprite.h>
#include <toncar.h>

#include <array>
#include <cstdint>

#include "gba-assert.h"

namespace toncar {

class ObjectVram final {
 private:
  class Charblock final {
   public:
    constexpr explicit Charblock(size_t offset) : offset_{offset} {}

    void LoadTiles(const Sprite& sprite) {
      MGBA_ASSERT(next_tile_id_ + sprite.TilesCount() < kMaxTileId);
      memcpy32(reinterpret_cast<uint16_t*>(kBaseAddress + offset_ +
                                           (next_tile_id_ * constants::kSpriteTileSizeBytes)),
               sprite.Data().data(),
               sprite.Data().size() / 4);
      next_tile_id_ += sprite.TilesCount();
    }

   private:
    static constexpr uintptr_t kBaseAddress{memory::kVram};
    size_t offset_;

    static constexpr uint16_t kMaxTileId{512};

    /// Naive bump allocator. Crashes when reaches kMaxTileId.
    uint16_t next_tile_id_{0};
  };

 public:
  static ObjectVram& Instance() {
    static ObjectVram vram{};
    return vram;
  }

  Charblock& GetCharblock(uint8_t index) {
    MGBA_ASSERT(index <= 5);
    return charblocks_[index];
  }

  Charblock& BackgroundCharblock(uint8_t index) {
    MGBA_ASSERT(index <= 3);
    return charblocks_[index];
  }

  Charblock& SpriteCharblock(uint8_t index) {
    MGBA_ASSERT(index <= 1);
    return charblocks_[kSpriteCharblockIndex + index];
  }

 private:
  ObjectVram() = default;

  std::array<Charblock, 6> charblocks_{Charblock{0x000000},
                                       Charblock{0x004000},
                                       Charblock{0x008000},
                                       Charblock{0x00C000},
                                       Charblock{0x010000},
                                       Charblock{0x014000}};

  static constexpr uint8_t kSpriteCharblockIndex{4};
};

}  // namespace toncar
