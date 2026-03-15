#pragma once

#include <asm/memcpy.h>
#include <sprite.h>
#include <toncar.h>
#include <zone.h>

#include <cstddef>
#include <cstdint>

#include "libtoncar/toncar.h"

namespace toncar {

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

}  // namespace toncar
