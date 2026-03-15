#pragma once

#include <asm/memcpy.h>
#include <libtoncar/toncar.h>
#include <panic.h>
#include <sprite.h>

#include <array>
#include <cstdint>

namespace toncar {

class Vram final {
 private:
  class Charblock final {
   public:
    constexpr explicit Charblock(size_t offset) : offset_{offset} {}

    void LoadTiles(const Sprite& sprite) const {
      memcpy32(reinterpret_cast<uint16_t*>(kBaseAddress + offset_),
               sprite.Data().data(),
               sprite.Data().size() / 4);
    }

   private:
    static constexpr uintptr_t kBaseAddress{memory::kVram};
    size_t offset_;
  };

 public:
  static Vram& Instance() {
    static Vram vram{};
    return vram;
  }

  Charblock& GetCharblock(uint8_t index) {
    GBA_ASSERT(index <= 5);
    return charblocks_[index];
  }

  Charblock& BackgroundCharblock(uint8_t index) {
    GBA_ASSERT(index <= 3);
    return charblocks_[index];
  }

  Charblock& SpriteCharblock(uint8_t index) {
    GBA_ASSERT(index <= 1);
    return charblocks_[kSpriteCharblockIndex + index];
  }

 private:
  Vram() = default;

  std::array<Charblock, 6> charblocks_{Charblock{0x000000},
                                       Charblock{0x004000},
                                       Charblock{0x008000},
                                       Charblock{0x00C000},
                                       Charblock{0x010000},
                                       Charblock{0x01400}};

  static constexpr uint8_t kSpriteCharblockIndex{4};
};

}  // namespace toncar
